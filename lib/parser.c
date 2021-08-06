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
#include <stdint.h>
#include <stdlib.h>

#include "mattersplatter.h"

void
matsplat_ast_destroy(struct matsplat_node *ast) {
	if (ast->left_child) {
		matsplat_ast_destroy(ast->left_child);
	}

	if (ast->right_child) {
		matsplat_ast_destroy(ast->right_child);
	}

	free(ast);
}

static struct matsplat_node
*ast_create(struct matsplat_src_token *tokens, size_t len, size_t *pos)
{
	if (*pos > len) {
		return NULL;
	}
	struct matsplat_src_token *t = &tokens[*pos];
	struct matsplat_node *node =
		(struct matsplat_node*) calloc(1, sizeof(struct matsplat_node));
	node->token = t;
	node->left_child = NULL;
	node->right_child = NULL;

	*pos += 1;
	if (t->type == JUMP_FORWARD) {
		node->left_child = ast_create(tokens, len, pos);
		node->right_child = ast_create(tokens, len, pos);
		return node;
	} else if (t->type == JUMP_BACKWARDS) {
		return node;
	} else {
		node->right_child = ast_create(tokens, len, pos);
		return node;
	}
}

struct matsplat_node
*matsplat_ast_create(struct matsplat_src_token *tokens, size_t len)
{
	size_t pos = 0;
	return ast_create(tokens, len, &pos);
}
