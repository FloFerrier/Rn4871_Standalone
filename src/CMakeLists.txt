set(SOURCE_FILES
	./main.c)

add_executable(release ${SOURCE_FILES})

target_link_libraries(release PUBLIC target)

set_target_properties(release PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")