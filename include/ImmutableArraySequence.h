#ifndef INF_2_LAB_2_IMMUTABLEARRAYSEQUENCE_H
#define INF_2_LAB_2_IMMUTABLEARRAYSEQUENCE_H

#include "ArraySequence.h"

template<typename T>
class ImmutableArraySequence : public ArraySequence<T> {
public:
    ImmutableArraySequence() : ArraySequence<T>() {}

    ImmutableArraySequence(const T *items, size_t count) : ArraySequence<T>(items, count) {}

    ImmutableArraySequence(const ImmutableArraySequence &other) : ArraySequence<T>(other) {}

    ImmutableArraySequence(DynamicArray<T> *arr, size_t size) : ArraySequence<T>(arr, size) {}

protected:
    Sequence<T> *Instance() override {
        return new ImmutableArraySequence(*this);
    }
};

#endif //INF_2_LAB_2_IMMUTABLEARRAYSEQUENCE_H