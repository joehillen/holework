#
# Simple scons build file
#

flags = '-Wall -Wno-unused-function'
libs = 'boost_system'

# TODO: automate source file discovery? Just grabbing every
#  .cpp file under src/ would probably be sufficient.
sources = '''
src/main.cpp
src/events.cpp
src/player.cpp
src/network/connection.cpp
src/network/packet.cpp
src/network/packetparser.cpp
src/network/response.cpp
'''


# Set up environment
env = Environment()
env.Append(CPPFLAGS = flags)
env.Append(LIBS = Split(libs))

# Define default target
object_list = env.Object(source = Split(sources))
target = env.Program(target='server', source=object_list)

Default(target)
