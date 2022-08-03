set(SOURCE_FILES
	./main.c)

# Set if you want use virtual module (true or false)
set(VIRTUAL_MODULE true)

add_executable(release ${SOURCE_FILES})

target_link_libraries(release PUBLIC librn4871-driver)

set_target_properties(release PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}")

target_compile_definitions(release PRIVATE VIRTUAL_MODULE=${VIRTUAL_MODULE})