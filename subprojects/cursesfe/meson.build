project('cursesfe', 
'cpp',
version : '0.0.0',
default_options : ['warning_level=3', 'cpp_std=c++20'])

libur_proj = subproject('libur')
libur_dep = libur_proj.get_variable('libur_dep')

# project_headers = [
#   'include/BinaryLotmachine.hpp',
# ]

project_source_files = [
  'src/main.cpp'
]

project_test_files = [
#   'tests/src/tests.cpp',
]

build_args = [

]

libncurses_dep = meson.get_compiler('cpp').find_library('ncurses')
executable(
    'cURses',
    files(project_source_files),
    dependencies : [libur_dep, libncurses_dep],
    install : false
)