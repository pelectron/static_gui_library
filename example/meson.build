executable('example_readme','example_readme.cpp', dependencies: core_dep)
executable('menu2','menu2.cpp', dependencies: core_dep)
executable('menu_tester','menu_tester.cpp', dependencies: core_dep)

if get_option('gui')
  executable('gui_example', 
              'qt_main.cpp', 
              dependencies: [sgl_dep],
              override_options: ['cpp_eh=default'])
endif
