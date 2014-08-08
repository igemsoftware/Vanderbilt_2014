env = Environment()

# output object files to 'bin' folder
env.binary_dir = 'bin'
env.source_dir = 'src'
env.executable_dir = "."
env.executable_name = "dwndiff"
env.VariantDir(env.binary_dir,env.source_dir,duplicate = 0)  # output to binary folder without copying files
env.output_executable = env.Program(env.executable_name,Glob(env.binary_dir + '/*.c'))

# debug/release
env.build_mode = 'DEBUG'

# defines
env.Append(CPPDEFINES = env.build_mode)
env.dna_modes = ['ECOLI','SINGLE_START_CODON']

env.Append(CPPDEFINES = env.dna_modes)

# optimization for debug vs release
if (env.build_mode == 'DEBUG'):     # use clang for better error messages, lower compilation time
    # would be -Og but clang doesn't support that
    env.Append(CCFLAGS = ['-O0','-ggdb','-g3','-Wall','-Wextra','-Werror'])
    env.Replace(CC = 'clang')
elif (env.build_mode == 'RELEASE'): # use gcc for robustness and speed of executable for better profiling
    env.Append(CCFLAGS = ['-Ofast','-finline-functions','-fomit-frame-pointer'])
    env.Replace(CC = 'gcc')

# add glib
env.ParseConfig('pkg-config --cflags --libs glib-2.0')
# add gmp
env.Append(LIBS = ['gmp'])

# add target to remove all files
cleanTarget = env.Command("",
                          Glob(env.binary_dir + '/*.c'),
                          [
                              Delete("$TARGET")
                          ])
Alias('clean',cleanTarget)
