cmake_minimum_required(VERSION 3.16)
project(framework-test-c)

set(CMAKE_C_STANDARD 99)

set(BINARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(SOURCE_DIR  ${CMAKE_SOURCE_DIR}/src)
set(TEST_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(LOGS_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/logs)

include_directories(${LIBRARY_DIR}/rn4871-driver)
add_subdirectory(${LIBRARY_DIR}/rn4871-driver)

message(INFO " CMAKE_BUILD_TYPE= ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	set(BUILD_TYPE debug)
	include_directories(${SOURCE_DIR})
	add_subdirectory(${SOURCE_DIR})
	message(INFO " Debug selected.")

elseif(CMAKE_BUILD_TYPE STREQUAL Release)
	set(BUILD_TYPE release)
	include_directories(${SOURCE_DIR})
	add_subdirectory(${SOURCE_DIR})
	message(INFO " Release selected.")

elseif(CMAKE_BUILD_TYPE STREQUAL Test)
	include(CTest)
	include(cmake/cmocka.cmake)
	set(BUILD_TYPE test)
	include_directories(${TEST_DIR})
	add_subdirectory(${TEST_DIR})
	message(INFO " Test selected.")

else()
	message(ERROR " Specify CMAKE_BUILD_TYPE as Debug, Test or Release")

endif()
