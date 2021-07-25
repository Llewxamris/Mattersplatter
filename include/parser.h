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
#ifndef MATTERSPLATTER_PARSER_H
#define MATTERSPLATTER_PARSER_H
#include <stdbool.h>
#include <stddef.h>

#include "tokens.h"

struct ast {
	struct ast *parent;
	struct ast *left_child;
	struct ast *right_child;
	struct token *token;
};

void
ast_destroy(struct ast *ast);

struct ast *
construct_ast(struct token *tokens, size_t token_len);

#endif // MATTERSPLATTER_PARSER_H
