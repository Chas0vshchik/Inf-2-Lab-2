#ifndef INF_2_LAB_2_OPTION_H
#define INF_2_LAB_2_OPTION_H

#include <stdexcept>

template<typename T>
class Option {
public:
    Option() : hasValue(false) {}

    explicit Option(const T &value) : hasValue(true), value(value) {}

    [[nodiscard]] bool HasValue() const { return hasValue; }

    T GetValue() const {
        if (!hasValue) throw std::logic_error("Option has no value");
        return value;
    }

    T GetValueOrDefault(const T &defaultValue = T()) const {
        return hasValue ? value : defaultValue;
    }
private:
    bool hasValue;
    T value;
};

#endif //INF_2_LAB_2_OPTION_H