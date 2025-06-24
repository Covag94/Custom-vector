# Download and link GoogleTest
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add the test executable
add_executable(test_vector test_vector.cpp)

target_include_directories(test_vector PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Link to GTest
target_link_libraries(test_vector PRIVATE gtest_main)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(test_vector)