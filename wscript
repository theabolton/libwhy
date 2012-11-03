# -*- mode: python -*-

# libwhy build script

APPNAME='libwhy'
VERSION='20121103'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c')

def configure(conf):
    conf.load('compiler_c')

    # -FIX- should parameterize the minimum GTK+ version, so it can be substituted into libwhygui.pc
    conf.check_cfg(package='gtk+-2.0', uselib_store='GTK', atleast_version='2.16.0',
                   mandatory = True, args = '--cflags --libs')
    conf.env.LIB_READLINE = ['readline', 'curses']

    conf.define('PACKAGE_VERSION', VERSION)
    conf.write_config_header('config.h')

def build(bld):
    # libwhygui: ygtk, gtkknob
    bld.shlib(source = ['gui/gtkknob.c', 'gui/gtkcontrolpanel.c',
                            'gui/gtkcontrol.c', 'gui/ygtk.c'] +
                       bld.path.ant_glob('gui/lua/*.c'),
              target = 'whygui',
              includes = 'gui gui/why',
              defines = ['LUA_USE_LINUX', 'YGTK_USE_GTK_KNOB', 'YGTK_USE_GTK_CONTROL'],
              use = 'GTK',
              name = 'WHYGUI',
              export_includes = 'gui gui/why')  # needed for ylua below
    bld.install_files('${PREFIX}/include/why', bld.path.ant_glob('gui/why/*.h'))
    bld.install_files('${PREFIX}/include', ['gui/whygui.h'])

    # libwhyutil: y_sscanf
    bld.shlib(source = 'util/y_sscanf.c', target = 'whyutil')
    bld.install_files('${PREFIX}/include', ['util/whyutil.h'])

    # pkg-config files
    pkg_env = bld.env.copy()
    pkg_env.table.update({'VERSION': VERSION,
            'exec_prefix': bld.env['PREFIX'],
            'includedir': ''.join([bld.env['PREFIX'], '/include'])
        })
    bld(features = 'subst', source = 'libwhygui.pc.in',
        target = 'libwhygui.pc', env = pkg_env,
        install_path = '${PREFIX}/lib/pkgconfig')
    bld(features = 'subst', source = 'libwhyutil.pc.in',
        target = 'libwhyutil.pc', env = pkg_env,
        install_path = '${PREFIX}/lib/pkgconfig')

    # ylua - lua interpreter linked with libwhy
    bld.program(source = ['ylua/ylua.c'],
                target = 'bin/ylua',
                defines = ['LUA_USE_LINUX'],
                use = ['WHYGUI', 'READLINE'])

