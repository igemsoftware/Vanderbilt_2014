env = Environment()

# output object files to 'bin' folder
env.binary_dir = 'bin'
env.source_dir = 'src'
env.executable_dir = "."
env.executable_name = "dwndiff"
env.VariantDir(env.binary_dir,env.source_dir,duplicate = 0)  # output to binary
# folder without copying files
env.output_executable = env.Program(env.executable_name,Glob(env.binary_dir +
                                                             '/*.c'))
# debug/release
# run with argument 'debug=0' to release
debug = ARGUMENTS.get('debug',1) # defaults to debug configuration
if int(debug):
    env.build_mode = 'DEBUG'
else:
    env.build_mode = 'RELEASE'
env.Append(CPPDEFINES = env.build_mode)

# TODO: statically link libraries to allow for easier installation to end users

# defines
env.dna_modes = ['ECOLI','SINGLE_START_CODON']
env.Append(CPPDEFINES = env.dna_modes)
# env.process_modes = ['CONCURRENT']
# env.Append(CPPDEFINES = env.process_modes)

# flags
env.Append(CCFLAGS = ['-std=c11'])
# optimization for debug vs release
if (env.build_mode == 'DEBUG'):
    env.Append(CCFLAGS = ['-O0','-ggdb','-g3','-Wall','-Wextra','-Werror'])
elif (env.build_mode == 'RELEASE'):
    env.Append(CCFLAGS = ['-Ofast','-finline-functions','-fomit-frame-pointer',
                          '-funroll-loops'])

# compiler
env.Replace(CC = 'gcc')
# add glib
env.ParseConfig('pkg-config --cflags --libs glib-2.0')
# add gmp
env.Append(LIBS = ['gmp'])
