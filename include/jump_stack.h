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
#ifndef MATTERSPLATTER_JUMP_STACK_H
#define MATTERSPLATTER_JUMP_STACK_H
#include "mattersplatter.h"

struct jump_stack {
	size_t size;
	struct matsplat_ast **stack;
};

struct jump_stack
jump_stack_create(void);

void
jump_stack_destroy(struct jump_stack);

void
push_jump_stack(struct matsplat_ast *, struct jump_stack *);

void
pop_jump_stack(struct matsplat_ast **, struct jump_stack *);


#endif // MATTERSPLATTER_JUMP_STACK_H
