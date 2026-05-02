#ifndef INF_2_LAB_2_SEQUENCE_H
#define INF_2_LAB_2_SEQUENCE_H

#include <stdexcept>
#include "IEnumerator.h"
#include "Option.h"

template<typename T>
class MutableArraySequence;

template<typename T>
class Sequence {
public:
    virtual T GetFirst() const = 0;

    virtual T GetLast() const = 0;

    [[nodiscard]] virtual size_t GetLength() const = 0;

    virtual Sequence *GetSubsequence(size_t startIndex, size_t endIndex) const = 0;

    virtual IEnumerator<T> *GetEnumerator() const = 0;

    Sequence *Append(const T &elem) {
        return Instance()->AppendImpl(elem);
    }

    Sequence *Prepend(const T &elem) {
        return Instance()->PrependImpl(elem);
    }

    Sequence *InsertAt(const T &elem, const size_t index) {
        return Instance()->InsertAtImpl(elem, index);
    }

    Sequence *Concat(const Sequence &list) {
        return Instance()->ConcatImpl(list);
    }

    Sequence *Del(const size_t index) {
        return Instance()->DelImpl(index);
    }

    template<typename T2>
    Sequence *Map(T2 (&func)(T)) {
        Sequence *result = CreateEmpty();
        auto enumerator = this->GetEnumerator();
        while (enumerator->MoveNext()) {
            result = result->Append(func(enumerator->Current()));
        }
        return result;
    }

    Sequence *Where(bool (&func)(T)) {
        Sequence *result = CreateEmpty();
        auto enumerator = this->GetEnumerator();
        while (enumerator->MoveNext()) {
            if (func(enumerator->Current())) {
                result = result->Append(enumerator->Current());
            }
        }
        return result;
    }

    template<typename T2>
    T Reduce(T2 (&func)(T2, T)) {
        if (GetLength() == 0)
            throw std::out_of_range("Cannot reduce empty sequence");
        auto enumerator = this->GetEnumerator();
        enumerator->MoveNext();
        T acc = enumerator->Current();
        while (enumerator->MoveNext()) {
            acc = func(acc, enumerator->Current());
        }
        return acc;
    }

    Option<T> GetFirst(bool (&func)(T)) {
        auto enumerator = this->GetEnumerator();
        while (enumerator->MoveNext()) {
            if (T elem = enumerator->Current(); func(elem)) return Option<T>(elem);
        }
        return Option<T>();
    }

    Option<T> GetLast(bool (&func)(T)) {
        auto enumerator = this->GetEnumerator();
        T res{};
        bool flag = false;
        while (enumerator->MoveNext()) {
            T elem = enumerator->Current();
            if (func(elem)) {
                res = elem;
                flag = true;
            }
        }
        if (flag)
            return Option<T>(res);
        return Option<T>();
    }

    virtual ~Sequence() = default;
protected:
    virtual Sequence *Instance() = 0;

    virtual Sequence *AppendImpl(const T &elem) = 0;

    virtual Sequence *PrependImpl(const T &elem) = 0;

    virtual Sequence *InsertAtImpl(const T &elem, size_t index) = 0;

    virtual Sequence *ConcatImpl(const Sequence &other) = 0;

    virtual Sequence *DelImpl(size_t index) = 0;

    virtual Sequence *CreateEmpty() const = 0;
};

#endif //INF_2_LAB_2_SEQUENCE_H