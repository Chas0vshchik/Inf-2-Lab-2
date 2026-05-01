#ifndef INF_2_LAB_2_IMMUTABLELISTSEQUENCE_H
#define INF_2_LAB_2_IMMUTABLELISTSEQUENCE_H

#include "ListSequence.h"

template<typename T>
class ImmutableListSequence : public ListSequence<T> {
public:
    ImmutableListSequence() : ListSequence<T>() {}

    ImmutableListSequence(const T *items, size_t count) : ListSequence<T>(items, count) {}

    ImmutableListSequence(const ImmutableListSequence &other) : ListSequence<T>(other) {}

    ImmutableListSequence(LinkedList<T> *list, size_t size) : ListSequence<T>(list, size) {}

protected:
    Sequence<T> *Instance() override {
        return new ImmutableListSequence(*this);
    }
};

#endif //INF_2_LAB_2_IMMUTABLELISTSEQUENCE_H