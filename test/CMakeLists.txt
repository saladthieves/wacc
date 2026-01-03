set(TARGET testing)

enable_testing()

add_executable(${TARGET})

target_link_libraries(${TARGET}
    PRIVATE
        # frontend-tests
        # tacky-tests
        backend-tests
        # driver-tests
        # util-tests
)

add_subdirectory(backend)
add_subdirectory(base)
add_subdirectory(driver)
add_subdirectory(frontend)
add_subdirectory(tacky)
add_subdirectory(test_utils)
add_subdirectory(util)

add_subdirectory(base/matchers)
add_subdirectory(base/formatters)

include(GoogleTest)
gtest_discover_tests(${TARGET})