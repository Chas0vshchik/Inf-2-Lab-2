cmake_minimum_required(VERSION 4.0)
project(Inf-2-Lab-2)

set(CMAKE_CXX_STANDARD 20)

add_executable(Inf-2-Lab-2 main.cpp
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
        src/UI.cpp
)