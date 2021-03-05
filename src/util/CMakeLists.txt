  
###################################################################
#SRC CMAKELISTS
###################################################################

# CMakeLists for src directory
PROJECT(skunk)

file(GLOB_RECURSE srcs ${PROJECT_SOURCE_DIR} *.cpp )

# static library
ADD_LIBRARY(skunk STATIC ${srcs})
SET_TARGET_PROPERTIES(skunk PROPERTIES OUTPUT_NAME "skunk")
SET_TARGET_PROPERTIES(skunk PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(skunk PROPERTIES CLEAN_DIRECT_OUTPUT 1)