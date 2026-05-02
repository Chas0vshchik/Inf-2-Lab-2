#include "include/UI.h"
#include "include/IEnumerator.h"
#include "include/Option.h"
#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include "include/ImmutableArraySequence.h"
#include "include/ImmutableListSequence.h"
#include "include/MutableArraySequence.h"
#include "include/MutableListSequence.h"

using namespace std;

void printSequence(const Sequence<DataType> *seq) {
    if (!seq) {
        cout << "No sequence" << endl;
        return;
    }
    cout << "Sequence: {";
    const auto enumerator = seq->GetEnumerator();
    while (enumerator->MoveNext()) {
        cout << enumerator->Current() << " ";
    }
    cout << "}" << endl;
}

void printMenu() {
    cout << "\n=== UI menu ===" << endl;
    cout << "1. Print sequence" << endl;
    cout << "2. Append elem" << endl;
    cout << "3. Prepend elem" << endl;
    cout << "4. Insert elem" << endl;
    cout << "5. Delete elem" << endl;
    cout << "6. Get first and last elems" << endl;
    cout << "7. GetSubsequence" << endl;
    cout << "8. Concat" << endl;
    cout << "9. Map" << endl;
    cout << "10. Where" << endl;
    cout << "11. Reduce" << endl;
    cout << "12. Get first and last (...) elems " << endl;
    cout << "13. Create new sequence" << endl;
    cout << "0. Exit" << endl;
    cout << "Your choice: ";
}

int inputInt(const string &prompt) {
    int val;
    cout << prompt;
    cin >> val;
    while (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Incorrect input. ";
        cout << prompt;
        cin >> val;
    }
    return val;
}

size_t inputSize(const string &prompt) {
    const int val = inputInt(prompt);
    return static_cast<size_t>(val);
}

vector<DataType> inputArray() {
    const size_t n = inputSize("Input size: ");
    vector<DataType> arr;
    cout << "Input " << n << " elems: ";
    for (size_t i = 0; i < n; ++i) {
        DataType x;
        cin >> x;
        arr.push_back(x);
    }
    return arr;
}

Sequence<DataType> *createSequence() {
    cout << "\nChoice type of sequence:" << endl;
    cout << "1. MutableArraySequence" << endl;
    cout << "2. ImmutableArraySequence" << endl;
    cout << "3. MutableListSequence" << endl;
    cout << "4. ImmutableListSequence" << endl;
    const int choice = inputInt("Your choice: ");

    const vector<DataType> items = inputArray();
    const DataType *arr = items.data();
    const size_t cnt = items.size();

    switch (choice) {
        case 1: return new MutableArraySequence(arr, cnt);
        case 2: return new ImmutableArraySequence(arr, cnt);
        case 3: return new MutableListSequence(arr, cnt);
        case 4: return new ImmutableListSequence(arr, cnt);
        default:
            cout << "Incorrect choice" << endl;
            return nullptr;
    }
}

DataType square(const DataType x) { return x * x; }
DataType addOne(const DataType x) { return x + 1; }
bool isEven(const DataType x) { return x % 2 == 0; }
bool isPositive(const DataType x) { return x > 0; }
DataType sum(const DataType a, const DataType b) { return a + b; }
DataType product(const DataType a, const DataType b) { return a * b; }

void UI() {
    Sequence<DataType>* seq = nullptr;
    while (!seq) {
        cout << "Create first sequence: ";
        seq = createSequence();
    }
    printSequence(seq);

    int choice;
    do {
        printMenu();
        choice = inputInt("");
        switch (choice) {
            case 1:
                printSequence(seq);
                break;
            case 2: {
                DataType val;
                cout << "Input value: ";
                cin >> val;
                if (Sequence<DataType> *newSeq = seq->Append(val); newSeq != seq) {
                    delete seq;
                    seq = newSeq;
                    cout << "New sequence created (immutable)" << endl;
                } else {
                    cout << "Item added (mutable)" << endl;
                }
                printSequence(seq);
                break;
            }
            case 3: {
                DataType val;
                cout << "Input value: ";
                cin >> val;
                if (Sequence<DataType> *newSeq = seq->Prepend(val); newSeq != seq) {
                    delete seq;
                    seq = newSeq;
                    cout << "New sequence created (immutable)" << endl;
                } else {
                    cout << "Item added (mutable)" << endl;
                }
                printSequence(seq);
                break;
            }
            case 4: {
                DataType val;
                cout << "Input value: ";
                cin >> val;
                const size_t idx = inputSize("Input index: ");
                try {
                    if (Sequence<DataType> *newSeq = seq->InsertAt(val, idx); newSeq != seq) {
                        delete seq;
                        seq = newSeq;
                        cout << "New sequence created (immutable)" << endl;
                    } else {
                        cout << "Item added (mutable)" << endl;
                    }
                    printSequence(seq);
                } catch (const exception &e) {
                    cout << "Exception: " << e.what() << endl;
                }
                break;
            }
            case 5: {
                const size_t idx = inputSize("Input index: ");
                try {
                    if (Sequence<DataType> *newSeq = seq->Del(idx); newSeq != seq) {
                        delete seq;
                        seq = newSeq;
                        cout << "New sequence created (immutable)" << endl;
                    } else {
                        cout << "Item added (mutable)" << endl;
                    }
                    printSequence(seq);
                } catch (const exception &e) {
                    cout << "Exception: " << e.what() << endl;
                }
                break;
            }
            case 6: {
                try {
                    cout << "GetFirst = " << seq->GetFirst() << endl;
                    cout << "GetLast = " << seq->GetLast() << endl;
                } catch (const exception &e) {
                    cout << "Exception: " << e.what() << endl;
                }
                break;
            }
            case 7: {
                const size_t start = inputSize("Input start index: ");
                const size_t end = inputSize("Input end index: ");
                try {
                    const Sequence<DataType> *sub = seq->GetSubsequence(start, end);
                    printSequence(sub);
                    delete sub;
                } catch (const exception &e) {
                    cout << "Exception: " << e.what() << endl;
                }
                break;
            }
            case 8: {
                cout << "Create second sequence for concat:" << endl;
                const Sequence<DataType> *other = createSequence();
                if (!other) break;
                if (Sequence<DataType> *newSeq = seq->Concat(*other); newSeq != seq) {
                    delete seq;
                    seq = newSeq;
                    cout << "New sequence created (immutable)." << endl;
                } else {
                    cout << "Item added (mutable)." << endl;
                }
                printSequence(seq);
                delete other;
                break;
            }
            case 9: {
                cout << "Choice func:" << endl;
                cout << "1. square(x) = x*x" << endl;
                cout << "2. addOne(x) = x+1" << endl;
                const int funcChoice = inputInt("Your choice: ");
                const Sequence<DataType> *mapped = nullptr;
                if (funcChoice == 1)
                    mapped = seq->Map(square);
                else
                    mapped = seq->Map(addOne);
                cout << "Result Map: ";
                printSequence(mapped);
                delete mapped;
                break;
            }
            case 10: {
                cout << "Choice condition:" << endl;
                cout << "1. isEven" << endl;
                cout << "2. isPositive" << endl;
                const int choice2 = inputInt("Your choice: ");
                const Sequence<DataType> *filtered = nullptr;
                if (choice2 == 1)
                    filtered = seq->Where(isEven);
                else
                    filtered = seq->Where(isPositive);
                cout << "Result Where: ";
                printSequence(filtered);
                delete filtered;
                break;
            }
            case 11: {
                cout << "Choice reduce function:" << endl;
                cout << "1. Sum" << endl;
                cout << "2. Product" << endl;
                const int choice2 = inputInt("Your choice: ");
                try {
                    DataType result;
                    if (choice2 == 1)
                        result = seq->Reduce(sum);
                    else
                        result = seq->Reduce(product);
                    cout << "Result Reduce: " << result << endl;
                } catch (const exception &e) {
                    cout << "Exception: " << e.what() << endl;
                }
                break;
            }
            case 12: {
                cout << "Choice condition:" << endl;
                cout << "1. isEven" << endl;
                cout << "2. isPositive" << endl;
                const int choice2 = inputInt("Your choice: ");
                bool (*pred)(DataType) = (choice2 == 1) ? isEven : isPositive;
                Option<DataType> first = seq->GetFirst(*pred);
                Option<DataType> last = seq->GetLast(*pred);
                const string strFunc = (choice2 == 1) ? "even" : "positive";
                cout << "First " << strFunc << " elem: ";
                if (first.HasValue()) cout << first.GetValue() << endl;
                else cout << "not found" << endl;
                cout << "Last " << strFunc << " elem: ";
                if (last.HasValue()) cout << last.GetValue() << endl;
                else cout << "not found" << endl;
                break;
            }
            case 13: {
                if (Sequence<DataType> *newSeq = createSequence()) {
                    delete seq;
                    seq = newSeq;
                    cout << "New sequence created" << endl;
                    printSequence(seq);
                }
                break;
            }
            case 0:
                break;
            default:
                cout << "Incorrect choice" << endl;
        }
    } while (choice != 0);
}

int main() {
    UI();
    return 0;
}