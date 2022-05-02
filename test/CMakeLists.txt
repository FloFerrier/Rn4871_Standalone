set(TEST_SOURCES
	test_main.c)

# Set if you want use virtual module (true or false)
set(VIRTUAL_MODULE true)

add_executable(test_target ${TEST_SOURCES})

set_target_properties(test_target PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_test(NAME test_target COMMAND test_target)

target_link_libraries(test_target PUBLIC target unity)

set_target_properties(target PROPERTIES COMPILE_FLAGS "--coverage")
set_target_properties(target PROPERTIES LINK_FLAGS "--coverage")

set_target_properties(test_target PROPERTIES COMPILE_FLAGS "--coverage")
set_target_properties(test_target PROPERTIES LINK_FLAGS "--coverage")

target_compile_definitions(test_target PRIVATE VIRTUAL_MODULE=${VIRTUAL_MODULE})