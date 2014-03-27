top = '.'
out = 'build'

def options(opt):
	opt.load('compiler_c')

def configure(cnf):
	cnf.load('compiler_c')
	cnf.check(features='c cprogram cshlib', cflags=['-Wall','-g'], defines=['var=foo'])
	cnf.check_cfg(package='wayland-client', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)
	cnf.check_cfg(package='wayland-egl', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)
	cnf.check_cfg(package='cairo-gl', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)
	cnf.check_cfg(package='xkbcommon', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)
	cnf.check_cfg(package='egl', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)

def build(bld):
	bld(features='c cprogram', 
		source='src/draw.c src/egl.c src/main.c src/shm.c src/ui.c src/util.c', 
		use=['SH_LIBS'],
		target='wl_term')