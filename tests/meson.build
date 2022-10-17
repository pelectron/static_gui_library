
test_sources = [
  'main.cpp',
  'callable.cpp',
  'cx_arg.cpp', 
  'fixed.cpp', 
  'fixed_point.cpp',
  'ryu/common_test.cpp',
  'ryu/d2fixed_test.cpp',
  'ryu/d2s_intrinsics_test.cpp',
  'ryu/d2s_table_test.cpp',
  'ryu/d2s_test.cpp',
  'ryu/f2s_test.cpp'
]

executable('test_main', 
            sources:test_sources,
            dependencies:[catch_dep,sgl_dep])
executable('named_tuple',
            sources:['named_tuple.cpp'],
            dependencies:[sgl_dep])