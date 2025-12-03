# Google Test

set(M_GTEST_HASH 52eb8108c5bdec04579160ae17225d66034bd723)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/${M_GTEST_HASH}.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
