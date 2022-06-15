project('static gui',['cpp','c'],default_options:['buildtype=release','cpp_std=c++17','cpp_eh=none','cpp_rtti=false','c_std=c11','warning_level=3'])

use_ryu = get_option('use_ryu')
optimize_size = get_option('optimize_size')
use_charconv = get_option('use_charconv')
char_type = get_option('char_type')
line_width = get_option('line_width')
instantiate = get_option('instantiate')
dependencies = []
defines = []
if use_ryu and not use_charconv
    dependencies = [dependency('ryu', fallback:['ryu','ryu_dep'])]
    defines += '-DSGL_USE_RYU=1'
    if optimize_size
    defines += '-DRYU_OPTIMIZE_SIZE'
    endif
elif use_charconv
    defines += '-DSGL_USE_CHARCONV=1'
else
warning('Neither use_ryu or use_charconv set in meson options. Formatting will only work for integers.')
endif
if instantiate
    if line_width == 0
        error('library line width set to 0')
    endif
    defines += ['-DSGL_CHAR_TYPE=@0@'.format(char_type), '-DSGL_LINE_WIDTH=@0@'.format(line_width)]
endif

add_project_arguments('-ferror-limit=1',language:'cpp')
# executable('menu_ex','menu_example.cpp')
# executable('menu_test','menu_tester.cpp')
# executable('sgl_test','sgl_test.cpp')
executable('items', 'include/items.cpp', dependencies: dependencies, cpp_args:defines)
executable('tuple', 'include/tuple.cpp')

static_gui_dep = declare_dependency(include_directories: include_directories('include'), dependencies: dependencies, compile_args: defines)
# custom_target('c_test', depend_files:['callable_test.cpp'], command:['clang++', '-O3','-std=c++17','-fno-exceptions','-o', meson.current_build_dir() + '/callable_test.S', '-S', meson.current_source_dir() + '/callable_test.cpp'], output:['callable_test.S'])
# executable('callable_test','callable_test.cpp',cpp_args:['-O3'])
# executable('item_base','item_base.cpp',dependencies:ryu_dep)

# custom_target('item_test',depend_files:['item_base.cpp'], command:['clang++', '-O3','-std=c++17','-fno-exceptions','-fno-rtti','-o', meson.current_build_dir() + '/item_base.S', '-S', meson.current_source_dir() + '/item_base.cpp', '-I',meson.current_source_dir()+ '/subprojects/ryu'], output:['item_base.S'])

# custom_target('m_test',depend_files:['menu_tester.cpp'], command:['clang++', '-O3','-std=c++17','-fno-exceptions','-fno-rtti','-o', meson.current_build_dir() + '/menu_tester.S', '-S', meson.current_source_dir() + '/menu_tester.cpp'], output:['menu_tester.S'])
# custom_target('m_ex',depend_files:['menu_example.cpp'], command:['clang++','-Os','-std=c++17','-fno-exceptions','-fno-rtti','-o', meson.current_build_dir() + '/menu_example.S', '-S', meson.current_source_dir() + '/menu_example.cpp'], output:['menu_example.S'])
if get_option('test').enabled()
  catch_dep = dependency('catch2')
  subdir('tests')
endif