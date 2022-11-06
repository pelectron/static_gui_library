
qt_name = 'qt@0@'.format(get_option('qt_major_version'))
qt_mod = import(qt_name)
dependencies += dependency(qt_name, modules: ['Core', 'Gui', 'Widgets'])

moc_headers = files(
  'display_item.hpp',
  'display_page.hpp',
  'display.hpp',
  'label.hpp',
  'mainwindow.hpp',
  'section.hpp',
)

gui_src = [
  qt_mod.compile_resources(name:'section_rc', 
                       sources: 'resources/section.qrc'),
  qt_mod.compile_moc( headers: moc_headers, 
                  include_directories:[include_directories('.'), sgl_include_dir]),
  'display_item.cpp',
  'display_page.cpp',
  'display.cpp',
  'label.cpp',
  'mainwindow.cpp',
  'menu_tree.cpp',
  'section.cpp',
]

dependencies += declare_dependency(
  sources: gui_src, 
  compile_args: '-DSGL_BUILD_LIB',
  include_directories: sgl_include_dir,
  dependencies: [dependencies]
)
