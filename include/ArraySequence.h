#ifndef INF_2_LAB_2_ARRAYSEQUENCE_H
#define INF_2_LAB_2_ARRAYSEQUENCE_H

#include "Sequence.h"
#include "DynamicArray.h"
#include <cmath>
#include <algorithm>

template<typename T>
class MutableArraySequence;

template<typename T>
class ArraySequence : public Sequence<T> {
public:
    ArraySequence() : data(new DynamicArray<T>(0)), size(0) {}

    ArraySequence(const T *items, size_t count) : data(new DynamicArray<T>(items, count)), size(count) {}

    ArraySequence(const ArraySequence &other) : data(new DynamicArray<T>(*(other.data))), size(other.size) {}

    ~ArraySequence() override { delete data; }

    T GetFirst() const override {
        if (size == 0) throw std::out_of_range("Sequence is empty");
        return data->Get(0);
    }

    T GetLast() const override {
        if (size == 0) throw std::out_of_range("Sequence is empty");
        return data->Get(size - 1);
    }

    T Get(size_t index) const {
        if (index >= size) throw std::out_of_range("Index out of range");
        return data->Get(index);
    }

    [[nodiscard]] size_t GetLength() const override {
        return size;
    }

    Sequence<T>* DelImpl(const size_t index) override {
        if (index >= size) throw std::out_of_range("Index out of range");
        for (size_t i = index; i < size - 1; i++) {
            data->Set(i, data->Get(i + 1));
        }
        size--;
        ensureReductionCapacity();
        return this;
    }

    Sequence<T>* AppendImpl(const T& elem) override {
        ensureIncreaseCapacity();
        data->Set(size, elem);
        size++;
        return this;
    }

    Sequence<T>* PrependImpl(const T& elem) override {
        const size_t oldSize = size;
        ensureIncreaseCapacity();
        for (int i = static_cast<int>(oldSize) - 1; i >= 0; i--) {
            data->Set(i + 1, data->Get(i));
        }
        data->Set(0, elem);
        size++;
        return this;
    }

    Sequence<T>* InsertAtImpl(const T& elem, size_t index) override {
        const size_t oldSize = size;
        ensureIncreaseCapacity();
        for (int i = static_cast<int>(oldSize) - 1; i >= static_cast<int>(index); i--) {
            data->Set(i + 1, data->Get(i));
        }
        data->Set(index, elem);
        size++;
        return this;
    }

    Sequence<T>* ConcatImpl(const Sequence<T>& other) override {
        const size_t oldSize = size;
        const size_t addSize = other.GetLength();
        data->Resize(addSize + oldSize);
        auto enumerator = other.GetEnumerator();
        size_t pos = oldSize;
        while (enumerator->MoveNext()) {
            data->Set(pos++, enumerator->Current());
        }
        size = addSize + oldSize;
        return this;
    }

    Sequence<T>* CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }

    Sequence<T>* GetSubsequence(const size_t startIndex, const size_t endIndex) const override {
        if (endIndex >= size || startIndex > endIndex)
            throw std::out_of_range("Invalid subsequence bounds");
        size_t newLen = endIndex - startIndex + 1;
        T* items = new T[newLen];
        for (size_t i = 0; i < newLen; ++i) {
            items[i] = data->Get(startIndex + i);
        }
        Sequence<T>* result = new MutableArraySequence<T>(items, newLen);
        delete[] items;
        return result;
    }

    IEnumerator<T>* GetEnumerator() const override {
        class ArrayEnumerator : public IEnumerator<T> {
        public:
            ArrayEnumerator(const ArraySequence<T>* s) : data(s->data), index(0), length(s->size) {}

            bool MoveNext() override {
                return ++index <= length;
            }

            T Current() const override {
                if (index == 0 || index > length)
                    throw std::out_of_range("Enumerator out of range");
                return data->Get(index - 1);
            }

            void Reset() override {
                index = 0;
            }
        private:
            const DynamicArray<T>* data;
            size_t index;
            size_t length;
        };
        return new ArrayEnumerator(this);
    }

protected:
    DynamicArray<T>* data;
    size_t size;

    ArraySequence(DynamicArray<T>* arr, const size_t sz) : data(arr), size(sz) {}

    void ensureIncreaseCapacity() {
        const size_t currentCap = data->GetSize();
        if (size < currentCap) return;
        size_t newCap = (currentCap == 0) ? 1 : currentCap * 2;
        data->Resize(newCap);
    }

    void ensureReductionCapacity() {
        const size_t currentCap = data->GetSize();
        if (2 * size > currentCap) return;
        size_t newCap = std::max(static_cast<size_t>(1), (currentCap+1)/2);
        data->Resize(newCap);
    }
};

#endif //INF_2_LAB_2_ARRAYSEQUENCE_H