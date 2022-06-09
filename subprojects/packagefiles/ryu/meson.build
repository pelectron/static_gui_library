project('ryu','c')
inc_dir = include_directories('.')
double_src = ['ryu/d2fixed.c','ryu/d2s.c','ryu/s2d.c']
float_src = ['ryu/f2s.c','ryu/s2f.c']
ryu_dep = declare_dependency(include_directories:inc_dir, sources:[double_src, float_src])