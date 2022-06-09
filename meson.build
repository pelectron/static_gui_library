project('static gui',['cpp','c'],default_options:['buildtype=release','cpp_std=c++17','cpp_eh=none','cpp_rtti=false','cpp_debugstl=false','c_std=c11','b_sanitize=none'])

ryu_dep = dependency('ryu', fallback:['ryu','ryu_dep'])
add_project_arguments('-ferror-limit=1',language:'cpp')
# executable('menu_ex','menu_example.cpp')
# executable('menu_test','menu_tester.cpp')
# executable('sgl_test','sgl_test.cpp')
executable('items','include/items.cpp',dependencies:ryu_dep)
executable('tuple','include/tuple.cpp')

static_gui_dep = declare_dependency(include_directories:include_directories('.'))
# custom_target('c_test', depend_files:['callable_test.cpp'], command:['clang++', '-O3','-std=c++17','-fno-exceptions','-o', meson.current_build_dir() + '/callable_test.S', '-S', meson.current_source_dir() + '/callable_test.cpp'], output:['callable_test.S'])
# executable('callable_test','callable_test.cpp',cpp_args:['-O3'])
# executable('item_base','item_base.cpp',dependencies:ryu_dep)

# custom_target('item_test',depend_files:['item_base.cpp'], command:['clang++', '-O3','-std=c++17','-fno-exceptions','-fno-rtti','-o', meson.current_build_dir() + '/item_base.S', '-S', meson.current_source_dir() + '/item_base.cpp', '-I',meson.current_source_dir()+ '/subprojects/ryu'], output:['item_base.S'])

# custom_target('m_test',depend_files:['menu_tester.cpp'], command:['clang++', '-O3','-std=c++17','-fno-exceptions','-fno-rtti','-o', meson.current_build_dir() + '/menu_tester.S', '-S', meson.current_source_dir() + '/menu_tester.cpp'], output:['menu_tester.S'])
# custom_target('m_ex',depend_files:['menu_example.cpp'], command:['clang++','-Os','-std=c++17','-fno-exceptions','-fno-rtti','-o', meson.current_build_dir() + '/menu_example.S', '-S', meson.current_source_dir() + '/menu_example.cpp'], output:['menu_example.S'])
