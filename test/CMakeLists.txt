set(TARGET "Rn4871")

set(TEST_TARGET "${CMAKE_BUILD_TYPE}_${TARGET}" CACHE INTERNAL "")

include_directories(
    ${CMAKE_SOURCE_DIR}/lib/rn4871-driver
)

add_executable(${TEST_TARGET}
    main_test.c
    test_rn4871.c
    test_virtual_module.c
    mock_rn4871.c

    ${CMAKE_SOURCE_DIR}/lib/rn4871-driver/rn4871_core.c
    ${CMAKE_SOURCE_DIR}/lib/rn4871-driver/rn4871_logger.c
    ${CMAKE_SOURCE_DIR}/lib/rn4871-driver/virtual_module.c
)

target_compile_definitions(${TEST_TARGET} PRIVATE
	-DVIRTUAL_MODULE
    RN4871_LOG_ENABLE=0x0
)

target_compile_options(${TEST_TARGET} PRIVATE
    --coverage
)

target_link_options(${TEST_TARGET} PRIVATE
    --coverage
)

target_link_libraries(${TEST_TARGET} PRIVATE rn4871-driver cmocka-static)

enable_testing()
add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})