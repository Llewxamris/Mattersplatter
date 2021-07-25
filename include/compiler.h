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
#ifndef MATTERSPLATTER_COMPILER_H
#define MATTERSPLATTER_COMPILER_H
#include "parser.h"

struct compilation_result {
	char *source_code;
	size_t source_code_len;
	int error_code;
};

struct compilation_result
compile(struct ast *ast, size_t mem);

void
compilation_result_destroy(struct compilation_result result);

#endif // MATTERSPLATTER_COMPILER_H
