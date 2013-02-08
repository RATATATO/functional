CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_TESTING()

PROJECT(compose)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
INCLUDE_DIRECTORIES(.)

#SET(CMAKE_BUILD_TYPE DEBUG)
SET(CMAKE_BUILD_TYPE RELEASE)
SET(CMAKE_CXX_COMPILER clang++)
#SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

ADD_EXECUTABLE(compose src/compose.cc)
TARGET_LINK_LIBRARIES(compose boost_system boost_timer)

MACRO (add_unit_test target_name)
	TARGET_LINK_LIBRARIES(${target_name} boost_unit_test_framework) 
	ADD_TEST(${target_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name})
ENDMACRO (add_unit_test)

ADD_EXECUTABLE(test_compose src/test_compose.cc)
add_unit_test(test_compose)

ADD_EXECUTABLE(test_optional_control src/test_optional_control.cc)
add_unit_test(test_optional_control)

