project(
  'mattersplatter',
        'c',
       version: '0.1.0',
       default_options: [
         'warning_level=3',
         'c_std=c11',
         'werror=true'
       ]
)
cc = meson.get_compiler('c')
ms_include = include_directories('include')
math_dep = cc.find_library('m', required : true)

nasm = find_program('nasm', native: true, required: false)
if not nasm.found()
  warning('NASM not found on current machine. Compiler will not function correctly.')
endif

ld = find_program('ld', native: true, required: false)
if not ld.found()
  warning('\'ld\' not found on current machine. Compiler will not function correctly.')
endif

ms_lib = library('mattersplatter',
  [
    'lib/compiler.c',
    'lib/interpreter.c',
    'lib/jump_stack.c',
    'lib/lexer.c',
    'lib/parser.c',
    'lib/mattersplatter.c'
  ],
  soversion: '0.1.0',
  include_directories: ms_include,
  dependencies: math_dep,
  install: true
)

install_headers('include/mattersplatter.h')
ms = declare_dependency(link_with: ms_lib, include_directories: ms_include)

ms_exe = executable(
  'mattersplatter',
  [
    'main.c',
  ],
  dependencies: [ms],
  install: true
)

scdoc = find_program('scdoc', native: true, required: false)
if scdoc.found()
  sh = find_program('sh')
  mandir = get_option('mandir')
  custom_target(
    'mattersplatter.1',
    input: 'doc/mattersplatter.1.scd',
    output: 'mattersplatter.1',
    command: [
      sh, '-c', '@0@ < @INPUT@ > mattersplatter.1'.format(scdoc.path())
    ],
    install: true,
    install_dir: '@0@/man@1@'.format(mandir, '1')
  )
  custom_target(
    'matsplat_tokenize.3',
    input: 'doc/matsplat_tokenize.3.scd',
    output: 'matsplat_tokenize.3',
    command: [
      sh, '-c', '@0@ < @INPUT@ > matsplat_tokenize.3'.format(scdoc.path())
    ],
    install: true,
    install_dir: '@0@/man@1@'.format(mandir, '3')
  )
endif
