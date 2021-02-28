cmake_minimum_required(VERSION 3.0)
project(skunk)

#CXX configurations
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O2 -g -Wall -Wextra -std=c++11 -pthread")
set(CMAKE_C_FLAGS "-O2 -g -Wall -pthread")

#options
option(SKUNK_UNIT_TEST "Build with Skunk unit test " ON)
option(SKUNK_EXAMPLE_BUILD "Build Skunk examples" ON)

# add google test lib 
# add_subdirectory(third_party/googletest)
# include_directories(third_party/googletest/googletest/include)
# include_directories(third_party/googletest/googlemock/include)


# add the src files of this project 
include_directories(include)
file(GLOB_RECURSE USER_LIBS_PATH src/*.cpp)
add_library(skunk STATIC ${USER_LIBS_PATH})
set_target_properties(skunk PROPERTIES OUTPUT_NAME "skunk")


# #Skunk test build function 
# function(skunk_test test_name)
#   if(SKUNK_UNIT_TEST)
#     message(STATUS "Build skunk uinit test: ${test_name}")
#     set(SRCS)
    
#     if(${ARGC} EQUAL 1)
#       message(FATAL_ERROR "Build skunk uinit test: ${test_name} failed because of the SRC list empty")
#     else()
#       # loop find the unit test source file in the test dir 
#       set(INDEX 1)
#       while(INDEX LESS ${ARGC})
#         list(APPEND SRCS ${ARGV${INDEX}})
#         math(EXPR INDEX "${INDEX} + 1")
#       endwhile()
      
#       add_executable(${test_name} ${SRCS})
#       target_link_libraries(${test_name} gtest skunk)
#     endif()
#   endif()
# endfunction(skunk_test)

# # Test Skunk Buffer 
# skunk_test(buffer_test test/buffer_test.cpp)

# Test Skunk Poller 
# TODO: test the skunk poller logic
# Test Skunk EventLoop 
# TODO: test the skunk event loop logic 
# Test Skunk Server 
# TODO: test the skunk server logic 
# Test Skunk Client 
# TODO: test the skunk client logic 

#Skunk example build function 
function(skunk_eample example_executable)
  if(SKUNK_EXAMPLE_BUILD)
    message(STATUS "Build skunk example: ${example_executable}")
    set(SRCS)
    if(${ARGC} EQUAL 1)
      message(FATAL_ERROR "Build skunk example: ${example_executable} failed because of the SRC list empty")
    else()
      # loop find the unit test source file in the test dir 
      set(INDEX 1)
      while(INDEX LESS ${ARGC})
        list(APPEND SRCS ${ARGV${INDEX}})
        math(EXPR INDEX "${INDEX} + 1")
      endwhile()
      add_executable(${example_executable} ${SRCS})
      # target_link_libraries(${example_executable} ${SRCS})
      # message(STATUS "Build skunk example: ${example_executable} ")
    endif()
  endif()
endfunction(skunk_eample)

#Example echo
# skunk_eample(echo_client example/echo/client.cpp)
skunk_eample(echo_server example/echo/server.cpp)

#Example im 

#Example socks5

#Example fstcgi 