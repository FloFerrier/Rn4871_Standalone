set(SOURCE_FILES
	./main.c)

# Set if you want use virtual module (true or false)
set(VIRTUAL_MODULE false)

add_executable(release ${SOURCE_FILES})

target_link_libraries(release PUBLIC target)

set_target_properties(release PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}")

target_compile_definitions(release PRIVATE VIRTUAL_MODULE=${VIRTUAL_MODULE})