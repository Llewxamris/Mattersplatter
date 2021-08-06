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

static void
execute(struct matsplat_node *node, size_t *pointer, int8_t *memory_cells,
	size_t cell_count)
{

	if (node->token->type == END || node->token->type == JUMP_BACKWARDS) {
		return;
	} else if (node->token->type == JUMP_FORWARD) {
		while (memory_cells[*pointer] != 0) {
			execute(node->left_child, pointer, memory_cells,
				cell_count);
		}

		execute(node->right_child, pointer, memory_cells,
			cell_count);
		return;
	} 
	switch (node->token->type) {
		case POINTER_RIGHT:
			if (*pointer == cell_count - 1) {
				*pointer = 0;
			} else {
				*pointer += 1;
			}
			break;
		case POINTER_LEFT:
			if (*pointer == 0) {
				*pointer = cell_count - 1;
			} else {
				*pointer -= 1;
			}
			break;
		case INCREMENT:
			memory_cells[*pointer]++;
			break;
		case DECREMENT:
			memory_cells[*pointer]--;
			break;
		case OUTPUT:
			printf("%c", memory_cells[*pointer]);
			break;
		case INPUT:
			scanf("%c", &memory_cells[*pointer]);
			break;
		case COMMENT:
			/*
			 * No comment tokens should be present. If they are,
			 * treat as NOOP.
			 */
			 /* Fallthrough */
		default:
			break;
	}

	execute(node->right_child, pointer, memory_cells, cell_count);
	return;
}

struct matsplat_execution_result
matsplat_execute(struct matsplat_node *start, size_t cell_count)
{
	int8_t *memory_cells = calloc(cell_count, sizeof(int8_t));
	uintmax_t pointer = 0;

	execute(start, &pointer, memory_cells, cell_count);

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
