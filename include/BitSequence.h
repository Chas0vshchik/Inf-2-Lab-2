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

    [[nodiscard]] size_t GetLength() const override;

    [[nodiscard]] BitSequence *GetSubsequence(size_t startIndex, size_t endIndex) const override;

    [[nodiscard]] IEnumerator<bool> *GetEnumerator() const override;

    [[nodiscard]] BitSequence *And(const BitSequence &other) const;

    [[nodiscard]] BitSequence *Or(const BitSequence &other) const;

    [[nodiscard]] BitSequence *Xor(const BitSequence &other) const;

    [[nodiscard]] BitSequence *Not() const;

    [[nodiscard]] bool GetFirst() const override;

    [[nodiscard]] bool GetLast() const override;

    [[nodiscard]] uint8_t getByte(size_t byteIdx) const;

    void setByte(size_t byteIdx, uint8_t val) const;

    [[nodiscard]] bool getBit(size_t bitIdx) const;

    BitSequence *setBit(size_t bitIdx, bool value);

    [[nodiscard]] BitSequence *CreateEmpty() const override;

    [[nodiscard]] std::string toDecimalString() const;

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
        explicit BitEnumerator(const BitSequence *s) : seq(s), currentIdx(0) {
        }

        bool MoveNext() override {
            if (currentIdx < seq->GetLength()) {
                ++currentIdx;
                return true;
            }
            return false;
        }

        [[nodiscard]] bool Current() const override {
            if (currentIdx == 0 || currentIdx > seq->GetLength())
                throw std::out_of_range("BitEnumerator: invalid access");
            return seq->getBit(currentIdx - 1);
        }

        void Reset() override {
            currentIdx = 0;
        }

        ~BitEnumerator() override = default;
    private:
        const BitSequence *seq;
        size_t currentIdx;
    };
private:
    DynamicArray<uint8_t> *bytes;
    size_t bitSize;

    void ensureReductionCapacity() const;

    void ensureIncreaseCapacity() const;
};

#endif //INF_2_LAB_2_BITSEQUENCE_H