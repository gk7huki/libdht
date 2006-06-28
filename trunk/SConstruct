# Build system copied/modified from
# http://www.scons.org/cgi-bin/wiki/AdvancedBuildExample
# Original by zedshaw@zedshaw.com
import os
import sys
from build_support import *
from build_config import *

opts = Options('custom.py')
opts.Add(EnumOption('debug', 'Build with debug symbols', 'no',
                    ['yes','no']))
env = Environment(options = opts, tools=['mingw'])
Help(opts.GenerateHelpText(env))

mode = "Release"
env.Append(CPPFLAGS = flgs)
if (env.get('debug') == 'yes'):
    print "Debug build"
    mode = "Debug"
    env.Append(CPPFLAGS = ['-g'])
    env.Append(CPPDEFINES = 'DEBUG')
else:
    env.Append(CPPFLAGS = ['-O3'])
    env.Append(CPPDEFINES = 'NDEBUG')
    
# Construct target directories and names. Since the
# build specific SConscript file is one level above
# the build dir, some trickery is required.
target_dir_base = '#' + SelectBuildDir(build_base_dir)
target_dir      = os.sep.join([target_dir_base, mode])
target_name     = os.sep.join([mode, target_name])

## Get the sources
sources_raw = DirGlob(source_base_dir, '*.cpp') # glob.glob(source_base_dir + os.sep + '*.cpp')
## Now must make a pure version relative to the build directory
## in Release/Debug (without the source_base_dir at start)
sources = []
for source in sources_raw:
    sources.append(os.sep.join([target_dir] + source.split(os.sep)[1:]))

# setup the include paths
env.Append(CPPPATH=include_search_path)
env.Append(LIBS=libs)
env.Append(LIBPATH=lib_search_path)
env.Append(CPPDEFINES=defs)

# start the build

# variables the sub build directories need
Export('env', 'sources', 'mode', 'target_name')
env.BuildDir(target_dir, source_base_dir, duplicate=0)
env.SConscript(target_dir_base + os.sep + 'SConscript')

