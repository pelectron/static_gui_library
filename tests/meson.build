test_sources = [
  'main.cpp',
  'callable.cpp',
  'cx_arg.cpp', 
  'fixed_point.cpp',
  'ryu/common_test.cpp',
  'ryu/d2fixed_test.cpp',
  'ryu/d2s_intrinsics_test.cpp',
  'ryu/d2s_table_test.cpp',
  'ryu/d2s_test.cpp',
  'ryu/f2s_test.cpp',
  'ryu/cx/f2s_test.cpp',
  'ryu/cx/d2s_test.cpp',
  'ryu/cx/d2fixed_test.cpp',
  'item_concept.cpp'
]

catch_dep = dependency('catch2')

executable('test_main', 
            include_directories: sgl_include_dir,
            sources: test_sources,
            dependencies: [catch_dep, dependencies]
)