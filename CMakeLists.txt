cmake_minimum_required(VERSION 4.0)
project(Inf-2-Lab-2)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

add_executable(ui UI.cpp
        include/ArraySequence.h
        include/BItSequence.h
        include/DynamicArray.h
        include/IEnumerator.h
        include/ImmutableArraySequence.h
        include/ImmutableListSequence.h
        include/LinkedList.h
        include/ListSequence.h
        include/MutableArraySequence.h
        include/MutableListSequence.h
        include/Option.h
        include/Sequence.h
        include/UI.h
        src/BItSequence.cpp
)

add_executable(tests tests.cpp
        include/ArraySequence.h
        include/BItSequence.h
        include/DynamicArray.h
        include/IEnumerator.h
        include/ImmutableArraySequence.h
        include/ImmutableListSequence.h
        include/LinkedList.h
        include/ListSequence.h
        include/MutableArraySequence.h
        include/MutableListSequence.h
        include/Option.h
        include/Sequence.h
        src/BItSequence.cpp
)

target_link_libraries(tests
        PRIVATE
        GTest::gtest
        GTest::gtest_main
)
enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)