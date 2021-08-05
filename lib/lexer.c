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

#include "mattersplatter.h"

static enum matsplat_token
check_token_type(char c)
{
	switch(c) {
		case '>':
			return POINTER_RIGHT;
		case '<':
			return POINTER_LEFT;
		case '+':
			return INCREMENT;
		case '-':
			return DECREMENT;
		case '.':
			return OUTPUT;
		case ',':
			return INPUT;
		case '[':
			return JUMP_FORWARD;
		case ']':
			return JUMP_BACKWARDS;
		case '\0':
			return END;
		default:
			return COMMENT;
	}
}

struct matsplat_token_human_readable
matsplat_token_to_human_readable(const enum matsplat_token type)
{
	switch(type) {
		case POINTER_RIGHT:
			return (struct matsplat_token_human_readable) {
			'<', "Pointer right"

		};
		case POINTER_LEFT:
			return (struct matsplat_token_human_readable) {
			'>', "Pointer left"
		};
		case INCREMENT:
			return (struct matsplat_token_human_readable) {
			'+', "Increment"
		};
		case DECREMENT:
			return (struct matsplat_token_human_readable) {
			'-', "Decrement"
		};
		case OUTPUT:
			return (struct matsplat_token_human_readable) {
			'.', "Output"
		};
		case INPUT:
			return (struct matsplat_token_human_readable) {
			',', "Input"
		};
		case JUMP_FORWARD:
			return (struct matsplat_token_human_readable) {
			'[', "Jump forward"
		};
		case JUMP_BACKWARDS:
			return (struct matsplat_token_human_readable) {
			']', "Jump backwards"
		};
		case END:
			return (struct matsplat_token_human_readable) {
			'\0', "End of file"
		};
		default:
			return (struct matsplat_token_human_readable) {
			' ', "Comment"
		};
	}
}

size_t
matsplat_tokenize(const char *source_code, struct matsplat_src_token *tokens, const size_t size)
{
	uintmax_t col = 0;
	uintmax_t row = 1;
	size_t tokens_idx = 0;

	for (size_t i = 0; i <= size; i++) {
		char c = source_code[i];
		enum matsplat_token t = check_token_type(c);

		if (t == COMMENT) {
			if (c == '\n' || (c == '\r' && source_code[i + 1] == '\n')) {
				col++;
				row = 1;
			}
		} else {
			struct matsplat_src_token new_token = {.type = t, .column = col, .row = row};
			tokens[tokens_idx] = new_token;
			tokens_idx++;
			row++;
		}
	}
	return tokens_idx - 1;
}