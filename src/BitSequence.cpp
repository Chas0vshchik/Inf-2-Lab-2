#include "../include//BitSequence.h"
#include "../include/DynamicArray.h"
#include "../include/Sequence.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>

void BitSequence::ensureReductionCapacity() const {
    const size_t currentCap = bytes->GetSize();
    if (bitSize / 8 > currentCap) return;
    const size_t newCap = std::max(static_cast<size_t>(1), static_cast<size_t>(currentCap) - 1);
    bytes->Resize(newCap);
}

void BitSequence::ensureIncreaseCapacity() const {
    const size_t currentCap = bytes->GetSize();
    if (bitSize / 8 < currentCap) return;
    const size_t newCap = currentCap + 1;
    bytes->Resize(newCap);
}

uint8_t BitSequence::getByte(const size_t byteIdx) const {
    return bytes->Get(byteIdx);
}

void BitSequence::setByte(const size_t byteIdx, const uint8_t val) const {
    bytes->Set(byteIdx, val);
}

bool BitSequence::getBit(const size_t bitIdx) const {
    if (bitIdx >= bitSize)
        throw std::out_of_range("bit index out of range");
    const size_t byteIdx = bitIdx / 8;
    const size_t bitPos = bitIdx % 8;
    return (getByte(byteIdx) >> bitPos) & 1;
}

BitSequence *BitSequence::setBit(const size_t bitIdx, const bool value) {
    if (bitIdx >= bitSize)
        throw std::out_of_range("bit index out of range");

    const size_t byteIdx = bitIdx / 8;
    const size_t bitPos = bitIdx % 8;
    uint8_t byteVal = bytes->Get(byteIdx);

    if (value) {
        byteVal |= static_cast<uint8_t>(1u << bitPos);
    } else {
        byteVal &= static_cast<uint8_t>(~(1u << bitPos));
    }

    bytes->Set(byteIdx, byteVal);
    return this;
}

BitSequence::BitSequence(DynamicArray<uint8_t> *b, const size_t bits)
    : bytes(b), bitSize(bits) {
}

BitSequence::BitSequence(const bool *bits, const size_t numBits) : bitSize(numBits) {
    if (!bits || numBits <= 0)
        throw std::invalid_argument("bits is nullptr");

    const size_t byteCount = (numBits + 7) / 8;
    bytes = new DynamicArray<uint8_t>(byteCount);

    for (size_t i = 0; i < numBits; ++i) {
        if (bits[i]) {
            const size_t bIdx = i / 8;
            const size_t bPos = i % 8;
            uint8_t val = bytes->Get(bIdx);
            val |= static_cast<uint8_t>(1u << bPos);
            bytes->Set(bIdx, val);
        }
    }
}

BitSequence::BitSequence(const char *bitStr) {
    if (!bitStr)
        throw std::invalid_argument("bitStr is nullptr");

    bitSize = std::strlen(bitStr);
    for (size_t i = 0; i < bitSize; ++i) {
        if (bitStr[i] != '0' && bitStr[i] != '1')
            throw std::invalid_argument("invalid character");
    }

    const size_t byteCount = (bitSize + 7) / 8;
    bytes = new DynamicArray<uint8_t>(byteCount);

    for (size_t i = 0; i < bitSize; ++i) {
        if (bitStr[i] == '1') {
            const size_t bIdx = i / 8;
            const size_t bPos = i % 8;
            uint8_t val = bytes->Get(bIdx);
            val |= static_cast<uint8_t>(1u << bPos);
            bytes->Set(bIdx, val);
        }
    }
}

size_t BitSequence::GetLength() const {
    return bitSize;
}

bool BitSequence::GetFirst() const {
    if (bitSize == 0) throw std::out_of_range("BitSequence is empty");
    return getBit(0);
}

bool BitSequence::GetLast() const {
    if (bitSize == 0) throw std::out_of_range("BitSequence is empty");
    return getBit(bitSize - 1);
}

BitSequence *BitSequence::GetSubsequence(const size_t startIndex, const size_t endIndex) const {
    if (endIndex > bitSize)
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

    const size_t byteIdx = bitSize / 8;
    const size_t bitPos = bitSize % 8;

    uint8_t byteVal = bytes->Get(byteIdx);
    if (elem) {
        byteVal |= static_cast<uint8_t>(1u << bitPos);
    } else {
        byteVal &= static_cast<uint8_t>(~(1u << bitPos));
    }
    bytes->Set(byteIdx, byteVal);

    ++bitSize;
    return this;
}

BitSequence *BitSequence::PrependImpl(const bool &elem) {
    if (bitSize == 0) {
        return AppendImpl(elem);
    }

    ensureIncreaseCapacity();

    for (size_t i = bitSize; i > 0; --i) {
        const size_t srcByteIdx = (i - 1) / 8;
        const size_t srcBitPos = (i - 1) % 8;
        const uint8_t srcByte = bytes->Get(srcByteIdx);
        const bool bit = (srcByte >> srcBitPos) & 1u;

        const size_t dstByteIdx = i / 8;
        const size_t dstBitPos = i % 8;
        uint8_t dstByte = bytes->Get(dstByteIdx);
        if (bit) {
            dstByte |= static_cast<uint8_t>(1u << dstBitPos);
        } else {
            dstByte &= static_cast<uint8_t>(~(1u << dstBitPos));
        }
        bytes->Set(dstByteIdx, dstByte);
    }

    uint8_t firstByte = bytes->Get(0);
    if (elem) {
        firstByte |= 0x01;
    } else {
        firstByte &= ~0x01u;
    }
    bytes->Set(0, firstByte);

    ++bitSize;
    return this;
}


BitSequence *BitSequence::InsertAtImpl(const bool &elem, const size_t index) {
    if (index > bitSize)
        throw std::out_of_range("BitSequence: insert index out of range");

    if (index == bitSize) return AppendImpl(elem);
    if (index == 0) return PrependImpl(elem);

    ensureIncreaseCapacity();

    const size_t newBitSize = bitSize + 1;

    for (size_t i = bitSize; i > index; --i) {
        const size_t srcByteIdx = (i - 1) / 8;
        const size_t srcBitPos = (i - 1) % 8;
        const uint8_t srcByte = bytes->Get(srcByteIdx);
        const bool bit = (srcByte >> srcBitPos) & 1u;

        const size_t dstByteIdx = i / 8;
        const size_t dstBitPos = i % 8;
        uint8_t dstByte = bytes->Get(dstByteIdx);
        if (bit) {
            dstByte |= static_cast<uint8_t>(1u << dstBitPos);
        } else {
            dstByte &= static_cast<uint8_t>(~(1u << dstBitPos));
        }
        bytes->Set(dstByteIdx, dstByte);
    }

    const size_t bIdx = index / 8;
    const size_t bPos = index % 8;
    uint8_t byteVal = bytes->Get(bIdx);
    if (elem) {
        byteVal |= static_cast<uint8_t>(1u << bPos);
    } else {
        byteVal &= static_cast<uint8_t>(~(1u << bPos));
    }
    bytes->Set(bIdx, byteVal);

    bitSize = newBitSize;
    return this;
}

BitSequence *BitSequence::DelImpl(const size_t index) {
    if (index >= bitSize)
        throw std::out_of_range("BitSequence: delete index out of range");

    if (index == bitSize - 1) {
        --bitSize;
        if (bitSize > 0 && bitSize % 8 != 0) {
            const size_t lastByteIdx = bitSize / 8;
            const auto mask = static_cast<uint8_t>((1u << (bitSize % 8)) - 1);
            const uint8_t lastByte = bytes->Get(lastByteIdx);
            bytes->Set(lastByteIdx, lastByte & mask);
        }
        ensureReductionCapacity();
        return this;
    }

    for (size_t i = index; i < bitSize - 1; ++i) {
        const size_t srcByteIdx = (i + 1) / 8;
        const size_t srcBitPos = (i + 1) % 8;
        const uint8_t srcByte = bytes->Get(srcByteIdx);
        const bool bit = (srcByte >> srcBitPos) & 1u;

        const size_t dstByteIdx = i / 8;
        const size_t dstBitPos = i % 8;
        uint8_t dstByte = bytes->Get(dstByteIdx);
        if (bit) {
            dstByte |= static_cast<uint8_t>(1u << dstBitPos);
        } else {
            dstByte &= static_cast<uint8_t>(~(1u << dstBitPos));
        }
        bytes->Set(dstByteIdx, dstByte);
    }

    --bitSize;

    if (bitSize > 0 && bitSize % 8 != 0) {
        const size_t lastByteIdx = bitSize / 8;
        const auto mask = static_cast<uint8_t>((1u << (bitSize % 8)) - 1);
        const uint8_t lastByte = bytes->Get(lastByteIdx);
        bytes->Set(lastByteIdx, lastByte & mask);
    } else if (bitSize == 0) {
        bytes->Set(0, 0);
    }

    ensureReductionCapacity();

    return this;
}

BitSequence *BitSequence::ConcatImpl(const Sequence &other) {
    const size_t otherLen = other.GetLength();
    if (otherLen == 0) return this;

    const size_t oldBitSize = bitSize;
    const size_t newBitSize = bitSize + otherLen;

    if ((newBitSize + 7) / 8 > (oldBitSize + 7) / 8) {
        bytes->Resize((newBitSize + 7) / 8);
    }

    bitSize = newBitSize;

    auto *enumPtr = other.GetEnumerator();
    size_t targetIdx = oldBitSize;
    while (enumPtr->MoveNext()) {
        setBit(targetIdx, enumPtr->Current());
        ++targetIdx;
    }
    delete enumPtr;
    return this;
}

BitSequence *BitSequence::And(const BitSequence &other) const {
    const size_t len = std::min(bitSize, other.bitSize);

    if (len == 0) {
        return new BitSequence(new DynamicArray<uint8_t>(0), 0);
    }

    const size_t byteCount = (len + 7) / 8;
    auto *newBytes = new DynamicArray<uint8_t>(byteCount);

    for (size_t i = 0; i < byteCount; ++i) {
        uint8_t resultByte = bytes->Get(i) & other.bytes->Get(i);
        newBytes->Set(i, resultByte);
    }

    if (len % 8 != 0) {
        const size_t lastByteIdx = len / 8;
        const auto mask = static_cast<uint8_t>((1u << (len % 8)) - 1);
        const uint8_t lastByte = newBytes->Get(lastByteIdx);
        newBytes->Set(lastByteIdx, lastByte & mask);
    }

    return new BitSequence(newBytes, len);
}

BitSequence *BitSequence::Or(const BitSequence &other) const {
    const size_t maxLen = std::max(bitSize, other.bitSize);

    if (maxLen == 0) {
        return new BitSequence(new DynamicArray<uint8_t>(0), 0);
    }

    const size_t byteCount = (maxLen + 7) / 8;
    auto *newBytes = new DynamicArray<uint8_t>(byteCount);

    const size_t thisByteCount = (bitSize + 7) / 8;
    const size_t otherByteCount = (other.bitSize + 7) / 8;

    for (size_t i = 0; i < byteCount; ++i) {
        const uint8_t a = (i < thisByteCount) ? bytes->Get(i) : 0;
        const uint8_t b = (i < otherByteCount) ? other.bytes->Get(i) : 0;
        newBytes->Set(i, a | b);
    }

    if (maxLen % 8 != 0) {
        const size_t lastByteIdx = maxLen / 8;
        const auto mask = static_cast<uint8_t>((1u << (maxLen % 8)) - 1);
        const uint8_t lastByte = newBytes->Get(lastByteIdx);
        newBytes->Set(lastByteIdx, lastByte & mask);
    }

    return new BitSequence(newBytes, maxLen);
}

BitSequence *BitSequence::Xor(const BitSequence &other) const {
    const size_t maxLen = std::max(bitSize, other.bitSize);

    if (maxLen == 0) {
        return new BitSequence(new DynamicArray<uint8_t>(0), 0);
    }

    const size_t byteCount = (maxLen + 7) / 8;
    auto *newBytes = new DynamicArray<uint8_t>(byteCount);

    const size_t thisByteCount = (bitSize + 7) / 8;
    const size_t otherByteCount = (other.bitSize + 7) / 8;

    for (size_t i = 0; i < byteCount; ++i) {
        const uint8_t a = (i < thisByteCount) ? bytes->Get(i) : 0;
        const uint8_t b = (i < otherByteCount) ? other.bytes->Get(i) : 0;
        newBytes->Set(i, a ^ b);
    }

    if (maxLen % 8 != 0) {
        const size_t lastByteIdx = maxLen / 8;
        const auto mask = static_cast<uint8_t>((1u << (maxLen % 8)) - 1);
        const uint8_t lastByte = newBytes->Get(lastByteIdx);
        newBytes->Set(lastByteIdx, lastByte & mask);
    }

    return new BitSequence(newBytes, maxLen);
}

BitSequence *BitSequence::Not() const {
    if (bitSize == 0) {
        return new BitSequence(new DynamicArray<uint8_t>(0), 0);
    }

    const size_t byteCount = (bitSize + 7) / 8;
    auto *newBytes = new DynamicArray<uint8_t>(byteCount);

    for (size_t i = 0; i < byteCount; ++i) {
        auto inverted = static_cast<uint8_t>(~bytes->Get(i));
        newBytes->Set(i, inverted);
    }

    if (bitSize % 8 != 0) {
        const size_t lastByteIdx = bitSize / 8;
        const auto mask = static_cast<uint8_t>((1u << (bitSize % 8)) - 1);
        const uint8_t lastByte = newBytes->Get(lastByteIdx);
        newBytes->Set(lastByteIdx, lastByte & mask);
    }

    return new BitSequence(newBytes, bitSize);
}


BitSequence *BitSequence::CreateEmpty() const {
    return new BitSequence(new DynamicArray<uint8_t>(0), 0);
}

BitSequence *BitSequence::Instance() {
    return this;
}

BitSequence::~BitSequence() {
    delete bytes;
}

std::string BitSequence::toDecimalString() const { //TODO что это
    if (bitSize == 0) return "0";

    std::string result = "0";


    for (size_t i = bitSize; i-- > 0;) {
        int carry = 0;
        for (size_t j = result.size(); j-- > 0;) {
            int digit = result[j] - '0';
            digit = digit * 2 + carry;
            carry = digit / 10;
            result[j] = static_cast<char>((digit % 10) + '0');
        }
        if (carry > 0) {
            result.insert(result.begin(), static_cast<char>(carry + '0'));
        }

        if (getBit(i)) {
            carry = 1;
            for (size_t j = result.size(); j-- > 0 && carry > 0;) {
                int digit = result[j] - '0' + carry;
                carry = digit / 10;
                result[j] = static_cast<char>((digit % 10) + '0');
            }
            if (carry > 0) {
                result.insert(result.begin(), '1');
            }
        }
    }

    return result;
}