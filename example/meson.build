readme_exe = executable('example_readme','example_readme.cpp', include_directories: sgl_include_dir, dependencies: gcem_dep)
menu2_exe = executable('menu2','menu2.cpp', include_directories: sgl_include_dir, dependencies: gcem_dep)
menu_teser = executable('menu_tester','menu_tester.cpp', include_directories: sgl_include_dir, dependencies: gcem_dep)

if get_option('gui')
  executable('gui_example', 
              'qt_main.cpp', 
              dependencies: [sgl_dep],
              override_options: ['cpp_eh=default'])
endif