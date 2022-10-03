qt = import('qt6')
qt_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'])

gui_src = qt.compile_resources(name:'section_rc', sources: 'resources/section.qrc')

gui_src += qt.compile_moc( headers: ['qt_item.hpp','qt_menu.hpp', 'qt_page.hpp', 'section.hpp', 'label.hpp','mainwindow.hpp', 'display.hpp'], 
                        include_directories:include_directories('.'), 
                        dependencies:[sgl_dep])
gui_src += [
  'main.cpp', 
  'qt_item.cpp',
  'qt_page.cpp',
  'qt_menu.cpp', 
  'section.cpp', 
  'label.cpp',
  'mainwindow.cpp',
  'menu_tree.cpp',
  'display.cpp'
]

executable('gui_test',  sources: gui_src, 
                        dependencies: [qt_dep, sgl_dep], cpp_args:['-Wno-microsoft-enum-value','-Wno-language-extension-token'],
                        override_options: ['cpp_eh=default'])
