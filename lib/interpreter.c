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
#include <stdbool.h>
#include <stdio.h>

#include "mattersplatter.h"
#include "jump_stack.h"

uintmax_t
matsplat_execute(struct matsplat_ast *in, char mem[], uintmax_t memsize)
{
	struct matsplat_ast *current = in;
	struct jump_stack jump_stack = jump_stack_create();
	bool is_not_complete = true;
	bool is_flow_left = false;
	uintmax_t pointer = 0;

	while(is_not_complete) {
		struct matsplat_src_token t = *current->token;
		switch (t.type) {
			case POINTER_RIGHT:
				if (pointer == memsize - 1) {
					pointer = 0;
				} else {
					pointer++;
				}
				break;
			case POINTER_LEFT:
				if (pointer == 0) {
					pointer = memsize - 1;
				} else {
					pointer--;
				}
				break;
			case INCREMENT:
				mem[pointer]++;
				break;
			case DECREMENT:
				mem[pointer]--;
				break;
			case OUTPUT:
				printf("%c", mem[pointer]);
				break;
			case INPUT:
				scanf("%c", &mem[pointer]);
				break;
			case JUMP_FORWARD:
				if (mem[pointer] == 0) {
					/* If the cell at pointer is 0, skip the jump. */
					is_flow_left = false;
				} else {
					push_jump_stack(current, &jump_stack);
					is_flow_left = true;
				}
				break;
			case JUMP_BACKWARDS:
				if (mem[pointer] != 0) {
					// Go back to loop start if cell is non-zero
					pop_jump_stack(&current, &jump_stack);
					is_flow_left = true;
					push_jump_stack(current, &jump_stack);
				} else {
					pop_jump_stack(&current, &jump_stack);
					is_flow_left = false;
				}
				break;
			case COMMENT:
				// No comment tokens should be present. If they
				// are, treat as NOOP.
				break;
			case END:
				is_not_complete = false;
				break;
		}

		if (is_flow_left) {
			current = current->left_child;
		} else {
			current = current->right_child;
		}
	}

	jump_stack_destroy(jump_stack);
	return pointer;
}
