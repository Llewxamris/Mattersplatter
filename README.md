# Mattersplatter
`mattersplatter` is a compiler & interpreter for the Brainf\*ck esoteric
programming language. See the included man page for more information.

# Dependencies
- C11 compiler (_compile time_)
- POSIX environment (_compile time_, untested on platforms other than GNU/Linux)
- `meson` (_compile time_)
- `scdoc` to generate the man page (_compile time_, _optional_)
- `nasm` to assemble the compiled assembly output (_run time_, _optional_)
- `ld` to link the assembled ELF binary (_run time_, _optional_)

`nasm` and `ld` are only required if running `mattersplatter` in compilation
mode. 

# Building & Installing
To build `mattersplatter`, execute the following in the root project directory:

`meson compile -C build`

To install:

`meson install -C build`

If `scdoc` is present, then `meson` will also install the man page.

# License
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.

