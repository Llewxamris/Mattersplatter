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
#ifndef MATTERSPLATTER_TOKENS_H
#define MATTERSPLATTER_TOKENS_H
#include <stdint.h>

enum token_type {
POINTER_RIGHT = 1 << 0,
POINTER_LEFT = 1 << 1,
INCREMENT = 1 << 2,
DECREMENT = 1 << 3,
OUTPUT = 1 << 4,
INPUT = 1 << 5,
JUMP_FORWARD = 1 << 6,
JUMP_BACKWARDS = 1 << 7,
COMMENT = 1 << 8,
END = 1 << 9,
};

struct token {
	enum token_type type;
	uintmax_t column;
	uintmax_t row;
};

struct token_human_readable {
	const char symbol;
	const char *description;
};

#endif // MATTERSPLATTER_TOKENS_H
