#/bin/env python
import os
from waflib import Build,Logs,Utils,Task,TaskGen,Configure,Options

top = '.'
out = 'build'

VERSION = '0.2'
APPNAME = 'ICN-Demo and App'

def options(opt):
    opt.load(['compiler_c','compiler_cxx','gnu_dirs'])
    opt.load(['default-compiler-flags','boost'],tooldir=['.waf-tools'])

def configure(conf):
    conf.load(['compiler_cxx',
                'compiler_c',
                'gnu_dirs',
                'default-compiler-flags',
                'boost'])

    if not os.environ.has_key('PKG_CONFIG_PATH'):
        os.environ['PKG_CONFIG_PATH'] = ':'.join([
            '/usr/lib/pkgconfig',
            '/usr/local/lib/pkgconfig',
            '/opt/local/lib/pkgconfig'])

    conf.check_cfg(package='libndn-cxx',args=['--cflags','--libs'],
                    uselib_store='NDN_CXX',mandatory=True)

    conf.check_cfg(package='gstreamer-1.0',args=['--cflags','--libs'],
                    uselib_store='GSTREAMER',mandatory=True)

    USED_BOOST_LIBS = ['system','iostream','filesystem','random','regex','thread','date_time','test']
    conf.check_boost(lib=USED_BOOST_LIBS,mandatory=True)

    conf.env.LIB_COMPILER = ['pthread','boost_system']

def build(bld):
    bld.recurse('src')
