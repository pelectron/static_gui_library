qt = import('qt6')
qt_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'])

moc_headers = files(
  'display_item.hpp',
  'display_page.hpp',
  'display.hpp',
  'label.hpp',
  'mainwindow.hpp',
  'menu_tree.hpp',
  'section.hpp',
)

gui_src = [
  qt.compile_resources(name:'section_rc', sources: 'resources/section.qrc'),
  qt.compile_moc( headers: moc_headers, 
                        include_directories:[include_directories('.'), inc_dir], 
                        dependencies:[sgl_dep]),
  'display_item.cpp',
  'display_page.cpp',
  'display.cpp',
  'label.cpp',
  'mainwindow.cpp',
  'menu_tree.cpp',
  'section.cpp',
]

sgl_qt_lib = static_library('sglQt', 
                      include_directories: inc_dir,
                      sources: gui_src, 
                      dependencies: [qt_dep, sgl_dep],
                      cpp_args:['-DSGL_BUILD_LIB'],
                      override_options: ['cpp_eh=default'])

gui_dep = declare_dependency(
  sources: gui_src, 
  include_directories: inc_dir,
  compile_args: ['-DSGL_BUILD_LIB'],
  dependencies: [qt_dep, sgl_dep])
