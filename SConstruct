env = Environment()

# output object files to 'bin' folder
env.binary_dir = 'bin'
env.source_dir = 'src'
env.executable_dir = "."
env.executable_name = "dardiff"
env.VariantDir(env.binary_dir,env.source_dir)  # take source from src dir, output to binary folder
SConscript('SConscript', duplicate = 0, exports = 'env') # required file
# usually "variant_dir = env.binary_dir" is required as an argument for the above, but not now for some reason
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