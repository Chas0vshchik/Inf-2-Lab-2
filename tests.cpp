#include <gtest/gtest.h>
#include <string>
#include "include/DynamicArray.h"
#include "include/LinkedList.h"
#include "include/IEnumerator.h"
#include "include/MutableArraySequence.h"
#include "include/MutableListSequence.h"
#include "include/ImmutableArraySequence.h"
#include "include/ImmutableListSequence.h"

TEST(DynamicArrayTests, Constructors) {
    int data[] = {1, 2, 3};
    const DynamicArray arr1(data, 3);
    EXPECT_EQ(arr1.GetSize(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr1.Get(i), data[i]);
    }

    EXPECT_THROW(
        DynamicArray<int> arr2(nullptr, 5),
        std::invalid_argument
    );

    const DynamicArray<int> arr3(3);
    EXPECT_EQ(arr3.GetSize(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr3.Get(i), int{});
    }

    const DynamicArray arr4(arr1);
    EXPECT_EQ(arr4.GetSize(), arr1.GetSize());
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr4.Get(i), arr1.Get(i));
    }

    const DynamicArray<int> arr5 = arr1;
    EXPECT_EQ(arr5.GetSize(), arr1.GetSize());
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr5.Get(i), arr1.Get(i));
    }
}

TEST(DynamicArrayTests, Set) {
    int data[] = {1, 2, 3};
    DynamicArray arr1(data, 3);
    EXPECT_EQ(arr1.GetSize(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr1.Get(i), data[i]);
    }

    arr1.Set(1, 0);
    EXPECT_EQ(arr1.Get(1), 0);

    EXPECT_THROW(
        arr1.Set(10, 0),
        std::out_of_range
    );
}

TEST(DynamicArrayTests, Resize) {
    int data[] = {1, 2, 3};
    DynamicArray arr1(data, 3);
    EXPECT_EQ(arr1.GetSize(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr1.Get(i), data[i]);
    }

    arr1.Resize(4);
    EXPECT_EQ(arr1.GetSize(), 4);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr1.Get(i), data[i]);
    }
    EXPECT_EQ(arr1.Get(3), int{});

    arr1.Resize(0);
    EXPECT_EQ(arr1.GetSize(), 0);
}

TEST(DynamicArrayTests, String) {
    std::string data[] = {"1", "2", "3"};
    DynamicArray arr1(data, 3);
    EXPECT_EQ(arr1.GetSize(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr1.Get(i), data[i]);
    }

    arr1.Resize(4);
    EXPECT_EQ(arr1.GetSize(), 4);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(arr1.Get(i), data[i]);
    }
    EXPECT_EQ(arr1.Get(3), std::string{});
}

TEST(LinkedListTests, Constructors) {
    const LinkedList<int> lst1;
    EXPECT_EQ(lst1.GetSize(), 0);

    int data[] = {1, 2, 3};
    const LinkedList lst2(data, 3);
    EXPECT_EQ(lst2.GetSize(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(lst2.Get(i), data[i]);
    }

    const LinkedList lst4(lst2);
    EXPECT_EQ(lst4.GetSize(), lst2.GetSize());
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(lst4.Get(i), lst2.Get(i));
    }

    const LinkedList<int> lst5 = lst2;
    EXPECT_EQ(lst5.GetSize(), lst2.GetSize());
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(lst5.Get(i), lst2.Get(i));
    }
}

TEST(LinkedListTests, GetFirstLast) {
    int data[] = {1, 2, 3};
    const LinkedList lst2(data, 3);
    EXPECT_EQ(lst2.GetSize(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(lst2.Get(i), data[i]);
    }

    EXPECT_EQ(lst2.GetFirst(), 1);
    EXPECT_EQ(lst2.GetLast(), 3);

    const LinkedList<int> lst3;
    EXPECT_THROW(
        lst3.GetFirst(),
        std::out_of_range
    );
    EXPECT_THROW(
        lst3.GetLast(),
        std::out_of_range
    );
}

TEST(LinkedListTests, AddAndDelElems) {
    int data[] = {1, 2, 3};
    LinkedList lst2(data, 3);
    EXPECT_EQ(lst2.GetSize(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(lst2.Get(i), data[i]);
    }

    lst2.Append(4);
    EXPECT_EQ(lst2.GetSize(), 4);
    EXPECT_EQ(lst2.GetLast(), 4);

    lst2.Prepend(0);
    EXPECT_EQ(lst2.GetSize(), 5);
    EXPECT_EQ(lst2.GetFirst(), 0);

    lst2.Del(2);
    EXPECT_EQ(lst2.GetSize(), 4);

    lst2.InsertAt(2, 2);
    EXPECT_EQ(lst2.GetSize(), 5);
    EXPECT_EQ(lst2.Get(2), 2);

    EXPECT_THROW(
        lst2.Get(10),
        std::out_of_range
    );
    EXPECT_THROW(
        lst2.Del(10),
        std::out_of_range
    );
    EXPECT_THROW(
        lst2.InsertAt(10, 0),
        std::out_of_range
    );
}

TEST(LinkedListTests, GetSubList) {
    int data[] = {1, 2, 3, 4};
    const LinkedList lst2(data, 4);
    EXPECT_EQ(lst2.GetSize(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(lst2.Get(i), data[i]);
    }

    const LinkedList<int>* sub = lst2.GetSubList(1, 2);
    EXPECT_EQ(sub->GetSize(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(sub->Get(i), data[i+1]);
    }

    EXPECT_THROW(
        lst2.GetSubList(10, 0),
        std::out_of_range
    );
    EXPECT_THROW(
        lst2.GetSubList(1, 9),
        std::out_of_range
    );
    EXPECT_THROW(
        lst2.GetSubList(9, 10),
        std::out_of_range
    );
}

TEST(LinkedListTests, Concat) {
    int data[] = {1, 2, 3, 4};
    LinkedList lst2(data, 4);
    EXPECT_EQ(lst2.GetSize(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(lst2.Get(i), data[i]);
    }

    lst2 = *lst2.Concat(lst2);
    EXPECT_EQ(lst2.GetSize(), 8);
    for (size_t i = 0; i < 8; ++i) {
        EXPECT_EQ(lst2.Get(i), data[i%4]);
    }
}

int square(const int x) { return x * x; }
bool isEven(const int x) { return x % 2 == 0; }
int sumAcc(const int acc, const int x) { return acc + x; }
int GetAt(const Sequence<int> *seq, const size_t index) {
    const auto en = seq->GetEnumerator();
    for (size_t i = 0; i <= index; ++i) {
        if (!en->MoveNext()) {
            delete en;
            throw std::out_of_range("Index out of range");
        }
    }
    const int val = en->Current();
    delete en;
    return val;
}

TEST(MutableArraySequenceTests, Constructors) {
    const MutableArraySequence<int> seq1;
    EXPECT_EQ(seq1.GetLength(), 0);

    int data[] = {1, 2, 3};
    const MutableArraySequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    const MutableArraySequence seq3(seq2);
    EXPECT_EQ(seq3.GetLength(), seq2.GetLength());
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq3.Get(i), seq2.Get(i));
    }
}

TEST(MutableArraySequenceTests, GetFirstLast) {
    int data[] = {1, 2, 3};
    const MutableArraySequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    EXPECT_EQ(seq2.GetFirst(), 1);
    EXPECT_EQ(seq2.GetLast(), 3);

    const MutableArraySequence<int> seq1;
    EXPECT_EQ(seq1.GetLength(), 0);

    EXPECT_THROW(
        seq1.GetFirst(),
        std::out_of_range
    );
    EXPECT_THROW(
        seq1.GetLast(),
        std::out_of_range
    );
}

TEST(MutableArraySequenceTests, AddAndDelElems) {
    int data[] = {1, 2, 3};
    MutableArraySequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    seq2.Append(4);
    EXPECT_EQ(seq2.GetLength(), 4);
    EXPECT_EQ(seq2.GetLast(), 4);

    seq2.Prepend(0);
    EXPECT_EQ(seq2.GetLength(), 5);
    EXPECT_EQ(seq2.GetFirst(), 0);

    seq2.Del(2);
    EXPECT_EQ(seq2.GetLength(), 4);

    seq2.InsertAt(2, 2);
    EXPECT_EQ(seq2.GetLength(), 5);
    EXPECT_EQ(seq2.Get(2), 2);
}

TEST(MutableArraySequenceTests, Concat) {
    int data[] = {1, 2, 3};
    MutableArraySequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    seq2.Concat(seq2);
    EXPECT_EQ(seq2.GetLength(), 6);
}

TEST(MutableArraySequenceTests, GetSubsequence) {
    int data[] = {1, 2, 3, 4};
    const MutableArraySequence seq2(data, 4);
    EXPECT_EQ(seq2.GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    const auto sub = seq2.GetSubsequence(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(GetAt(sub, i), data[i+1]);
    }
    delete sub;
}

int cast(const double x) { return static_cast<int>(x); }
TEST(MutableArraySequenceTests, Map_double_to_int) {
    // MutableArraySequence<double> arr;
    // arr.Append(3.14);
    // Sequence<int>* arrm = arr.Map(cast);
}

TEST(MutableArraySequenceTests, Map_Reduce) {
    int data[] = {1, 2, 3, 4};
    MutableArraySequence seq2(data, 4);
    EXPECT_EQ(seq2.GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    const auto res1 = seq2.Map(square);
    EXPECT_EQ(res1->GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(GetAt(res1, i), data[i]*data[i]);
    }

    const auto res2 = seq2.Where(isEven);
    EXPECT_EQ(res2->GetLength(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(GetAt(res2, i), data[i*2+1]);
    }

    EXPECT_EQ(seq2.Reduce(sumAcc), 10);

    delete res1;
    delete res2;
}

TEST(MutableListSequenceTests, Constructors) {
    const MutableListSequence<int> seq1;
    EXPECT_EQ(seq1.GetLength(), 0);

    int data[] = {1, 2, 3};
    const MutableListSequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const MutableListSequence seq3(seq2);
    EXPECT_EQ(seq3.GetLength(), seq2.GetLength());
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq3, i), GetAt(&seq2, i));
    }
}

TEST(MutableListSequenceTests, GetFirstLast) {
    int data[] = {1, 2, 3};
    const MutableListSequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    EXPECT_EQ(seq2.GetFirst(), 1);
    EXPECT_EQ(seq2.GetLast(), 3);

    const MutableListSequence<int> seq1;
    EXPECT_EQ(seq1.GetLength(), 0);

    EXPECT_THROW(
        seq1.GetFirst(),
        std::out_of_range
    );
    EXPECT_THROW(
        seq1.GetLast(),
        std::out_of_range
    );
}

TEST(MutableListSequenceTests, AddAndDelElems) {
    int data[] = {1, 2, 3};
    MutableListSequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    seq2.Append(4);
    EXPECT_EQ(seq2.GetLength(), 4);
    EXPECT_EQ(seq2.GetLast(), 4);

    seq2.Prepend(0);
    EXPECT_EQ(seq2.GetLength(), 5);
    EXPECT_EQ(seq2.GetFirst(), 0);

    seq2.Del(2);
    EXPECT_EQ(seq2.GetLength(), 4);

    seq2.InsertAt(2, 2);
    EXPECT_EQ(seq2.GetLength(), 5);
    EXPECT_EQ(GetAt(&seq2, 2), 2);
}

TEST(MutableListSequenceTests, Concat) {
    int data[] = {1, 2, 3};
    MutableListSequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const MutableListSequence seq3(seq2);
    seq2.Concat(seq3);
    EXPECT_EQ(seq2.GetLength(), 6);
}

TEST(MutableListSequenceTests, GetSubsequence) {
    int data[] = {1, 2, 3, 4};
    const MutableListSequence seq2(data, 4);
    EXPECT_EQ(seq2.GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const auto sub = seq2.GetSubsequence(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(GetAt(sub, i), data[i+1]);
    }
    delete sub;
}

TEST(MutableListSequenceTests, Map_Reduce) {
    int data[] = {1, 2, 3, 4};
    MutableListSequence seq2(data, 4);
    EXPECT_EQ(seq2.GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const auto res1 = seq2.Map(square);
    EXPECT_EQ(res1->GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(GetAt(res1, i), data[i]*data[i]);
    }

    const auto res2 = seq2.Where(isEven);
    EXPECT_EQ(res2->GetLength(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(GetAt(res2, i), data[i*2+1]);
    }

    EXPECT_EQ(seq2.Reduce(sumAcc), 10);

    delete res1;
    delete res2;
}

TEST(ImmutableArraySequenceTests, Constructors) {
    const ImmutableArraySequence<int> seq1;
    EXPECT_EQ(seq1.GetLength(), 0);

    int data[] = {1, 2, 3};
    const ImmutableArraySequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    const ImmutableArraySequence seq3(seq2);
    EXPECT_EQ(seq3.GetLength(), seq2.GetLength());
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq3.Get(i), seq2.Get(i));
    }
}

TEST(ImmutableArraySequenceTests, GetFirstLast) {
    int data[] = {1, 2, 3};
    const ImmutableArraySequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    EXPECT_EQ(seq2.GetFirst(), 1);
    EXPECT_EQ(seq2.GetLast(), 3);

    const ImmutableArraySequence<int> seq1;
    EXPECT_EQ(seq1.GetLength(), 0);

    EXPECT_THROW(
        seq1.GetFirst(),
        std::out_of_range
    );
    EXPECT_THROW(
        seq1.GetLast(),
        std::out_of_range
    );
}

TEST(ImmutableArraySequenceTests, AddAndDelElems) {
    int data[] = {1, 2, 3};
    ImmutableArraySequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    const auto res = seq2.Append(4);
    EXPECT_EQ(res->GetLength(), 4);
    EXPECT_EQ(res->GetLast(), 4);

    const auto res1 = seq2.Prepend(0);
    EXPECT_EQ(res1->GetLength(), 4);
    EXPECT_EQ(res1->GetFirst(), 0);

    const auto res2 = seq2.Del(2);
    EXPECT_EQ(res2->GetLength(), 2);

    const auto res3 = seq2.InsertAt(2, 2);
    EXPECT_EQ(res3->GetLength(), 4);
    EXPECT_EQ(GetAt(res3, 2), 2);

    delete res;
    delete res1;
    delete res2;
    delete res3;
}

TEST(ImmutableArraySequenceTests, Concat) {
    int data[] = {1, 2, 3};
    ImmutableArraySequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    const auto res = seq2.Concat(seq2);
    EXPECT_EQ(res->GetLength(), 6);

    delete res;
}

TEST(ImmutableArraySequenceTests, GetSubsequence) {
    int data[] = {1, 2, 3, 4};
    const ImmutableArraySequence seq2(data, 4);
    EXPECT_EQ(seq2.GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    const auto sub = seq2.GetSubsequence(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(GetAt(sub, i), data[i+1]);
    }
    delete sub;
}

TEST(ImmutableArraySequenceTests, Map_Reduce) {
    int data[] = {1, 2, 3, 4};
    ImmutableArraySequence seq2(data, 4);
    EXPECT_EQ(seq2.GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(seq2.Get(i), data[i]);
    }

    const auto res1 = seq2.Map(square);
    EXPECT_EQ(res1->GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(GetAt(res1, i), data[i]*data[i]);
    }

    const auto res2 = seq2.Where(isEven);
    EXPECT_EQ(res2->GetLength(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(GetAt(res2, i), data[i*2+1]);
    }

    EXPECT_EQ(seq2.Reduce(sumAcc), 10);

    delete res1;
    delete res2;
}

TEST(ImmutableListSequenceTests, Constructors) {
    const ImmutableListSequence<int> seq1;
    EXPECT_EQ(seq1.GetLength(), 0);

    int data[] = {1, 2, 3};
    const ImmutableListSequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const ImmutableListSequence seq3(seq2);
    EXPECT_EQ(seq3.GetLength(), seq2.GetLength());
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq3, i), GetAt(&seq2, i));
    }
}

TEST(ImmutableListSequenceTests, GetFirstLast) {
    int data[] = {1, 2, 3};
    const ImmutableListSequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    EXPECT_EQ(seq2.GetFirst(), 1);
    EXPECT_EQ(seq2.GetLast(), 3);

    const ImmutableListSequence<int> seq1;
    EXPECT_EQ(seq1.GetLength(), 0);

    EXPECT_THROW(
        seq1.GetFirst(),
        std::out_of_range
    );
    EXPECT_THROW(
        seq1.GetLast(),
        std::out_of_range
    );
}

TEST(ImmutableListSequenceTests, AddAndDelElems) {
    int data[] = {1, 2, 3};
    ImmutableListSequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const auto res = seq2.Append(4);
    EXPECT_EQ(res->GetLength(), 4);
    EXPECT_EQ(res->GetLast(), 4);

    const auto res1 = seq2.Prepend(0);
    EXPECT_EQ(res1->GetLength(), 4);
    EXPECT_EQ(res1->GetFirst(), 0);

    const auto res2 = seq2.Del(2);
    EXPECT_EQ(res2->GetLength(), 2);

    const auto res3 = seq2.InsertAt(2, 2);
    EXPECT_EQ(res3->GetLength(), 4);
    EXPECT_EQ(GetAt(res3, 2), 2);

    delete res;
    delete res1;
    delete res2;
    delete res3;
}

TEST(ImmutableListSequenceTests, Concat) {
    int data[] = {1, 2, 3};
    ImmutableListSequence seq2(data, 3);
    EXPECT_EQ(seq2.GetLength(), 3);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const ImmutableListSequence seq3(seq2);
    const auto res = seq2.Concat(seq3);
    EXPECT_EQ(res->GetLength(), 6);

    delete res;
}

TEST(ImmutableListSequenceTests, GetSubsequence) {
    int data[] = {1, 2, 3, 4};
    const ImmutableListSequence seq2(data, 4);
    EXPECT_EQ(seq2.GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const auto sub = seq2.GetSubsequence(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(GetAt(sub, i), data[i+1]);
    }
    delete sub;
}

TEST(ImmutableListSequenceTests, Map_Reduce) {
    int data[] = {1, 2, 3, 4};
    ImmutableListSequence seq2(data, 4);
    EXPECT_EQ(seq2.GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(GetAt(&seq2, i), data[i]);
    }

    const auto res1 = seq2.Map(square);
    EXPECT_EQ(res1->GetLength(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(GetAt(res1, i), data[i]*data[i]);
    }

    const auto res2 = seq2.Where(isEven);
    EXPECT_EQ(res2->GetLength(), 2);
    for (size_t i = 0; i < 2; ++i) {
        EXPECT_EQ(GetAt(res2, i), data[i*2+1]);
    }

    EXPECT_EQ(seq2.Reduce(sumAcc), 10);

    delete res1;
    delete res2;
}