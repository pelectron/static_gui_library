test_sources = [
  'main.cpp', 
  'ryu/common_test.cpp',
  'ryu/d2fixed_test.cpp',
  'ryu/d2s_intrinsics_test.cpp',
  'ryu/d2s_table_test.cpp',
  'ryu/d2s_test.cpp',
  'ryu/f2s_test.cpp',
  'ryu/cx/f2s_test.cpp',
  'ryu/cx/d2s_test.cpp',
  'ryu/cx/d2fixed_test.cpp',

  'array.cpp',
  'callable.cpp',
  'cx_arg.cpp',
  'fixed_point.cpp',
  'input.cpp',
  'item_concept.cpp',
  'limits.cpp',
  'name.cpp',
  'named_tuple.cpp',
  'named_value.cpp',
  'pair.cpp',
  'static_string.cpp',
  'string_view.cpp',
  'type_list.cpp',
]

catch_dep = dependency('catch2')

executable('test_main', 
            include_directories: sgl_include_dir,
            sources: test_sources,
            dependencies: [catch_dep, dependencies]
)