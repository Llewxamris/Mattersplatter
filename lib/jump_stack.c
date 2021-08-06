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
#include <stdlib.h>

#include "jump_stack.h"

struct jump_stack
jump_stack_create(void)
{
	struct jump_stack jstack;
	jstack.size = 0;
	jstack.stack = (struct matsplat_node **) calloc(1, sizeof(struct ast *));
	return jstack;
}

void
jump_stack_destroy(struct jump_stack jstack)
{
	for (size_t i = 0; i <= jstack.size; i++) {
		jstack.stack[i] = NULL;
	}

	free(jstack.stack);
}

void
push_jump_stack(struct matsplat_node *jump_node, struct jump_stack *jstack)
{
	if (jstack->size == 0) {
		jstack->stack[0] = jump_node;
		jstack->size = 1;
		return;
	}

	size_t new_len = jstack->size + 1;
	jstack->stack =
		(struct matsplat_node **)
		realloc(jstack->stack, new_len * sizeof(struct matsplat_node *));
	jstack->stack[jstack->size] = jump_node;
	jstack->size = new_len;
}

void
pop_jump_stack(struct matsplat_node **out, struct jump_stack *jstack)
{
	if (jstack->size == 0) {
		*out = NULL;
		return;
	}

	size_t new_len = jstack->size - 1;
	*out = jstack->stack[new_len];
	jstack->stack[new_len] = NULL;
	if (new_len <= 0) {
		jstack->stack =
			(struct matsplat_node **)
			realloc(jstack->stack, 1 * sizeof(struct ast *));
	} else {
		jstack->stack =
			(struct matsplat_node **)
			realloc(jstack->stack, new_len * sizeof(struct ast *));
	}
	jstack->size = new_len;
}
