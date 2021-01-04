cmake_minimum_required(3.0)

project(skunk)

# add google test deps 
add_subdirectory(third_party/googletest)
include_directories(third_party/googletest/googletest/include)
include_directories(third_party/googletest/googlemock/include)

# add test 
add_subdirectory(test)