test_sources = [
  'main.cpp',

  'ryu/common_test.cpp',
  'ryu/d2fixed_test.cpp',
  'ryu/d2s_intrinsics_test.cpp',
  'ryu/d2s_table_test.cpp',
  'ryu/d2s_test.cpp',
  'ryu/f2s_test.cpp',
  'ryu/s2f_test.cpp',
  'ryu/s2d_test.cpp',

  'ryu/cx/f2s_test.cpp',
  'ryu/cx/d2s_test.cpp',
  'ryu/cx/d2fixed_test.cpp',
 'ryu/cx/s2f_test.cpp',
  'ryu/cx/s2d_test.cpp',

  'array.cpp',
  'callable.cpp',
  'cx_arg.cpp',
  'enum_map.cpp',
  'fixed_point.cpp',
  'input.cpp',
  'item_concept.cpp',
  'limits.cpp',
  'menu.cpp',
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
            sources: test_sources,
            dependencies: [catch_dep, sgl_dep]
)
