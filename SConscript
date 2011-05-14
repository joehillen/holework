#
# scons script specifying just the sources and target for the project
#

Import('env')

sources = Glob('src/*.cpp') + Glob('src/*/*.cpp')
objects = env.Object(source = sources)
target = env.Program( target='server', source = objects)

test_alias = env.Alias('test', [target], target[0].abspath + ' --test ')
AlwaysBuild(test_alias)

Default(target)
Default(test_alias)
