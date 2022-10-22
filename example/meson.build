readme_exe = executable('example_readme','example_readme.cpp', dependencies:[sgl_dep])
menu2_exe = executable('menu2','menu2.cpp',dependencies:[sgl_dep])
menu_teser = executable('menu_tester','menu_tester.cpp',dependencies:[sgl_dep])

if get_option('gui')
  args = ['-Wno-language-extension-token']
  if host_machine.system() == 'windows'
    args += '-Wno-microsoft-enum-value'
  endif
  executable('gui_example', 
              'qt_main.cpp', 
              cpp_args: args,
              dependencies: [sgl_dep], 
              override_options: ['cpp_eh=default'])
endif