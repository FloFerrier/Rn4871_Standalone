set(SOURCE_FILES
	./main.c)

# Set if you want use virtual module (true or false)
set(VIRTUAL_MODULE true)

set(LOG_FILE_NAME "${LOGS_DIR}/main.log")

add_executable(${BUILD_TYPE} ${SOURCE_FILES})

target_link_libraries(${BUILD_TYPE} PUBLIC librn4871-driver)

set_target_properties(${BUILD_TYPE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}")

target_compile_definitions(${BUILD_TYPE} PRIVATE
	VIRTUAL_MODULE=${VIRTUAL_MODULE}
	LOG_FILE_NAME="${LOG_FILE_NAME}"
)