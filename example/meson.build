args = []

readme_exe = executable('example_readme','example_readme.cpp', cpp_args: args, dependencies:[sgl_dep])
menu2_exe = executable('menu2','menu2.cpp', cpp_args: args, dependencies:[sgl_dep])
menu_teser = executable('menu_tester','menu_tester.cpp', cpp_args: args, dependencies:[sgl_dep])

if get_option('gui')

  executable('gui_example', 
              'qt_main.cpp', 
              cpp_args: args,
              dependencies: [sgl_dep], 
              override_options: ['cpp_eh=default'])
endif