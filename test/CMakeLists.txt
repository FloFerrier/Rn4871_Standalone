set(TEST_SOURCES
	test_main.c)

include(FetchContent)
# Fetch Unity from GitHub
FetchContent_Declare(unity GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git GIT_TAG v2.5.2)
FetchContent_MakeAvailable(unity)

# Set if you want use virtual module (true or false)
set(VIRTUAL_MODULE true)

# Set your log granularity for RN4871 driver
set(RN4871_LOG_ENABLE 0x7)

add_executable(${BUILD_TYPE} ${TEST_SOURCES})

set_target_properties(${BUILD_TYPE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}")

add_test(NAME ${BUILD_TYPE} COMMAND ${BUILD_TYPE})

target_link_libraries(${BUILD_TYPE} PUBLIC librn4871-driver unity)

target_compile_definitions(librn4871-driver PRIVATE
	RN4871_LOG_ENABLE=${RN4871_LOG_ENABLE}
)

set_target_properties(librn4871-driver PROPERTIES COMPILE_FLAGS "--coverage")
set_target_properties(librn4871-driver PROPERTIES LINK_FLAGS "--coverage")

set_target_properties(${BUILD_TYPE} PROPERTIES COMPILE_FLAGS "--coverage")
set_target_properties(${BUILD_TYPE} PROPERTIES LINK_FLAGS "--coverage")

target_compile_definitions(${BUILD_TYPE} PRIVATE VIRTUAL_MODULE=${VIRTUAL_MODULE})