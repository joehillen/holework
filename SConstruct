#
# Simple scons build file
#

flags = '-std=c++0x -Wall -Wno-unused-function -O0 -fno-inline -ggdb'
libs = 'boost_system boost_iostreams gtest'

# Set up environment
import os
env = Environment(ENV = {'PATH' : os.environ['PATH'],
                         'TERM' : os.environ['TERM'],
                         'HOME' : os.environ['HOME'],
                         'GTEST_COLOR' : 'yes'})
env.Append(CPPFLAGS = flags)
env.Append(LIBS = Split(libs))

# TODO: really this should be "g++ 4.5 or higher"?
env.Replace(CXX = 'g++-4.5')

# Export environment and run script to compile everything into the build/ dir
Export('env')
SConscript('SConscript', variant_dir='build')
