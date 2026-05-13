#ifndef INF_2_LAB_2_BITSEQUENCE_H
#define INF_2_LAB_2_BITSEQUENCE_H

#include <cstdint>
#include <stdexcept>
#include "Sequence.h"
#include "DynamicArray.h"
#include "IEnumerator.h"

template<typename T>
class MutableArraySequence;

class BitSequence : public Sequence<bool> {
public:
    BitSequence(const bool *bits, size_t numBits);
    BitSequence(DynamicArray<uint8_t> *b, size_t bits);
    explicit BitSequence(const char *bitStr);
    BitSequence(const BitSequence& other);
    BitSequence& operator=(const BitSequence& other);

    [[nodiscard]] size_t GetLength() const override;

    [[nodiscard]] BitSequence *GetSubsequence(size_t startIndex, size_t endIndex) const override;
    [[nodiscard]] IEnumerator<bool> *GetEnumerator() const override;

    BitSequence& operator&=(const BitSequence& other);
    BitSequence& operator|=(const BitSequence& other);
    BitSequence& operator^=(const BitSequence& other);
    BitSequence& operator~();

    friend BitSequence operator&(const BitSequence& lhs, const BitSequence& rhs);
    friend BitSequence operator|(const BitSequence& lhs, const BitSequence& rhs);
    friend BitSequence operator^(const BitSequence& lhs, const BitSequence& rhs);

    [[nodiscard]] bool GetFirst() const override;
    [[nodiscard]] bool GetLast() const override;

    [[nodiscard]] bool getBit(size_t bitIdx) const;
    BitSequence *setBit(size_t bitIdx, bool value);

    [[nodiscard]] BitSequence *CreateEmpty() const override;

    ~BitSequence() override;

protected:
    BitSequence *DelImpl(size_t index) override;

    BitSequence *AppendImpl(const bool &elem) override;

    BitSequence *PrependImpl(const bool &elem) override;

    BitSequence *InsertAtImpl(const bool &elem, size_t index) override;

    BitSequence *ConcatImpl(const Sequence<bool> &other) override;

    BitSequence *Instance() override;


    class BitEnumerator : public IEnumerator<bool> {
    public:
        explicit BitEnumerator(const BitSequence *s) : data(s), index(0) {
        }

        bool MoveNext() override {
            if (index < data->GetLength()) {
                ++index;
                return true;
            }
            return false;
        }

        [[nodiscard]] bool Current() const override {
            if (index == 0 || index > data->GetLength())
                throw std::out_of_range("BitEnumerator: invalid access");
            return data->getByte(index - 1);
        }

        void Reset() override {
            index = 0;
        }

        ~BitEnumerator() override = default;
    private:
        const BitSequence* data;
        size_t index;
    };

    DynamicArray<uint8_t> *data;
    size_t size;

    void ensureReductionCapacity() const;

    void ensureIncreaseCapacity() const;

    [[nodiscard]] uint8_t getByte(size_t byteIdx) const;
    void setByte(size_t byteIdx, uint8_t val) const;
};

#endif //INF_2_LAB_2_BITSEQUENCE_H