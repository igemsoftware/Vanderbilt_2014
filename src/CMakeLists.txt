cmake_minimum_required(VERSION 2.8) # because cmake told me to; don't know what the best practice for this is
file(GLOB sources *.h *.c)

set(DEBUG "TRUE")								# for this file only
add_definitions(-DDEBUG)				# for conditional compilation
# set(RELEASE "TRUE")
# add_definitions(-DRELEASE)
add_definitions(-DECOLI)				# support only E. coli
add_definitions(-DSINGLE_START_CODON) # only use most popular start codons

# set compile flags
set(COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic -std=c99")
if(DEBUG)
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -O0 -ggdb -g3")
elseif(RELEASE)
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -O3 -finline-functions")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}" )
	
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
add_executable(main ${sources})