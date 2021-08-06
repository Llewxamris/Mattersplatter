matsplat_tokenize(3) "" "libmattersplatter Manual"

# NAME

matsplat_tokenize - Tokenize Brainf\*ck source code

# SYNOPSIS

\#include <mattersplatter.h>

struct matsplat_tokenize_result matsplat_tokenize(const char _\*src_code_,
	const size_t _len_);

void matsplat_tokenize_destroy(struct matsplat_tokenize_result _result_);

# DESCRIPTION

The *matsplat_tokenize()* function takes in some Brainf\*ck code _src_code_ and
its length _len_. It returns a *struct matsplat_tokenize_result*. This struct
has two fields:

. *tokens* :: An array of *enum matsplat_src_token*
. *len* :: The size of the *tokens* array

Since the *tokens* array is dynamically sized, it needs to be *free*'d.
*matsplat_tokenize_destroy()* takes in the result struct, deallocates the array,
and resets the length to 0.

# RETURN VALUE

*matsplat_tokenize()* returns the results struct. *matsplat_tokenize_destroy()*
returns _void_.

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
