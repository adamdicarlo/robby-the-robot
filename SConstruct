#
# SConstruct: Describes how to build Robby the Robot. This file is used by the
# SCons build system.
#
env = Environment()

debug = ARGUMENTS.get('debug', 0)

# use 'debug=1' as command line argument
if int(debug):
        env.Append(CCFLAGS='-ggdb3 -DDEBUG')
else:
        env.Append(CCFLAGS='-O3 -DNDEBUG')

# -rdynamic allows stack traces to show meaningful function names
env.Append(CCFLAGS='-Wall')
env.Append(LINKFLAGS='-rdynamic')
env.Program('robby', ['main.c', 'robby.c', 'error.c', 'misc.c', 'parse.c', 'population.c', 'strategy.c', 'world.c'])
