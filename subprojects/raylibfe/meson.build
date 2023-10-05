project('raylibfe', 'c', 'cpp',
default_options : ['warning_level=3', 'cpp_std=c++20'])
cmake = import('cmake')
raylib_proj = cmake.subproject('raylib')
raylib_dep = raylib_proj.get_variable('raylib_dep')

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

c_args = []
cpp_args = ['-std=c++20']
link_args = []
name_suffix = []
dependencies = []
sources = files('src/main.cpp')
inc_dir = include_directories(
  'src',
)

raylib_opts = cmake.subproject_options()
raylib_opts.set_install(false)
raylib_opts.add_cmake_defines({
  'BUILD_EXAMPLES': 'OFF',
  # Add other CMake options for Raylib, if needed
})
if cc.get_id() == 'emscripten'
  raylib_opts.add_cmake_defines({
    'PLATFORM': 'Web',
  })
else
  raylib_opts.add_cmake_defines({
    'PLATFORM': 'Desktop',
  })
endif
raylib_subproject = cmake.subproject('raylib', options: raylib_opts)
dependencies += raylib_subproject.dependency('raylib')
dependencies += cc.find_library('glfw')

# General configuration
if host_machine.system() == 'windows'
  dependencies += [
    cc.find_library('winmm'),
  ]
elif host_machine.system() == 'darwin'
  # TODO
elif host_machine.system() == 'linux'
  dependencies += [
    cc.find_library('m'),
    cc.find_library('dl'),
  ]
elif host_machine.system() == 'emscripten'
  link_args += [
    '-s', 'ENVIRONMENT=web',
    '-s', 'USE_GLFW=3',
  ]
  name_suffix = 'html'
endif

# libur
libur_proj = subproject('libur')
libur_dep = libur_proj.get_variable('libur_dep')
dependencies += libur_dep


# for handlign assets, referenced: https://stackoverflow.com/questions/46473646/meson-working-with-data-assets-and-portable-relative-paths
pkgdatadir = 'assets/'
conf = configuration_data()
conf.set_quoted('ASSET_PATH', join_paths(meson.source_root(), meson.current_source_dir(), pkgdatadir))
conf.set_quoted('WINDOW_TITLE', 'Game of Ur')
configure_file(
  output: 'config.hpp',
  configuration: conf
)

executable('raylib-meson',
  sources,
  c_args: c_args,
  cpp_args: cpp_args,
  dependencies: dependencies,
  include_directories: inc_dir,
  link_args: link_args,
  name_suffix: name_suffix,
)