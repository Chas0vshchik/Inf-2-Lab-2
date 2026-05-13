#ifndef INF_2_LAB_2_IENUMERATOR_H
#define INF_2_LAB_2_IENUMERATOR_H

template<typename T>
class IEnumerator {
public:
    virtual ~IEnumerator() = default;

    virtual bool MoveNext() = 0;

    virtual T Current() const = 0;

    virtual void Reset() = 0;
};

#endif //INF_2_LAB_2_IENUMERATOR_H