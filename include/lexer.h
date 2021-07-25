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
#ifndef MATTERSPLATTER_LEXER_H
#define MATTERSPLATTER_LEXER_H
#include <stddef.h>

#include "tokens.h"

size_t
tokenize(const char *source_code, struct token *tokens, const size_t size);

struct token_human_readable
token_type_to_human_readable(const enum token_type type);

#endif // MATTERSPLATTER_LEXER_H
