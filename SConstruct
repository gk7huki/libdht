# Build system copied/modified from
# http://www.scons.org/cgi-bin/wiki/AdvancedBuildExample
# Original by zedshaw@zedshaw.com
import os
import sys
from build_support import *
from build_config import *

opts = Variables('custom.py')
opts.Add(EnumVariable('debug', 'Build with debug symbols', 'no',
                    allowed_values=('yes','no')))
opts.Add(EnumVariable('win32', 'Build for win32 target', 'no',
                    allowed_values=('yes','no')))
env = Environment(variables = opts) # , tools=['mingw'])
Help("\nType 'scons' to build the library\n")
Help("\nType 'scons example' to build the examples\n")
# Help("\nType 'scons test' to build and run the unit tests\n")
Help(opts.GenerateHelpText(env))

platform = None
if (env.get('win32') == 'yes'):
    env.Tool('mingw')
    env['CC'] = 'i686-w64-mingw32-gcc'
    env['CXX'] = 'i686-w64-mingw32-g++'
    env['AR'] = 'i686-w64-mingw32-ar'
    env['RANLIB'] = 'i686-w64-mingw32-ranlib'
    env['BUILD_PLATFORM'] = 'win32'
    platform = 'win32'
else:
    env.Tool('gcc')
    env['BUILD_PLATFORM'] = sys.platform

mode = "Release"
env['BUILD_VARIANT']  = mode
env.Append(CPPFLAGS = cflags)
env.Append(LINKFLAGS = lflags)
if (env.get('debug') == 'yes'):
    print("Debug build")
    mode = "Debug"
    env.Append(CPPFLAGS = ['-g'])
    env.Append(CPPDEFINES = 'DEBUG')
else:
    env.Append(CPPFLAGS = ['-O3'])
    env.Append(CPPDEFINES = 'NDEBUG')
    env.Append(LINKFLAGS = ['-s'])

# Construct target directories and names. Since the
# build specific SConscript file is one level above
# the build dir, some trickery is required.
platform_dir = '#' + SelectBuildDir(build_base_dir, platform)
build_dir    = os.path.join(platform_dir, mode)
target_dir   = build_dir

# setup the include paths
env.Append(CPPPATH=include_search_path)
env.Append(LIBS=libs)
env.Append(LIBPATH=lib_search_path)
env.Append(CPPDEFINES=defs)

env.VariantDir(build_dir, source_base_dir, duplicate=0)

# Executable building environment needs usually some more
# specifications such as platform specific libraries.
exe_env = env.Clone()
exe_env.Append(LIBS=exe_libs)

# Until SCons gets its own SCons.Glob function, we will
# have to do some trickery to get all the compiled files.
# Calling BuildDir requires the source files to be
# specified as if they were in the build directory.
# So glob the files from the real sources but replacing
# the start with the target dir.
# Once scons has its Glob this trickery can be
# probably removed and selection of files moved
# to the src/SConscript file.
lib_sources = example_sources = test_sources = []
lib_sources = DirGlob(dir         = lib_source_dir, 
                      match       = '*.cpp', 
                      dir_match   = source_base_dir,
                      dir_replace = build_dir)

env_exports = ['env', 'exe_env', 'lib_sources', 
              'target_name', 'target_dir',
              'example_sources',
              'test_sources', 'test_libs']

if 'test' in BUILD_TARGETS:
    test_sources = DirGlob(dir         = test_source_dir, 
                           match       = '*.cpp', 
                           dir_match   = source_base_dir,
                           dir_replace = build_dir)
else:
    test_sources = []
    test_libs = []

if 'example' in BUILD_TARGETS:
    example_sources = DirGlob(dir         = example_source_dir, 
                              match       = '*.cpp', 
                              dir_match   = source_base_dir,
                              dir_replace = build_dir)
else:
    example_sources = []

Export(env_exports)
# The first SConscripts calls platform specific
# configurations. The second one creates 
# configuration for building the sources.
env.SConscript(dirs = platform_dir)
env.SConscript(dirs = build_dir)
