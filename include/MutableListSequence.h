#ifndef INF_2_LAB_2_MUTABLELISTSEQUENCE_H
#define INF_2_LAB_2_MUTABLELISTSEQUENCE_H

#include "ListSequence.h"

template<typename T>
class MutableListSequence : public ListSequence<T> {
public:
    MutableListSequence() : ListSequence<T>() {}

    MutableListSequence(const T *items, size_t count) : ListSequence<T>(items, count) {}

    MutableListSequence(const MutableListSequence<T> &other) : ListSequence<T>(other) {}

    MutableListSequence(LinkedList<T> *list, size_t size) : ListSequence<T>(list, size) {}

protected:
    Sequence<T> *Instance() override {
        return this;
    }
};

#endif //INF_2_LAB_2_MUTABLELISTSEQUENCE_H