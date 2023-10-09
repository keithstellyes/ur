project('raylibfe', 'c', 'cpp',
default_options : ['warning_level=3', 'cpp_std=c++20'],
version: '1.0.0')
cmake = import('cmake')

# option('WINDOW_TITLE', type : 'string', value : 'Game of Ur', description : 'Set application window title')

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

c_args = []
cpp_args = ['-std=c++20']
link_args = []
name_suffix = []
dependencies = []
sources = files('src/main.cpp', 
  'src/raylib-extra.cpp', 'src/GameStateDrawer.cpp', 'src/DrawThread.cpp', 
  'src/AiThread.cpp',
  'src/MainMenu.cpp',
  'src/InfoMenu.cpp',
  'src/StartGameMenu.cpp')
inc_dir = include_directories(
  'include',
)

raylib_opts = cmake.subproject_options()
raylib_opts.set_install(false)
raylib_opts.add_cmake_defines({
  'BUILD_EXAMPLES': 'OFF',
  # Add other CMake options for Raylib, if needed,
})
if cc.get_id() == 'emscripten'
  message('first if')
  raylib_opts.add_cmake_defines({
    'PLATFORM': 'Web',
  })
else
  message('second if')
  raylib_opts.add_cmake_defines({
    'PLATFORM': 'Desktop',
  })
endif
raylib_opts.append_compile_args('c', '-pthread')
raylib_opts.append_compile_args('cpp', '-pthread')
raylib_opts.append_link_args('c', '-pthread')
raylib_opts.append_link_args('cpp', '-pthread')
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
    '-s', 'SHARED_MEMORY',
    '-s', 'ASYNCIFY',
    '-pthread'

  ]
  cpp_args += ['-isystem', '/usr/lib/emscripten/system/lib/libcxx/include/', '-isystem', '/usr/lib/emscripten/system/lib/libc', '-pthread', '-DPLATFORM_WEB']
  name_suffix = 'html'
endif

# libur
libur_proj = subproject('libur')
libur_dep = libur_proj.get_variable('libur_dep')
dependencies += libur_dep

#urai
urai_proj = subproject('urai')
urai_dep = urai_proj.get_variable('urai_dep')
dependencies += urai_dep

# for handlign assets, referenced: https://stackoverflow.com/questions/46473646/meson-working-with-data-assets-and-portable-relative-paths
pkgdatadir = 'assets/'
conf = configuration_data()
asset_path = get_option('ASSET_PATH')
if asset_path == ''
  asset_path = meson.current_source_dir() + '/assets/'
endif

conf.set_quoted('WINDOW_TITLE', get_option('WINDOW_TITLE'))
conf.set_quoted('ASSET_PATH', asset_path)

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