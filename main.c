/*
 * Mattersplatter - a compiler & interpreter for the Brainf*ck language.
 * Copyright (C) 2021 Maxwell R. Haley <maxwell.r.haley@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "compiler.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"

static const char *usage_msg =
	"Usage: mattersplatter [-o outfile] [-v] [-d] filename\n"
	"       mattersplatter -b [-v] [-d] filename\n"
	"       mattersplatter -h\n"
	"\n"
	"       -b        \tRun in batch mode.\n"
	"       -d        \tShow debug output.\n"
	"       -h        \tDisplay this message.\n"
	"       -o outfile\tWrite output to outfile.\n"
	"       -v        \tShow verbose output.";

enum  options_result {
OPTIONS_OK,
OPTIONS_NO_FILE,
OPTIONS_FILE_TOO_LONG,
OPTIONS_OUT_FILE_TOO_LONG,
OPTIONS_MISSING_ARG,
OPTIONS_UNKNOWN_ARG,
};

enum options_mode {
MODE_COMPILER,
MODE_INTERPRETER,
MODE_HELP,
};

struct options {
	char in_file_name[PATH_MAX];
	char out_file_name[FILENAME_MAX];
	bool is_verbose;
	bool is_debug;
	enum options_result result;
	enum options_mode mode;
	char wrong_opt;
};

static struct options
options_create(int argc, char *argv[])
{
	struct options o = { .is_verbose = false, .is_debug = false };
	o.mode = MODE_COMPILER;
	int opt;
	while ((opt = getopt(argc, argv, ":bdho:v")) != -1) {
		switch (opt) {
			case 'b':
				o.mode = MODE_INTERPRETER;
				break;
			case 'd':
				o.is_debug = true;
				break;
			case 'h':
				o.mode = MODE_HELP;
				return o;
			case 'o':
				if (strlen(optarg) > FILENAME_MAX) {
					o.result = OPTIONS_OUT_FILE_TOO_LONG;
					return o;
				} else {
					strncpy(o.out_file_name, optarg, FILENAME_MAX);
				}
				break;
			case 'v':
				o.is_verbose = true;
				break;
			case ':':
				o.result = OPTIONS_MISSING_ARG;
				o.wrong_opt = optopt;
				return o;
			case '?':
				o.result = OPTIONS_UNKNOWN_ARG;
				o.wrong_opt = optopt;
				return o;
			default:
				break;
		}
	}

	if (argv[optind] == NULL) {
		o.result = OPTIONS_NO_FILE;
	} else if (strlen(argv[optind]) >= PATH_MAX) {
		o.result = OPTIONS_FILE_TOO_LONG;
	} else {
		strcpy(o.in_file_name, argv[optind]);
		o.result = OPTIONS_OK;
	}

	if (o.out_file_name[0] == '\0') {char in_file[PATH_MAX] = {0};
		strncpy(in_file, o.in_file_name, PATH_MAX);
		char bname[FILENAME_MAX];
		strncpy(bname, basename(in_file), FILENAME_MAX);
		uintptr_t last_period = (uintptr_t) strrchr(bname, '.');

		if (last_period) {
			/* Trim string and set */
			ptrdiff_t period_idx =
				last_period - (uintptr_t) bname;
			strncpy(o.out_file_name, bname, period_idx);

		} else {
			strncpy(o.out_file_name, "a.out", FILENAME_MAX);
		}
	}

	return o;
}

static intmax_t
load_file_to_buffer(char **buffer, const char *filename)
{
	intmax_t size = -1;

	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		goto load_file_to_buffer_error;
	}

	if(fseek(f, 0L, SEEK_END) == -1) {
		goto load_file_to_buffer_error;
	}

	size = ftell(f);
	if (size == -1) {
		goto load_file_to_buffer_error;
	}
	rewind(f);

	*buffer = calloc(size + 1, sizeof(char));
	if (*buffer == NULL) {
		goto load_file_to_buffer_error;
	}

	if (fread(*buffer, sizeof(char), size, f) != size * sizeof(char)) {
		goto load_file_to_buffer_error;
	}


	fclose(f);
	return size;

load_file_to_buffer_error:
	*buffer = NULL;
	if (f) {
		fclose(f);
	}
	return -1;
}

static size_t
write_assembly_to_disk(const struct compilation_result compr)
{
	size_t size;
	FILE *f = fopen("out.asm", "w");
	if (f == NULL) {
		goto write_assembly_to_disk_error;
	}

	size = fputs(compr.source_code, f);
	fflush(NULL);
	fclose(f);
	return size;

write_assembly_to_disk_error:
	if (f) {
		fclose(f);
	}
	return -1;
}

static void
print_timestamp()
{
	time_t ltm = time(NULL);
	struct tm *tm = localtime(&ltm);
	char *stamp = asctime(tm);
	stamp[strlen(stamp) - 1] = '\0';
	printf("[%s] ", stamp);
}


static void
printd_file(const char *contents, const char *file_name, const size_t size,
	    const struct options opts)
{
	if (opts.is_debug) {
		print_timestamp();
		printf("File %s (%lu bytes). In-memory content follows.\n",
		       file_name, size);
		printf("%s\n", contents);
	}
}

static void
printd_tokens(const struct token *tokens, const size_t size,
	      struct options opts)
{
	if (opts.is_debug) {
		print_timestamp();
		printf("The following is the output of the lexer.\n");
		for (size_t i = 0; i <= size; i++) {
			struct token t = tokens[i];
			struct token_human_readable thr =
				token_type_to_human_readable(t.type);
			printf("Token #%zu: %s \"%c\" (%" PRIuMAX ", %" PRIuMAX ")\n",
			       i,
			       thr.description,
			       thr.symbol,
			       t.column,
			       t.row);
		}
	}

}

static void
printf_v(const struct options opts, const char *format,  ...)
{
	if (opts.is_verbose) {
		va_list args;
		va_start(args, format);
		print_timestamp();
		vprintf(format, args);
	}
}

enum invoke_assembler_status {
INVOKE_SUCCESS,
INVOKE_NASM_FAIL,
INVOKE_LD_FAIL,
};

struct invoke_assembler_result {
	enum invoke_assembler_status status;
	char cmd_output[UINT8_MAX];
	int error_no;
};

static struct invoke_assembler_result
invoke_assembler(const char *out_name)
{
	struct invoke_assembler_result result = {0};
	const char nasm_cmd[] = "nasm -felf64 -g out.asm 2>&1";
	FILE *nasm_pipe = popen(nasm_cmd, "r");
	if (!nasm_pipe) {
		goto invoke_assembler_nasm_error;
	}

	char buf[UINT8_MAX];
	while (fgets(buf, 80, nasm_pipe) != NULL) {
		assert(strlen(buf) <= UINT8_MAX);
		uintptr_t p =
			(uintptr_t) strncat(result.cmd_output, buf, UINT8_MAX);
		assert(p == (uintptr_t) result.cmd_output);
	}
	memset(buf, 0x0, UINT8_MAX);

	int nasm_exit_status = pclose(nasm_pipe);
	if (nasm_exit_status == -1) {
		goto invoke_assembler_nasm_error;
	} else if (nasm_exit_status > 0) {
		result.error_no = 0;
		result.status = INVOKE_NASM_FAIL;
		return result;
	}

	char ld_cmd[FILENAME_MAX];
	snprintf(ld_cmd, FILENAME_MAX, "ld -o %s out.o 2>&1", out_name);

	FILE *ld_pipe = popen(ld_cmd, "r");
	if (!ld_pipe) {
		goto invoke_assembler_ld_error;
	}

	while (fgets(buf, UINT8_MAX, ld_pipe) != NULL) {
		assert(strlen(buf) <= UINT8_MAX);
		uintptr_t p =
			(uintptr_t) strncat(result.cmd_output, buf, UINT8_MAX);
		assert(p == (uintptr_t) result.cmd_output);
	}
	memset(buf, 0x0, UINT8_MAX);

	int ld_exit_status = pclose(ld_pipe);
	if (ld_exit_status == -1) {
		goto invoke_assembler_ld_error;
	} else if (ld_exit_status > 0) {
		result.error_no = 0;
		result.status = INVOKE_LD_FAIL;
		return result;
	}

	result.error_no = 0;
	result.status = INVOKE_SUCCESS;
	return result;

invoke_assembler_nasm_error:
	result.error_no = errno;
	result.status = INVOKE_NASM_FAIL;
	return result;

invoke_assembler_ld_error:
	result.error_no = errno;
	result.status = INVOKE_LD_FAIL;
	return result;
}

int
main(int argc, char *argv[])
{
	const struct options opts = options_create(argc, argv);
	char *source_code = NULL;
	struct token *tokens = NULL;
	intmax_t file_size;
	size_t token_count;
	uint8_t err = 0;

	if (opts.mode == MODE_HELP) {
		puts(usage_msg);
		exit(EXIT_SUCCESS);
	}

	if (opts.result != OPTIONS_OK) {
		err = opts.result;
		goto main_opt_error;
	}

	printf_v(opts, "Beginning to load file %s to memory...\n", opts.in_file_name);
	file_size = load_file_to_buffer(&source_code, opts.in_file_name);

	if (file_size == -1) {
		goto main_file_read_err;
	}

	printf_v(opts,
		 "...file loaded into memory (enable debug for more information).\n",
		 opts.in_file_name);
	printd_file(source_code, opts.in_file_name, file_size, opts);

	tokens = calloc(file_size, sizeof(struct token));
	printf_v(opts, "Lexer beginning to parse source code...\n");
	token_count = tokenize(source_code, tokens, file_size);
	printf_v(opts, "..parsing complete (enable debug for more information).\n");
	printd_tokens(tokens, token_count, opts);
	free(source_code);

	struct ast *ast = construct_ast(tokens, token_count);
	char mem[30000] = {0};

	struct invoke_assembler_result invoke_result = {0};
	if (opts.mode == MODE_COMPILER) {
		struct compilation_result cresults = compile(ast, 30000);
		write_assembly_to_disk(cresults);
		compilation_result_destroy(cresults);
		invoke_result = invoke_assembler(opts.out_file_name);

		if (invoke_result.status != INVOKE_SUCCESS) {
			goto main_invoke_assembler_err;
		}

	} else {
		execute(ast, mem, 30000);
	}

	free(tokens);
	ast_destroy(ast);
	exit(EXIT_SUCCESS);

main_opt_error:
	switch (err) {
		case OPTIONS_NO_FILE:
			fprintf(stderr,
				"Please provide a brainf*ck source file.");
			break;
		case OPTIONS_FILE_TOO_LONG:
			fprintf(stderr,
				"File name longer than maximum value of %d.",
				PATH_MAX);
			break;
		case OPTIONS_MISSING_ARG:
			fprintf(stderr,
				"Missing argument for option -%c.\n",
				opts.wrong_opt);
			fprintf(stderr, "%s", usage_msg);
			break;
		case OPTIONS_UNKNOWN_ARG:
			fprintf(stderr,
				"Unknown option -%c.\n",
				opts.wrong_opt);
			fprintf(stderr, "%s", usage_msg);
			break;
		default:
			fprintf(stderr,
				"An unknown error has occured.");
			break;
	}
	exit(err);

main_file_read_err:
	fprintf(stderr,
		"Error reading file %s: %s",
		opts.in_file_name,
		strerror(errno));
	exit(errno);

main_invoke_assembler_err:
	if (invoke_result.error_no == 0) {
		if (invoke_result.status == INVOKE_NASM_FAIL) {
			fprintf(stderr, "NASM failed to assemble:\n%s",
				invoke_result.cmd_output);
		} else {
			fprintf(stderr, "LD failed to link binary:\n%s",
				invoke_result.cmd_output);
		}
		exit(EXIT_FAILURE);
	} else {
		if (invoke_result.status == INVOKE_NASM_FAIL) {
			fprintf(stderr, "Error invoking NASM:\n%s",
				strerror(invoke_result.error_no));
		} else {
			fprintf(stderr, "Error invoking LD:\n%s",
				strerror(invoke_result.error_no));
		}
		exit(invoke_result.error_no);
	}
}
