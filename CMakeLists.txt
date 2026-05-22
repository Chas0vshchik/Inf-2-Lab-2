cmake_minimum_required(VERSION 4.0)
project(Inf-2-Lab-2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SFML_ROOT "C:/ChasPrograms/SFML-3.1.0")
set(SFML_DIR "${SFML_ROOT}/lib/cmake/SFML")

# Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

# Общие заголовочные файлы
set(COMMON_HEADERS
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
        include/Lab3-Segment.h
        include/Lab3-PiecewiseFunc.h
)
set(COMMON_SOURCES
        src/BItSequence.cpp
)

# Executable 1: UI
add_executable(ui UI.cpp ${COMMON_HEADERS} ${COMMON_SOURCES})

# Executable 2: Tests
add_executable(tests tests.cpp ${COMMON_HEADERS} ${COMMON_SOURCES})
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)
enable_testing()
include(GoogleTest)
gtest_discover_tests(tests)

# Executable 3: UI3 with SFML graphics
find_package(SFML 3.1 REQUIRED COMPONENTS Graphics Window System)
add_executable(ui3 UI3.cpp ${COMMON_HEADERS} ${COMMON_SOURCES})

# SFML
target_link_libraries(ui3 PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)