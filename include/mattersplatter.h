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

/*
 * Each value represents a token in a Brainf*ck source. Technically, any
 * character in a BF source file is a token. Any character that does not map to
 * an operation (i.e. [^><+_.,\[\]EOF]) is treated as a COMMENT.
 */
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

/*
 * Contains information about a single source code token. Stores the type of
 * token, and it's position within the source file.
 */
struct matsplat_src_token {
	enum matsplat_token type;
	uintmax_t column;
	uintmax_t row;
};


/* Used to store human readable token information. */
struct matsplat_token_human_readable {
	const char symbol;
	const char *description;
};

/*
 * The result of the tokenization process. Contains an array of
 * `matsplat_src_token` objects, and the lenght of said array. This struct
 * should be destroyed by `matsplat_tokenize_destroy` to free up the heap space
 * used for the token array.
 */
struct matsplat_tokenize_result {
	size_t len;
	struct matsplat_src_token *tokens;
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

/*
 * Tokenizes Brainf*ck source code. Takes in the Brainf*ck source code as a
 * buffer, and the lenght of the source code. Returns a
 * `matsplat_tokenize_result` which contains the array of tokens. This struct
 * should be destroyed by `matsplat_tokenize_destroy` to free up heap space.
 */
struct matsplat_tokenize_result
matsplat_tokenize(const char *src_code, const size_t len);

/* Frees any memory used by the token array, and resets the length to 0. */
void
matsplat_tokenize_destory(struct matsplat_tokenize_result result);

void
matsplat_ast_destroy(struct matsplat_ast *ast);

struct matsplat_ast *
matsplat_construct_ast(struct matsplat_src_token *tokens, size_t token_len);

struct matsplat_token_human_readable
matsplat_token_to_human_readable(const enum matsplat_token type);

struct matsplat_compilation_result
matsplat_compile(struct matsplat_ast *ast, size_t mem);

void
matsplat_compilation_result_destroy(struct matsplat_compilation_result result);

uintmax_t
matsplat_execute(struct matsplat_ast *in, char mem[], uintmax_t memsize);

#endif // MATTERSPLATTER_H
