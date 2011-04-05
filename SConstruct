#
# Simple scons build file
#

flags = '-Wall -Wno-unused-function'
libs = 'boost_system boost_iostreams'

# Set up environment
env = Environment()
env.Append(CPPFLAGS = flags)
env.Append(LIBS = Split(libs))

# Export environment and run script to compile everything into the build/ dir
Export('env')
SConscript('SConscript', variant_dir='build')
