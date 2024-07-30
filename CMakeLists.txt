cmake_minimum_required(VERSION 3.26)
project(LearningPtr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.0
)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Add executable for your test cases
add_executable(LearningPtrTest
        src/UniquePtrTest.cpp
)

# Link GoogleTest with your test executable
target_link_libraries(LearningPtrTest gtest_main gtest)

# Enable testing
enable_testing()

# Add test cases
add_test(
        NAME UniquePtrTests
        COMMAND LearningPtrTest
)