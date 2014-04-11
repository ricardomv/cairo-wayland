top = '.'
out = 'build'

def options(opt):
	opt.load('compiler_c')

def configure(cnf):
	cnf.load('compiler_c')
	cnf.check(features='c cprogram cshlib', cflags=['-Wall','-g'], defines=['var=foo'])
	cnf.check_cfg(package='wayland-client', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)
	cnf.check_cfg(package='xkbcommon', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)
	cnf.check_cfg(package='egl', args='--cflags --libs', mandatory=False)
	if cnf.env.LIB_EGL:
		cnf.check_cfg(package='cairo-gl', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)
		cnf.check_cfg(package='wayland-egl', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)
	else:
		cnf.check_cfg(package='cairo', args='--cflags --libs', uselib_store='SH_LIBS', mandatory=True)


	cnf.write_config_header('config.h')

def build(bld):
	if bld.env.LIB_EGL:
		source='src/egl.c '
	else:
		source='src/shm.c '

	bld(features='c cprogram', 
		source=source+'src/draw.c src/main.c src/ui.c src/util.c',
		cflags=['-Wall','-g', '-I./'],
		use=['SH_LIBS','EGL'],
		target='wl_term')
