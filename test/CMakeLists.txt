set(TEST_SOURCES
	test_main.c)

include(FetchContent)
# Fetch Unity from GitHub
FetchContent_Declare(unity GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git GIT_TAG v2.5.2)
FetchContent_MakeAvailable(unity)

# Set if you want use virtual module (true or false)
set(VIRTUAL_MODULE true)

add_executable(test_target ${TEST_SOURCES})

set_target_properties(test_target PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}")

add_test(NAME test_target COMMAND test_target)

target_link_libraries(test_target PUBLIC librn4871-driver unity)

set_target_properties(librn4871-driver PROPERTIES COMPILE_FLAGS "--coverage")
set_target_properties(librn4871-driver PROPERTIES LINK_FLAGS "--coverage")

set_target_properties(test_target PROPERTIES COMPILE_FLAGS "--coverage")
set_target_properties(test_target PROPERTIES LINK_FLAGS "--coverage")

target_compile_definitions(test_target PRIVATE VIRTUAL_MODULE=${VIRTUAL_MODULE})