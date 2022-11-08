qt_name = 'qt@0@'.format(get_option('qt_major_version'))
qt_mod = import(qt_name)
dependencies += dependency(qt_name, modules: ['Core', 'Gui', 'Widgets'])

cxx = meson.get_compiler('cpp').get_id()

if cxx == 'clang'
  defines += '-Wno-language-extension-token'
  if host_machine.system() == 'windows'
    defines += '-Wno-microsoft-enum-value'
  endif
endif

moc_headers = files(
  'display_item.hpp',
  'display_page.hpp',
  'display.hpp',
  'label.hpp',
  'mainwindow.hpp',
  'section.hpp',
)

visualizer_src = [
  qt_mod.compile_resources( name:'section_rc', 
                            sources: 'resources/section.qrc'),
  qt_mod.compile_moc( headers: moc_headers, 
                      dependencies: core_dep,
                      extra_args: '-DSGL_BUILD_LIB'),
  'display_item.cpp',
  'display_page.cpp',
  'display.cpp',
  'label.cpp',
  'mainwindow.cpp',
  'menu_tree.cpp',
  'section.cpp',
]

dependencies += declare_dependency(
  sources: visualizer_src, 
  compile_args: ['-DSGL_BUILD_LIB', defines],
  include_directories: sgl_include_dir,
  dependencies: core_dep
)
