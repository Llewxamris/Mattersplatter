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
#include <stdlib.h>

#include "mattersplatter.h"
#include "jump_stack.h"

struct matsplat_execution_result
matsplat_execute(struct matsplat_node *start, size_t cell_count)
{
	int8_t *memory_cells = calloc(cell_count, sizeof(int8_t));
	struct matsplat_node *current = start;
	struct jump_stack jump_stack = jump_stack_create();
	bool is_not_complete = true;
	bool is_flow_left = false;
	uintmax_t pointer = 0;

	while(is_not_complete) {
		struct matsplat_src_token t = *current->token;
		switch (t.type) {
			case POINTER_RIGHT:
				if (pointer == cell_count - 1) {
					pointer = 0;
				} else {
					pointer++;
				}
				break;
			case POINTER_LEFT:
				if (pointer == 0) {
					pointer = cell_count - 1;
				} else {
					pointer--;
				}
				break;
			case INCREMENT:
				memory_cells[pointer]++;
				break;
			case DECREMENT:
				memory_cells[pointer]--;
				break;
			case OUTPUT:
				printf("%c", memory_cells[pointer]);
				break;
			case INPUT:
				scanf("%c", &memory_cells[pointer]);
				break;
			case JUMP_FORWARD:
				if (memory_cells[pointer] == 0) {
					/* If the cell at pointer is 0, skip the jump. */
					is_flow_left = false;
				} else {
					push_jump_stack(current, &jump_stack);
					is_flow_left = true;
				}
				break;
			case JUMP_BACKWARDS:
				if (memory_cells[pointer] != 0) {
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
			is_flow_left = false;
		} else {
			current = current->right_child;
		}
	}

	jump_stack_destroy(jump_stack);

	return (struct matsplat_execution_result)
		{ .pointer = pointer, .cell_count = cell_count,
		  .memory_cells = memory_cells };
}


void
matsplat_execution_result_destory(struct matsplat_execution_result result)
{
	free(result.memory_cells);
	result.cell_count = 0;
	result.pointer = 0;
}
