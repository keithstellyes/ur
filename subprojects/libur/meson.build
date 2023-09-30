# Referenced from https://github.com/kstenerud/meson-examples/tree/master/library
project(
  'libur',
  'cpp',
  version : '0.0.0',
  default_options : ['warning_level=3', 'cpp_std=c++20']
)

CXX='g++'

project_headers = [
  'include/BinaryLotmachine.hpp',
  'include/coord.hpp',
  'include/GameSettings.hpp',
  'include/GameState.hpp'
]

project_source_files = [
  'src/BinaryLotMachine.cpp',
  'src/coord.cpp',
  'src/GameSettings.cpp',
  'src/GameState.cpp'
]

project_test_files = [
  'tests/src/tests.cpp',
]

build_args = [

]

# ===================================================================

# ======
# Target
# ======

public_headers = include_directories('include')

# Only make public interfaces visible
if target_machine.system() == 'windows' or target_machine.system() == 'cygwin'
  build_args += '-DMYLIB_PUBLIC="__declspec(dllexport)"'
else
  build_args += '-DMYLIB_PUBLIC=__attribute__((visibility("default")))'
endif

project_target = static_library(
  meson.project_name(),
  project_source_files,
  install : true,
  cpp_args : build_args,
  gnu_symbol_visibility : 'hidden',
  include_directories : public_headers,
)

# =======
# Project
# =======

# Make this library usable as a Meson subproject.
project_dep = declare_dependency(
  include_directories: public_headers,
  link_with : project_target
)
set_variable(meson.project_name() + '_dep', project_dep)

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
gtest_main_dep = gtest_proj.get_variable('gtest_main_dep')

# if not meson.is_subproject()
  add_languages('cpp')
  subdir('tests')

  test('all_tests',
    executable(
      'run_tests',
      files(project_test_files),
      dependencies : [project_dep, gtest_dep, gtest_main_dep],
      install : false
    )
  )
# endif

# libur_dep = dependency(
#   'libur',
#   fallback : ['libur', 'libur_dep'],
#   default_options: ['default_library=static']
# )