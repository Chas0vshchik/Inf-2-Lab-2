#ifndef INF_2_LAB_2_DYNAMICARRAY_H
#define INF_2_LAB_2_DYNAMICARRAY_H

#include <stdexcept>

template<class T>
class DynamicArray {
public:
    DynamicArray(const T *items, const size_t count) : size(count) {
        if (items == nullptr) {
            throw std::invalid_argument("items is nullptr");
        }
        data = new T[count];
        for (size_t i = 0; i < count; i++) {
            data[i] = items[i];
        }
    }

    explicit DynamicArray(const size_t size) : size(size) {
        data = new T[size];
        for (size_t i = 0; i < size; i++) data[i] = T();
    }

    DynamicArray(const DynamicArray &dynamicArray) : size(dynamicArray.size) {
        data = new T[dynamicArray.size];
        for (size_t i = 0; i < dynamicArray.size; i++) data[i] = dynamicArray.data[i];
    }

    ~DynamicArray() {
        delete[] data;
    }

    DynamicArray &operator=(const DynamicArray &dynamicArray) {
        if (this != &dynamicArray) {
            delete[] data;
            data = new T[dynamicArray.size];
            for (size_t i = 0; i < dynamicArray.size; i++) data[i] = dynamicArray.data[i];
            size = dynamicArray.size;
        }
        return *this;
    }

    T Get(size_t index) const {
        if (index >= size) {
            throw std::out_of_range("IndexOutOfRange");
        }
        return data[index];
    }

    [[nodiscard]] size_t GetSize() const {
        return size;
    }

    void Set(size_t index, const T &value) {
        if (index >= size) {
            throw std::out_of_range("IndexOutOfRange");
        }
        data[index] = value;
    }

    void Resize(size_t newSize) {
        T *newData = new T[newSize];
        for (size_t i = 0; i < std::min(newSize, size); i++) newData[i] = data[i];
        delete[] data;
        for (size_t i = std::min(size, newSize); i < newSize; i++) newData[i] = T();
        data = newData;
        size = newSize;
    }
private:
    T *data;
    size_t size;
};

#endif //INF_2_LAB_2_DYNAMICARRAY_H