/*
 * Mattersplatter - a compiler & interpreter for the Brainf*ck language.
 * Copyright (C) 2021 Maxwell R. Haley
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mattersplatter.h"

enum subroutine_flags {
SR_POINTER_RIGHT = 1 << 0,
SR_POINTER_LEFT = 1 << 1,
SR_PRINT = 1 << 2,
SR_READ = 1 << 3,
};

struct source_block {
	char *block;
	size_t len;
};

struct source {
	struct source_block global;
	struct source_block data;
	struct source_block bss;
	struct source_block text;
	struct source_block start;
};

/* Global scaffolding text. */
static char *global_start;
static size_t global_start_len;

/* Data section skeleton text. */
static char *data_section;
static size_t data_section_len;
static char *size_def;
static size_t size_def_len;

/* BSS skeleton text.  */
static char *bss_section;
static size_t bss_section_len;

/* Text section skeketon text. */
static char *text_section;
static size_t text_section_len;
static char *sr_pointer_right;
static size_t sr_pointer_right_len;
static char *call_sr_pointer_right;
static size_t call_sr_pointer_right_len;
static char *sr_pointer_left;
static size_t sr_pointer_left_len;
static char *call_sr_pointer_left;
static size_t call_sr_pointer_left_len;
static char *increment;
static size_t increment_len;
static char *decrement;
static size_t decrement_len;
static char *sr_print;
static size_t sr_print_len;
static char *call_sr_print;
static size_t call_sr_print_len;
static char *sr_read;
static size_t sr_read_len;
static char *call_sr_read;
static size_t call_sr_read_len;
static char *loop_start;
static size_t loop_start_len;
static char *loop_end;
static size_t loop_end_len;
static char *done;
static size_t done_len;

/* Start section skeketon text. */
static char *start_section;
static size_t start_section_len;

/* Source block declarations. */
static struct source nasm_src;
static struct source_block global;
static struct source_block data;
static struct source_block bss;
static struct source_block text;
static struct source_block start;

static size_t
source_block_create(struct source_block *src_blk, const char *block,
		    const size_t len)
{
	src_blk->block = calloc(len, sizeof(char *));

	if (src_blk->block == NULL) {
		return errno;
	}

	strncpy(src_blk->block, block, len);
	src_blk->len = len;

	return 0;
}

static void
source_blocks_destroy(const size_t count, ...)
{
	struct source_block *blk = NULL;
	va_list blks;
	va_start(blks, count);

	for (size_t i = 0; i < count; i++) {
		blk = va_arg(blks,  struct source_block *);
		blk->len = 0;
		free(blk->block);
	}

	va_end(blks);
}

static size_t
append_to_block(struct source_block *src_block, const char *string, size_t len)
{
	size_t new_len = len + src_block->len;
	src_block->block = realloc(src_block->block, new_len + 1);

	if (errno) {
		return errno;
	}
	strncat(src_block->block, string, len * sizeof(char *));
	src_block->len = new_len;
	return 0;
}

static struct matsplat_compilation_result
source_to_string(const struct source src)
{
	struct matsplat_compilation_result result;
	const size_t src_length =
		src.global.len
		+ src.data.len
		+ src.bss.len
		+ src.text.len
		+ src.start.len
		+ 1;

	result.source_code = calloc(src_length, sizeof(char *));
	result.source_code_len = src_length;

	strncat(result.source_code, src.global.block, src.global.len);
	strncat(result.source_code, src.data.block, src.data.len);
	strncat(result.source_code, src.bss.block, src.bss.len);
	strncat(result.source_code, src.text.block, src.text.len);
	strncat(result.source_code, src.start.block, src.start.len);

	return result;
}

static void
initialize_asm_values()
{
	/* Global scaffolding text. */
	global_start = "global _start\n";
	global_start_len = strlen(global_start);

	/* Data section skeleton text. */
	data_section = "section .data\n";
	data_section_len = strlen(data_section);
	size_def = "size: equ";
	size_def_len = strlen(size_def);

	/* BSS skeleton text.  */
	bss_section = "section .bss\n" "array: resb size\n";
	bss_section_len = strlen(bss_section);

	/* Text section skeketon text. */
	text_section = "section .text\n";
	text_section_len = strlen(text_section);
	sr_pointer_right =
		"pointer_right:\n"
		"cmp r9, size - 1\n"
		"je pointer_right_overflow\n"
		"inc r9\n"
		"ret\n"
		"pointer_right_overflow:\n"
		"mov r9, 0\n"
		"ret\n";
	sr_pointer_right_len = strlen(sr_pointer_right);
	call_sr_pointer_right = "call pointer_right\n";
	call_sr_pointer_right_len = strlen(call_sr_pointer_right);
	sr_pointer_left = "pointer_left:\n"
		"cmp r9, 0\n"
		"je pointer_left_overflow\n"
		"dec r9\n"
		"ret\n"
		"pointer_left_overflow:\n"
		"mov r9, size - 1\n"
		"ret\n";
	sr_pointer_left_len = strlen(sr_pointer_left);
	call_sr_pointer_left = "call pointer_left\n";
	call_sr_pointer_left_len = strlen(call_sr_pointer_left);
	increment = "inc byte [rdx + r9]\n";
	increment_len = strlen(increment);
	decrement = "dec byte [rdx + r9]\n";
	decrement_len = strlen(decrement);
	sr_print = "print:\n"
		"mov rax, 1\n"
		"mov rdi, 1\n"
		"add rdx, r9\n"
		"mov rsi, rdx\n"
		"mov rdx, 1\n"
		"syscall\n"
		"mov rdx, array\n"
		"ret\n";
	sr_print_len = strlen(sr_print);
	call_sr_print = "call print\n";
	call_sr_print_len = strlen(call_sr_print);
	sr_read = "read:\n"
		"add rdx, r9\n"
		"mov rax, 0\n"
		"mov rdi, 0\n"
		"mov rsi, rdx\n"
		"mov rdx, 1\n"
		"syscall\n"
		"mov rdx, array\n"
		"ret\n";
	sr_read_len = strlen(sr_read);
	call_sr_read = "call read\n";
	call_sr_read_len = strlen(call_sr_read);
	loop_start = "loop_%p:\n" "cmp byte [rdx + r9], 0\n" "je loop_%p_end\n";
	loop_start_len = strlen(loop_start);
	loop_end = "jnz loop_%p\n" "loop_%p_end:\n";
	loop_end_len = strlen(loop_end);
	done = "done:\n" "mov rax, 60\n" "xor rdi, rdi\n" "syscall\n";
	done_len = strlen(done);

	/* Start section skeketon text. */
	start_section = "_start:\n" "mov rdx, array\n" "mov r9, 0\n";
	start_section_len = strlen(start_section);

}

static int
initialize_source_blocks()
{
	int result = 0;
	/* Initialize source blocks. */
	if ((result = source_block_create(&global, global_start, global_start_len)
	     != 0)) {
		goto init_failure;
	}

	if ((result = source_block_create(&data, data_section, data_section_len))
	    != 0) {
		goto init_failure;
	}

	if ((result = source_block_create(&bss, bss_section, bss_section_len))
	    != 0) {
		goto init_failure;
	}

	if ((result = source_block_create(&text, text_section, text_section_len))
	    != 0) {
		goto init_failure;
	}

	if ((result = source_block_create(&start, start_section, start_section_len))
	    != 0) {
		goto init_failure;
	}

	return result;

init_failure:
	return result;
}

void
compile(struct matsplat_node *node, uint8_t *included_subroutines)
{
	enum matsplat_token t = node->token->type;
	char *temp = NULL;

	switch (t) {
		case POINTER_RIGHT:
			if ((*included_subroutines & SR_POINTER_RIGHT) == 0x0) {
				*included_subroutines |= SR_POINTER_RIGHT;
				append_to_block(&text, sr_pointer_right, sr_pointer_right_len);
			}

			append_to_block(&start, call_sr_pointer_right,
					call_sr_pointer_right_len);
			break;
		case POINTER_LEFT:
			if ((*included_subroutines & SR_POINTER_LEFT) == 0x0) {
				*included_subroutines |= SR_POINTER_LEFT;
				append_to_block(&text, sr_pointer_left, sr_pointer_left_len);
			}

			append_to_block(&start, call_sr_pointer_left,
					call_sr_pointer_left_len);
			break;
		case INCREMENT:
			append_to_block(&start, increment, increment_len);
			break;
		case DECREMENT:
			append_to_block(&start, decrement, decrement_len);
			break;
		case OUTPUT:
			if ((*included_subroutines & SR_PRINT) == 0x0) {
				*included_subroutines |= SR_PRINT;
				append_to_block(&text, sr_print,
						sr_print_len);
			}
			append_to_block(&start, call_sr_print,
					call_sr_print_len);
			break;
		case INPUT:
			if ((*included_subroutines & SR_READ) == 0x0) {
				*included_subroutines |= SR_READ;
				append_to_block(&text, sr_read,
						sr_read_len);
			}
			append_to_block(&start, call_sr_read,
					call_sr_read_len);
			break;
		case JUMP_FORWARD:
			temp = calloc(loop_start_len, sizeof(char *));
			sprintf(temp, loop_start, node, node);
			append_to_block(&start, temp, strlen(temp));
			free(temp);
			/* push_jump_stack(current, &jump_stack); */
			/* is_flow_left = true; */
			break;
		case END:
			append_to_block(&start, done, done_len);
			append_to_block(&start, "\n", 1);
			/* is_not_complete = false; */
			break;
		case JUMP_BACKWARDS:
		default:
			break;
	}

	if (t == END || t == JUMP_BACKWARDS) {
		return;
	} else if (t == JUMP_FORWARD) {
		compile(node->left_child, included_subroutines);

		temp = calloc(loop_end_len, sizeof(char *));
		sprintf(temp, loop_end, node, node);
		append_to_block(&start, temp, strlen(temp));
		free(temp);

		compile(node->right_child, included_subroutines);
		return;
	} else {
		compile(node->right_child, included_subroutines);
		return;
	}
}

struct matsplat_compilation_result
matsplat_compile(struct matsplat_node *ast, size_t memsize)
{
	struct matsplat_compilation_result result =
		{.source_code = NULL, .source_code_len = 0, .error_code = 0};

	initialize_asm_values();
	result.error_code = initialize_source_blocks();
	if (result.error_code != 0) {
		return result;
	}

	/* Add memory size as static data. */
	uintmax_t memsize_len = log10(memsize) + 1;
	char *temp = NULL;

	/*
	 * Add the lenght of the size static definition, the lenght of the
	 * memsize, and 2 (one for the space between `equ` and `memsize`, and
	 * one for the newline).
	 */
	temp = calloc(size_def_len + memsize_len + 2, sizeof(char *));
	sprintf(temp, "%s %lu\n", size_def, memsize);
	append_to_block(&data, temp, strlen(temp));
	free(temp);

	/* Parse the actual syntax tree. */
	uint8_t included_subroutines = 0x0;
	compile(ast, &included_subroutines);

	nasm_src.global = global;
	nasm_src.data = data;
	nasm_src.bss = bss;
	nasm_src.text = text;
	nasm_src.start = start;

	result = source_to_string(nasm_src);

	source_blocks_destroy(5, &global, &data, &bss, &text, &start);
	return result;
}

void
matsplat_compilation_result_destroy(struct matsplat_compilation_result result)
{
	if (result.source_code) {
		free(result.source_code);
	}

	result.source_code_len = 0;
	result.error_code = 0;
}
