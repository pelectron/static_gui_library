readme_exe = executable('example_readme','example_readme.cpp', dependencies:[sgl_dep])
menu2_exe = executable('menu2','menu2.cpp',dependencies:[sgl_dep])
menu_teser = executable('menu_tester','menu_tester.cpp',dependencies:[sgl_dep])

if get_option('gui')
  executable('gui_example', 'qt_main.cpp', 
                            dependencies: [gui_dep], 
                            cpp_args:['-Wno-microsoft-enum-value','-Wno-language-extension-token'],
                            override_options: ['cpp_eh=default'])
endif