cmake_minimum_required(VERSION 2.8) # because cmake told me to; don't know what the best practice for this is
file(GLOB sources *.h *.c)
add_definitions(-DDEBUG)
SET(CLANG_COMPILE_FLAGS "-O0 -ggdb")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CLANG_COMPILE_FLAGS}")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(main ${sources})