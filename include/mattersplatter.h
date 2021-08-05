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
#ifndef MATTERSPLATTER_H
#define MATTERSPLATTER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum matsplat_token {
POINTER_RIGHT,
POINTER_LEFT,
INCREMENT,
DECREMENT,
OUTPUT,
INPUT,
JUMP_FORWARD,
JUMP_BACKWARDS,
COMMENT,
END
};

struct matsplat_src_token {
	enum matsplat_token type;
	uintmax_t column;
	uintmax_t row;
};

struct matsplat_token_human_readable {
	const char symbol;
	const char *description;
};

struct matsplat_ast {
	struct matsplat_ast *parent;
	struct matsplat_ast *left_child;
	struct matsplat_ast *right_child;
	struct matsplat_src_token *token;
};

struct matsplat_compilation_result {
	char *source_code;
	size_t source_code_len;
	int error_code;
};

void
matsplat_ast_destroy(struct matsplat_ast *ast);

struct matsplat_ast *
matsplat_construct_ast(struct matsplat_src_token *tokens, size_t token_len);

size_t
matsplat_tokenize(const char *source_code, struct matsplat_src_token *tokens, const size_t size);

struct matsplat_token_human_readable
matsplat_token_to_human_readable(const enum matsplat_token type);

struct matsplat_compilation_result
matsplat_compile(struct matsplat_ast *ast, size_t mem);

void
matsplat_compilation_result_destroy(struct matsplat_compilation_result result);

uintmax_t
matsplat_execute(struct matsplat_ast *in, char mem[], uintmax_t memsize);

#endif // MATTERSPLATTER_H
