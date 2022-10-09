include_directories(${CMAKE_SOURCE_DIR}/lib/rn4871-driver/Core/inc)

set(TARGET_NAME "Rn4871_${CMAKE_BUILD_TYPE}")

set(SOURCE_FILES main.c)

# Set if you want use virtual module (true or false)
set(VIRTUAL_MODULE false)

set(LOG_FILE_NAME "${LOGS_DIR}/main.log")

add_executable(${TARGET_NAME} ${SOURCE_FILES})

target_link_libraries(${TARGET_NAME} PRIVATE rn4871-driver)

target_compile_definitions(rn4871-driver PRIVATE
	RN4871_LOG_ENABLE=0x7
)

target_compile_definitions(${TARGET_NAME} PRIVATE
	VIRTUAL_MODULE=${VIRTUAL_MODULE}
	LOG_FILE_NAME="${LOG_FILE_NAME}"
)