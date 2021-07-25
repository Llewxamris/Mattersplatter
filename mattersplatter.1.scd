mattersplatter(1) "" "Mattersplatter"

# NAME

mattersplatter - brainf\*ck compiler & interpreter

# SYNOPSIS

*mattersplatter* [[-o _outfile_] | -b] [-v] [-d] _filename_

# DESCRIPTION

*mattersplatter* is a compiler & interpreter for the Brainf\*ck esoteric
programming language. It will either compile _filename_ to an executable
binary, or act as an interpreter and execute the program written in _filename_.
The name of the output file is chosen following these steps:

. Use _outfile_ if the option *-o* is set
. Use the original filename with the file extention removed
. Use the name _a.out_

*mattersplatter* defaults to compiler mode. To run in batch (interpreter) mode,
provide the *-b* option.

*mattersplatter* currently only compiles to x86_64 Linux ELF binaries.
*mattersplatter* also requires *nasm*(1) and *ld*(1) to be on the host machine
during compile time.

# OPTIONS

*-b*
	Run *mattersplatter* in batch mode. In this mode, *mattersplatter* acts as
	an interpreter. It will parse _filename_, and execute the intructions.

*-d*
	Sends debug output to _stdout_.

*-h*
	Displays the usage information. The usage information is also shown if an
	unknown option is declared, or if an option is missing an argument.

*-o* _outfile_
	Specify a name for the output binary instead of *mattersplatter* choosing a
	name. This option is ignored if the *-b* option is present.

*-v*
	Sends verbos output to _stdout_.
