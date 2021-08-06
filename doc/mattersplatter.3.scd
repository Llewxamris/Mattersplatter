mattersplatter(3) "" "libmattersplatter Manual"

# NAME

libmattersplatter - Mattersplatter C API

# SYNOPSIS

\#include <mattersplatter.h>

struct matsplat_tokenize_result matsplat_tokenize(const char _\*src_code_,
	const size_t _len_);

void matsplat_tokenize_destroy(struct matsplat_tokenize_result _result_);

struct matsplat_node \*matsplat_ast_create(struct matsplat_src_token _\*tokens_,
	size_t _len_);

void matsplat_ast_destroy(struct matsplat_node _\*root_);

# DESCRIPTION

The *matsplat_tokenize()* function takes in some Brainf\*ck code _src_code_ and
its length _len_. It returns a *struct matsplat_tokenize_result*. This struct
has two fields:

. *tokens* :: An array of *enum matsplat_src_token*
. *len* :: The size of the *tokens* array

Since the *tokens* array is dynamically sized, it needs to be *free*'d.

The *matsplat_tokenize_destroy()* function takes in the result struct,
deallocates the array, and resets the length to 0.

The *matsplat_ast_create()* function parses _tokens_ up to size _len_,
generating an abstract syntax tree.

The *matsplat_ast_destroy()* function deallocates all nodes.

# RETURN VALUE

*matsplat_tokenize()* returns the results struct.

*matsplat_tokenize_destroy()* returns _void_.

*matsplat_ast_create()* returns a pointer to the root node of the AST.

*matsplat_ast_destroy()* returns _void_.

# COPYRIGHT

Mattersplatter - a compiler & interpreter for the Brainf\*ck language.

Copyright (C) 2021 Maxwell R. Haley <maxwell.r.haley@gmail.com>

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
