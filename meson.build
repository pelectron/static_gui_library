project('static gui',['cpp','c'], default_options:[ 'buildtype=debug',
                                                    'cpp_std=c++17',
                                                    'cpp_eh=none',
                                                    'cpp_rtti=false',
                                                    'c_std=c11',
                                                    'warning_level=3'])
inc_dir = include_directories('include')
dependencies = [dependency('gcem', fallback:['gcem','gcem_dep'])]
defines = []


if get_option('optimize_size')
  defines += '-DRYU_OPTIMIZE_SIZE'
endif

defines += '-DSGL_USE_CHARCONV=1'

if get_option('instantiate')
    if get_option('line_width') == 0
        error('library line must be greater than 0')
    endif

    defines += ['-DSGL_CHAR_TYPE=@0@'.format(get_option('char_type')), '-DSGL_LINE_WIDTH=@0@'.format(get_option('line_width'))]
endif

if meson.get_compiler('cpp').get_id() == 'clang'
  defines += '-ferror-limit=1'
endif

sgl_dep = declare_dependency( include_directories: inc_dir, 
                              dependencies: dependencies, 
                              compile_args: defines)

if get_option('gui')
  subdir('include/sgl/qt')
endif

if get_option('test').enabled()
  catch_dep = dependency('catch2')
  subdir('tests')
  subdir('example')
endif
