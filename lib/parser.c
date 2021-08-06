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

#include "jump_stack.h"
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

struct matsplat_node
*matsplat_ast_create(struct matsplat_src_token *tokens, size_t len)
{
	struct matsplat_node *root = NULL;
	struct matsplat_node *current = NULL;
	struct matsplat_node *parent = NULL;
	struct jump_stack jump_stack = jump_stack_create();
	bool is_flow_left = false;

	for(size_t i = 0; i < len; i++) {
		/* Explicitly handle setting the root node. */
		if (root == NULL) {
			root = (struct matsplat_node*)
				malloc(sizeof(struct matsplat_node));
			root->parent = NULL;
			root->left_child = NULL;
			root->right_child = NULL;
			root->token = &tokens[i];
			parent = root;
			continue;
		}
		struct matsplat_src_token *t = &tokens[i];
		current = (struct matsplat_node*)
			malloc(sizeof(struct matsplat_node));
		current->parent = parent;
		current->left_child = NULL;
		current->right_child = NULL;
		current->token = t;

		if (is_flow_left) {
			parent->left_child = current;
		} else {
			parent->right_child = current;
		}

		if (current->token->type == JUMP_FORWARD) {
			push_jump_stack(current, &jump_stack);
			is_flow_left = true;
		} else if (current->token->type == JUMP_BACKWARDS) {
			pop_jump_stack(&current, &jump_stack);
			is_flow_left = false;
		}
 		parent = current;
	}

	free(jump_stack.stack);
	return root;
}
