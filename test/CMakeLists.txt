set(TARGET testing)

enable_testing()

add_executable(${TARGET})

target_link_libraries(${TARGET}
    PRIVATE
        frontend-tests
        backend-tests
        driver-tests
        util-tests
)

add_subdirectory(backend)
add_subdirectory(driver)
add_subdirectory(frontend)
add_subdirectory(test_utils)
add_subdirectory(util)

include(GoogleTest)
gtest_discover_tests(${TARGET})