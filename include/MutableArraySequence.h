#ifndef INF_2_LAB_2_MUTABLEARRAYSEQUENCE_H
#define INF_2_LAB_2_MUTABLEARRAYSEQUENCE_H

#include "ArraySequence.h"

template<typename T>
class MutableArraySequence : public ArraySequence<T> {
public:
    MutableArraySequence() : ArraySequence<T>() {}

    MutableArraySequence(const T *items, size_t count) : ArraySequence<T>(items, count) {}

    MutableArraySequence(const MutableArraySequence &other) : ArraySequence<T>(other) {}

    MutableArraySequence(DynamicArray<T> *arr, size_t size) : ArraySequence<T>(arr, size) {}

protected:
    Sequence<T> *Instance() override {
        return this;
    }
};

#endif //INF_2_LAB_2_MUTABLEARRAYSEQUENCE_H