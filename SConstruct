#
# Top-level scons build file; sets up environment and runs target build script
#

# TODO: we really should have some way of saying "g++4.5 or higher". For that
#   matter, Microsoft's C++ compiler should work too, with appropriate flags.
cxx = 'g++-4.5'
flags = '-std=c++0x -Wall -Werror -Wno-unused-function -O0 -fno-inline -ggdb'
paths = '.'

# Set up environment
import os
env = Environment(ENV = {'PATH' : os.environ['PATH'],
                         'TERM' : os.environ['TERM'],
                         'HOME' : os.environ['HOME'],
                         'GTEST_COLOR' : 'yes'})
env.Replace(CXX = cxx)
env.Append(CPPFLAGS = flags)
env.Replace(CPPPATH = paths)

# Export environment and run script to compile everything into the build/ dir
Export('env')
env.SConscript('src/SConscript', variant_dir='build', duplicate=False)

