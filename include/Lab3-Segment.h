#ifndef INF_2_LAB_2_LAB3_SEGMENT_H
#define INF_2_LAB_2_LAB3_SEGMENT_H
#include <functional>

template <typename T>
class Segment {
public:
    T start;
    T end;
    std::function<T(T)> func;
    bool is_continuous = true;
    bool is_monotonic = true;

    Segment() : start(0), end(0) {}

    Segment(const T s, const T e, std::function<T(T)> f)
        : start(s), end(e), func(f) {}

    Segment(const T s, const T e, std::function<T(T)> f, const bool continuous, const bool monotonic)
        : start(s), end(e), func(f), is_continuous(continuous), is_monotonic(monotonic) {}

    [[nodiscard]] T evaluateInSegment(const T x) const {
        return func(x);
    }
};

#endif //INF_2_LAB_2_LAB3_SEGMENT_H