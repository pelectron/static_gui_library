project('static gui library',['cpp'], default_options:[ 'cpp_std=c++17',
                                                        'warning_level=3'])

sgl_include_dir = include_directories('include')
gcem_dep = dependency('gcem', fallback:['gcem','gcem_dep'])
dependencies = [gcem_dep]

defines = []
cxx = meson.get_compiler('cpp').get_id()
if cxx == 'clang'
  defines += '-Wno-language-extension-token'
  if host_machine.system() == 'windows'
    defines += '-Wno-microsoft-enum-value'
  endif
endif


if get_option('optimize_size')
  defines += '-DRYU_OPTIMIZE_SIZE'
endif

defines += '-DSGL_USE_CHARCONV=1'

if get_option('instantiate')
    if get_option('line_width') == 0
        error('line_width must be greater than 0')
    endif

    defines += ['-DSGL_CHAR_TYPE=@0@'.format(get_option('char_type')), '-DSGL_LINE_WIDTH=@0@'.format(get_option('line_width'))]
endif

if get_option('gui')
  subdir('include/sgl/qt')
endif

sgl_dep = declare_dependency( include_directories: sgl_include_dir, 
                              dependencies: dependencies, 
                              compile_args: defines)

if get_option('test').enabled()
  subdir('tests')
endif

if get_option('example').enabled()
  subdir('example')
endif