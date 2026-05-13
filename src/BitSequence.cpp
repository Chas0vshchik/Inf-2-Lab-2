#include "../include//BitSequence.h"
#include "../include/DynamicArray.h"
#include "../include/Sequence.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>

void BitSequence::ensureReductionCapacity() const {
    const size_t currentCap = data->GetSize();
    if (size / 8 > currentCap) return;
    const size_t newCap = std::max(static_cast<size_t>(1), static_cast<size_t>(currentCap) - 1);
    data->Resize(newCap);
}

void BitSequence::ensureIncreaseCapacity() const {
    const size_t currentCap = data->GetSize();
    if (size / 8 < currentCap) return;
    const size_t newCap = currentCap + 1;
    data->Resize(newCap);
}

uint8_t BitSequence::getByte(const size_t byteIdx) const {
    return data->Get(byteIdx);
}

void BitSequence::setByte(const size_t byteIdx, const uint8_t val) const {
    data->Set(byteIdx, val);
}

bool BitSequence::getBit(const size_t bitIdx) const {
    if (bitIdx >= size)
        throw std::out_of_range("bit index out of range");
    const size_t byteIdx = bitIdx / 8;
    const size_t bitPos = bitIdx % 8;
    return (getByte(byteIdx) >> bitPos) & 1;
}

BitSequence *BitSequence::setBit(const size_t bitIdx, const bool value) {
    if (bitIdx >= size)
        throw std::out_of_range("bit index out of range");

    const size_t byteIdx = bitIdx / 8;
    const size_t bitPos = bitIdx % 8;
    uint8_t byteVal = data->Get(byteIdx);

    if (value) {
        byteVal |= static_cast<uint8_t>(1u << bitPos);
    } else {
        byteVal &= static_cast<uint8_t>(~(1u << bitPos));
    }

    data->Set(byteIdx, byteVal);
    return this;
}

BitSequence::BitSequence(DynamicArray<uint8_t> *b, const size_t bits)
    : data(b), size(bits) {
}

BitSequence::BitSequence(const bool *bits, const size_t numBits) : size(numBits) {
    if (!bits || numBits <= 0)
        throw std::invalid_argument("bits is nullptr");

    const size_t byteCount = (numBits + 7) / 8;
    data = new DynamicArray<uint8_t>(byteCount);

    for (size_t i = 0; i < numBits; ++i) {
        if (bits[i]) {
            const size_t bIdx = i / 8;
            const size_t bPos = i % 8;
            uint8_t val = data->Get(bIdx);
            val |= static_cast<uint8_t>(1u << bPos);
            data->Set(bIdx, val);
        }
    }
}

BitSequence::BitSequence(const char *bitStr) {
    if (!bitStr)
        throw std::invalid_argument("bitStr is nullptr");

    size = std::strlen(bitStr);
    for (size_t i = 0; i < size; ++i) {
        if (bitStr[i] != '0' && bitStr[i] != '1')
            throw std::invalid_argument("invalid character");
    }

    const size_t byteCount = (size + 7) / 8;
    data = new DynamicArray<uint8_t>(byteCount);

    for (size_t i = 0; i < size; ++i) {
        if (bitStr[i] == '1') {
            const size_t bIdx = i / 8;
            const size_t bPos = i % 8;
            uint8_t val = data->Get(bIdx);
            val |= static_cast<uint8_t>(1u << bPos);
            data->Set(bIdx, val);
        }
    }
}

BitSequence::BitSequence(const BitSequence& other) : size(other.size) {
    const size_t byteCount = (size + 7) / 8;
    data = new DynamicArray<uint8_t>(byteCount);

    for (size_t i = 0; i < byteCount; ++i) {
        data->Set(i, other.getByte(i));
    }
}

BitSequence& BitSequence::operator=(const BitSequence& other) {
    if (this != &other) {
        delete data;

        size = other.size;
        const size_t byteCount = (size + 7) / 8;
        data = new DynamicArray<uint8_t>(byteCount);

        for (size_t i = 0; i < byteCount; ++i) {
            data->Set(i, other.getByte(i));
        }
    }
    return *this;
}

size_t BitSequence::GetLength() const {
    return size;
}

bool BitSequence::GetFirst() const {
    if (size == 0) throw std::out_of_range("BitSequence is empty");
    return getBit(0);
}

bool BitSequence::GetLast() const {
    if (size == 0) throw std::out_of_range("BitSequence is empty");
    return getBit(size - 1);
}

BitSequence *BitSequence::GetSubsequence(const size_t startIndex, const size_t endIndex) const {
    if (endIndex > size)
        throw std::out_of_range("endIndex out of bounds");

    const size_t newLen = endIndex - startIndex;
    if (newLen == 0)
        return new BitSequence(new DynamicArray<uint8_t>(0), 0);

    const auto tmp = new bool[newLen];
    for (size_t i = 0; i < newLen; ++i)
        tmp[i] = getBit(startIndex + i);

    auto *res = new BitSequence(tmp, newLen);
    delete[] tmp;
    return res;
}

IEnumerator<bool> *BitSequence::GetEnumerator() const {
    return new BitEnumerator(this);
}

BitSequence *BitSequence::AppendImpl(const bool &elem) {
    ensureIncreaseCapacity();

    const size_t byteIdx = size / 8;
    const size_t bitPos = size % 8;

    uint8_t byteVal = data->Get(byteIdx);
    if (elem) {
        byteVal |= static_cast<uint8_t>(1u << bitPos);
    } else {
        byteVal &= static_cast<uint8_t>(~(1u << bitPos));
    }
    data->Set(byteIdx, byteVal);

    ++size;
    return this;
}

BitSequence *BitSequence::PrependImpl(const bool &elem) {
    if (size == 0) {
        return AppendImpl(elem);
    }

    ensureIncreaseCapacity();

    for (size_t i = size; i > 0; --i) {
        const size_t srcByteIdx = (i - 1) / 8;
        const size_t srcBitPos = (i - 1) % 8;
        const uint8_t srcByte = data->Get(srcByteIdx);
        const bool bit = (srcByte >> srcBitPos) & 1u;

        const size_t dstByteIdx = i / 8;
        const size_t dstBitPos = i % 8;
        uint8_t dstByte = data->Get(dstByteIdx);
        if (bit) {
            dstByte |= static_cast<uint8_t>(1u << dstBitPos);
        } else {
            dstByte &= static_cast<uint8_t>(~(1u << dstBitPos));
        }
        data->Set(dstByteIdx, dstByte);
    }

    uint8_t firstByte = data->Get(0);
    if (elem) {
        firstByte |= 0x01;
    } else {
        firstByte &= ~0x01u;
    }
    data->Set(0, firstByte);

    ++size;
    return this;
}


BitSequence *BitSequence::InsertAtImpl(const bool &elem, const size_t index) {
    if (index > size)
        throw std::out_of_range("BitSequence: insert index out of range");

    if (index == size) return AppendImpl(elem);
    if (index == 0) return PrependImpl(elem);

    ensureIncreaseCapacity();

    const size_t newBitSize = size + 1;

    for (size_t i = size; i > index; --i) {
        const size_t srcByteIdx = (i - 1) / 8;
        const size_t srcBitPos = (i - 1) % 8;
        const uint8_t srcByte = data->Get(srcByteIdx);
        const bool bit = (srcByte >> srcBitPos) & 1u;

        const size_t dstByteIdx = i / 8;
        const size_t dstBitPos = i % 8;
        uint8_t dstByte = data->Get(dstByteIdx);
        if (bit) {
            dstByte |= static_cast<uint8_t>(1u << dstBitPos);
        } else {
            dstByte &= static_cast<uint8_t>(~(1u << dstBitPos));
        }
        data->Set(dstByteIdx, dstByte);
    }

    const size_t bIdx = index / 8;
    const size_t bPos = index % 8;
    uint8_t byteVal = data->Get(bIdx);
    if (elem) {
        byteVal |= static_cast<uint8_t>(1u << bPos);
    } else {
        byteVal &= static_cast<uint8_t>(~(1u << bPos));
    }
    data->Set(bIdx, byteVal);

    size = newBitSize;
    return this;
}

BitSequence *BitSequence::DelImpl(const size_t index) {
    if (index >= size)
        throw std::out_of_range("BitSequence: delete index out of range");

    if (index == size - 1) {
        --size;
        if (size > 0 && size % 8 != 0) {
            const size_t lastByteIdx = size / 8;
            const auto mask = static_cast<uint8_t>((1u << (size % 8)) - 1);
            const uint8_t lastByte = data->Get(lastByteIdx);
            data->Set(lastByteIdx, lastByte & mask);
        }
        ensureReductionCapacity();
        return this;
    }

    for (size_t i = index; i < size - 1; ++i) {
        const size_t srcByteIdx = (i + 1) / 8;
        const size_t srcBitPos = (i + 1) % 8;
        const uint8_t srcByte = data->Get(srcByteIdx);
        const bool bit = (srcByte >> srcBitPos) & 1u;

        const size_t dstByteIdx = i / 8;
        const size_t dstBitPos = i % 8;
        uint8_t dstByte = data->Get(dstByteIdx);
        if (bit) {
            dstByte |= static_cast<uint8_t>(1u << dstBitPos);
        } else {
            dstByte &= static_cast<uint8_t>(~(1u << dstBitPos));
        }
        data->Set(dstByteIdx, dstByte);
    }

    --size;

    if (size > 0 && size % 8 != 0) {
        const size_t lastByteIdx = size / 8;
        const auto mask = static_cast<uint8_t>((1u << (size % 8)) - 1);
        const uint8_t lastByte = data->Get(lastByteIdx);
        data->Set(lastByteIdx, lastByte & mask);
    } else if (size == 0) {
        data->Set(0, 0);
    }

    ensureReductionCapacity();

    return this;
}

BitSequence *BitSequence::ConcatImpl(const Sequence &other) {
    const size_t otherLen = other.GetLength();
    if (otherLen == 0) return this;

    const size_t oldBitSize = size;
    const size_t newBitSize = size + otherLen;

    if ((newBitSize + 7) / 8 > (oldBitSize + 7) / 8) {
        data->Resize((newBitSize + 7) / 8);
    }

    size = newBitSize;

    auto *enumPtr = other.GetEnumerator();
    size_t targetIdx = oldBitSize;
    while (enumPtr->MoveNext()) {
        setBit(targetIdx, enumPtr->Current());
        ++targetIdx;
    }
    delete enumPtr;
    return this;
}

BitSequence& BitSequence::operator&=(const BitSequence& other) {
    if (size != other.size) {
        throw std::invalid_argument("Different sizes");
    }

    const size_t byteCount = (size + 7) / 8;
    for (size_t i = 0; i < byteCount; ++i) {
        const uint8_t byte1 = getByte(i);
        const uint8_t byte2 = other.getByte(i);
        setByte(i, byte1 & byte2);
    }

    return *this;
}

BitSequence& BitSequence::operator|=(const BitSequence& other) {
    if (size != other.size) {
        throw std::invalid_argument("Different sizes");
    }

    const size_t byteCount = (size + 7) / 8;
    for (size_t i = 0; i < byteCount; ++i) {
        const uint8_t byte1 = getByte(i);
        const uint8_t byte2 = other.getByte(i);
        setByte(i, byte1 | byte2);
    }

    return *this;
}

BitSequence& BitSequence::operator^=(const BitSequence& other) {
    if (size != other.size) {
        throw std::invalid_argument("Different sizes");
    }

    const size_t byteCount = (size + 7) / 8;
    for (size_t i = 0; i < byteCount; ++i) {
        const uint8_t byte1 = getByte(i);
        const uint8_t byte2 = other.getByte(i);
        setByte(i, byte1 ^ byte2);
    }

    return *this;
}

BitSequence& BitSequence::operator~() {
    const size_t byteCount = (size + 7) / 8;
    for (size_t i = 0; i < byteCount; ++i) {
        setByte(i, ~getByte(i));
    }

    if (size % 8 != 0) {
        const size_t lastByteIdx = size / 8;
        const uint8_t mask = (1u << (size % 8)) - 1;
        const uint8_t lastByte = getByte(lastByteIdx);
        setByte(lastByteIdx, lastByte & mask);
    }

    return *this;
}

BitSequence operator&(const BitSequence& lhs, const BitSequence& rhs) {
    BitSequence result = lhs;
    result &= rhs;
    return result;
}

BitSequence operator|(const BitSequence& lhs, const BitSequence& rhs) {
    BitSequence result = lhs;
    result |= rhs;
    return result;
}

BitSequence operator^(const BitSequence& lhs, const BitSequence& rhs) {
    BitSequence result = lhs;
    result ^= rhs;
    return result;
}

BitSequence *BitSequence::CreateEmpty() const {
    return new BitSequence(new DynamicArray<uint8_t>(0), 0);
}

BitSequence *BitSequence::Instance() {
    return this;
}

BitSequence::~BitSequence() {
    delete data;
}