set(SOURCE_FILES
	./main.c)

# Set if you want use virtual module (true or false)
set(VIRTUAL_MODULE false)

set(LOG_FILE_NAME "${LOGS_DIR}/main.log")

# Set your log granularity for RN4871 driver
set(RN4871_LOG_ENABLE 0x7)

add_executable(${BUILD_TYPE} ${SOURCE_FILES})

target_link_libraries(${BUILD_TYPE} PRIVATE librn4871-driver)

target_compile_definitions(librn4871-driver PRIVATE
	RN4871_LOG_ENABLE=${RN4871_LOG_ENABLE}
)

set_target_properties(${BUILD_TYPE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}")

target_compile_definitions(${BUILD_TYPE} PRIVATE
	VIRTUAL_MODULE=${VIRTUAL_MODULE}
	LOG_FILE_NAME="${LOG_FILE_NAME}"
)