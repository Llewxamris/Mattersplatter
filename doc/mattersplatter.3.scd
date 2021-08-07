mattersplatter(3) "" "libmattersplatter Manual"

# NAME

libmattersplatter - Mattersplatter C API

# SYNOPSIS

\#include <mattersplatter.h>

truct matsplat_tokenize_result matsplat_tokenize(const char _\*src_code_,
	const size_t _len_);

void matsplat_tokenize_destroy(struct matsplat_tokenize_result _result_);

struct matsplat_node \*matsplat_ast_create(struct matsplat_src_token _\*tokens_,
	size_t _len_);

void matsplat_ast_destroy(struct matsplat_node _\*root_);

struct matsplat_execution_result matsplat_execute(struct matsplat_node \*start,
	size_t cell_count);

void matsplat_execution_result_destory(struct matsplat_execution_result result);

struct matsplat_compilation_result matsplat_compile(struct matsplat_node *ast,
	size_t mem);

void matsplat_compilation_result_destroy(
	struct matsplat_compilation_result result);

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

The *matsplat_execute()* function executes the application in-place. That is, it
modifies values, moves the pointer, reads, and writes the moment the operation
is found. It takes in _start_ which is treated as the root node of the
application, and _cell\_count_ which is the amount of 8-bit memory cells made
available to the application. It returns a *struct matsplat_execution_result*.
This struct has three field:

. size\_t *pointer* :: The final position of the pointer.
. size\_t *cell_count* :: The amount of cells & the length of _memory_cells_.
. int8\_t \**memory_cells* :: The array after the program has executed.

Since *memory_cells* is dynamically allocated, the resulting structure should
be destroyed with *matsplat_execution_result_destroy()*.

The function *matsplat_execution_result_destroy()* deallocates *struct
matsplat_execution_result*. Specifically, the _memory\_cells_ field. This
function should be called even if the caller does not wish to store the results
struct. In that case, the caller should do something like the following:

```
		matsplat_execution_result_destory(
				matsplat_execute(node, cell_count));
```

Failure to do so will result in a memory leak.

The function *matsplat_compile()* generates x86\_64 assembly based on the
provided _ast_. The amount of memory declared in the assembly is based off of
the _cell\_count_ value. It returns a *struct matsplat_compilation_result*. This
struct has three fields:

. char *\*source_code* :: The assembly source code
. size\_t *source_code_len* :: The length of the _source\_code_ field
. int *error_code* :: The error identifier

If _error\_code_ is non-zero, then the compilation process failed. The only
reason for this is if a call to *calloc*(3) fails. This means the value of
_error\_code_ will match a possible error value from *calloc*.

The function *matsplat_compilation_result_destroy()* takes in a *struct
matsplat_compilation_result*, deallocates the _source\_code_ field and sets the
other two fields to 0.

# RETURN VALUE

*matsplat_tokenize()* returns the results struct.

*matsplat_tokenize_destroy()* returns _void_.

*matsplat_ast_create()* returns a pointer to the root node of the AST.

*matsplat_ast_destroy()* returns _void_.

*matsplat_execute()* returns the results struct.

*matsplat_execution_result_destroy()* returns _void_.

*matsplat_compile()* returns the results struct.

*matsplat_compilation_result_destroy()* returns _void_.

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
