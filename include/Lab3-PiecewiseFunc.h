#ifndef INF_2_LAB_2_LAB3_PIECEWISEFUNC_H
#define INF_2_LAB_2_LAB3_PIECEWISEFUNC_H

#include <stdexcept>
#include <cmath>

#include "Lab3-Segment.h"
#include "MutableArraySequence.h"

template <typename T>
class PiecewiseFunc {
public:
    PiecewiseFunc() : segments(new MutableArraySequence<Segment<T>>()) {}

    explicit PiecewiseFunc(const MutableArraySequence<Segment<T>>& segs)
        : segments(new MutableArraySequence<Segment<T>>(*segs)) {}

    PiecewiseFunc(const PiecewiseFunc& other)
        : segments(new MutableArraySequence<Segment<T>>(*other.segments)) {}

    virtual ~PiecewiseFunc() { delete segments; }

    [[nodiscard]] size_t GetSize() const {
        return segments->GetLength();
    }

    Segment<T> GetSegment(size_t index) const {
        if (index >= segments->GetLength()) {
            throw std::out_of_range("index out of range");
        }
        return segments->Get(index);
    }

    PiecewiseFunc* DelSegment(size_t index) {
        segments->Del(index);
        return this;
    }

    friend bool operator==(const PiecewiseFunc& lhs, const PiecewiseFunc& rhs) {
        if (lhs.GetSize() != rhs.GetSize()) {
            return false;
        }
        for (size_t i = 0; i < lhs.GetSize(); ++i) {
            Segment left = lhs.GetSegment(i);
            Segment right = rhs.GetSegment(i);
            if (std::abs(left.start - right.start) > 1e-9 ||
                std::abs(left.end - right.end) > 1e-9) {
                return false;
            }
        }
        return true;
    }
    friend bool operator!=(const PiecewiseFunc& lhs, const PiecewiseFunc& rhs) {
        return !(lhs == rhs);
    }

    [[nodiscard]] T evaluateInFunc(T x) const {
        for (size_t i = 0; i < segments->GetLength(); ++i) {
            Segment seg = segments->Get(i);
            if (x >= seg.start && x <= seg.end) {
                if (i != 0 && x == seg.start ||
                    i != segments->GetLength() - 1 && x == seg.end) {
                    throw std::out_of_range("multiple values at point x");
                }
                return seg.evaluateInSegment(x);
            }
        }
        throw std::out_of_range("x out of function");
    }

    bool isContinuousOnInterval(T start, T end) const {
        size_t current_index = 0;
        while (current_index < segments->GetLength()) {
            if (segments->Get(current_index).end > start) break;
            ++current_index;
        }

        if (current_index == segments->GetLength() ||
            end <= segments->Get(current_index).start ||
            segments->Get(current_index).is_continuous == false &&
            segments->Get(current_index).start == start) {
            return false;
        }
        if (segments->Get(current_index).is_continuous == false &&
            segments->Get(current_index).start != start) {
            throw std::invalid_argument("start_x is in the middle of the discontinuous segment");
        }
        ++current_index;

        while (current_index < segments->GetLength()) {
            if (segments->Get(current_index - 1).end >= end) return true;
            if (std::abs(segments->Get(current_index - 1).end -
                segments->Get(current_index).start) < 1e-9 &&
                std::abs(segments->Get(current_index - 1).evaluateInSegment(segments->Get(current_index - 1).end) -
                segments->Get(current_index).evaluateInSegment(segments->Get(current_index).start)) < 1e-9) {
                if (segments->Get(current_index).is_continuous == false) {
                    if (segments->Get(current_index).end <= end) {
                        return false;
                    }
                    throw std::invalid_argument("end_x is in the middle of discontinuous segment");
                }
                ++current_index;
            } else {
                return false;
            }
        }
        if (segments->GetLast().end >= end) return true;
        return false;
    }

    bool isMonotonicOnInterval(T start, T end) const {
        if (!isContinuousOnInterval(start, end)) return false;

        size_t current_index = 0;
        while (current_index < segments->GetLength()) {
            if (segments->Get(current_index).end > start) break;
            ++current_index;
        }

        if (segments->Get(current_index).is_monotonic == false &&
            segments->Get(current_index).start == start) {
            return false;
        }
        if (segments->Get(current_index).is_monotonic == false &&
            segments->Get(current_index).start != start) {
            throw std::invalid_argument("start_x is in the middle of non-monotonic segment");
        }

        int direction = 0;
        while (current_index < segments->GetLength()) {
            if (segments->Get(current_index).start >= end) return true;
            if (segments->Get(current_index).is_monotonic) {
                if (segments->Get(current_index).evaluateInSegment(segments->Get(current_index).start) <
                segments->Get(current_index).evaluateInSegment(segments->Get(current_index).end)) {
                    if (direction == -1) return false;
                    direction = 1;
                }
                if (segments->Get(current_index).evaluateInSegment(segments->Get(current_index).start) >
                    segments->Get(current_index).evaluateInSegment(segments->Get(current_index).end)) {
                    if (direction == 1) return false;
                    direction = -1;
                }
                ++current_index;
            } else {
                if (segments->Get(current_index).end <= end) {
                    return false;
                }
                throw std::invalid_argument("end_x is in the middle of non-monotonic segment");
            }
        }
        return true;
    }

    IEnumerator<T>* GetEnumerator() const {
        return segments->GetEnumerator();
    }

    PiecewiseFunc* RedefineOnInterval(Segment<T> new_interval) {
        if (segments->GetLength() == 0) {
            segments->Append(new_interval);
            return this;
        }

        int first_del_index = -1;
        int last_del_index = -1;
        int current_index = 0;
        bool is_first_sliced = false;
        bool is_last_sliced = false;

        while (current_index < segments->GetLength()) {
            if (segments->Get(current_index).end > new_interval.start) { //не левее
                if (segments->Get(current_index).start < new_interval.end) { //не правее
                    first_del_index = current_index;
                    if (segments->Get(current_index).start < new_interval.start) {
                        is_first_sliced = true;
                    }
                    break;
                }
                segments->InsertAt(new_interval, current_index);
                return this;
            }
            ++current_index;
        }
        if (current_index == segments->GetLength()) {
            segments->Append(new_interval);
            return this;
        }
        current_index = segments->GetLength() - 1;
        while (current_index >= 0) {
            if (segments->Get(current_index).start < new_interval.end) { //не правее
                if (segments->Get(current_index).end > new_interval.start) { //не левее
                    last_del_index = current_index;
                    if (segments->Get(current_index).end > new_interval.end) {
                        is_last_sliced = true;
                    }
                    break;
                }
            }
            --current_index;
        }
        if (first_del_index == -1 || last_del_index == -1) {
            throw std::out_of_range("beda");
        }
        if (is_last_sliced) {
            segments->InsertAt(Segment<T>(new_interval.end,
                                            segments->Get(last_del_index).end,
                                            segments->Get(last_del_index).func),
                                            last_del_index + 1);
        }
        segments->InsertAt(new_interval, last_del_index + 1);
        if (is_first_sliced) {
            segments->InsertAt(Segment<T>(segments->Get(first_del_index).start,
                                            new_interval.start,
                                            segments->Get(first_del_index).func),
                                            last_del_index + 1);
        }
        for (; first_del_index <= last_del_index; --last_del_index) {
            segments->Del(first_del_index);
        }
        return this;
    }

private:
    MutableArraySequence<Segment<T>>* segments;
};

#endif //INF_2_LAB_2_LAB3_PIECEWISEFUNC_H