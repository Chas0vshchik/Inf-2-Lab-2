#ifndef INF_2_LAB_2_LINKEDLIST_H
#define INF_2_LAB_2_LINKEDLIST_H

#include <stdexcept>
#include <utility>
#include "IEnumerator.h"

template<typename T>
class LinkedList {
public:
    void swap(LinkedList& other) noexcept {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(size, other.size);
    }

    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    LinkedList(const T* items, const size_t count) : LinkedList() {
        if (count <= 0) {
            return;
        }
        head = new Node(items[0]);
        tail = head;
        for (size_t i = 1; i < count; i++) {
            tail->next = new Node(items[i]);
            tail = tail->next;
        }
        tail->next = nullptr;
        size = count;
    }

    LinkedList(const LinkedList& other) : LinkedList() {
        if (other.size == 0) {
            return;
        }
        head = new Node(other.head->data);
        tail = head;
        Node* current = other.head->next;
        while (current != nullptr) {
            tail->next = new Node(current->data);
            tail = tail->next;
            current = current->next;
        }
        size = other.size;
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            LinkedList temp(other);
            swap(temp);
        }
        return *this;
    }

    ~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    T GetFirst() const {
        if (size == 0) {
            throw std::out_of_range("list is empty");
        }
        return head->data;
    }

    T GetLast() const {
        if (size == 0) {
            throw std::out_of_range("list is empty");
        }
        return tail->data;
    }

    T Get(const size_t index) const {
        if (index >= size) {
            throw std::out_of_range("IndexOutOfRange");
        }
        Node* current = head;
        for (size_t i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    [[nodiscard]] size_t GetSize() const {
        return size;
    }

    void Del(const size_t index) {
        if (index >= size) {
            throw std::out_of_range("IndexOutOfRange");
        }
        if (size == 1) {
            delete head;
            head = nullptr;
            tail = nullptr;
            size = 0;
            return;
        }
        if (index == 0) {
            const Node* current = head;
            head = head->next;
            delete current;
            size--;
            return;
        }
        Node* current = head;
        for (size_t i = 0; i < index - 1; i++) {
            current = current->next;
        }
        Node* temp = current->next;
        current->next = temp->next;
        delete temp;
        if (index == size - 1) {
            tail = current;
        }
        size--;
    }

    IEnumerator<T>* GetEnumerator() const {
        return new LinkedListEnumerator(this);
    }

    void Append(const T& item) {
        Node* newNode = new Node(item);
        if (size == 0) {
            head = newNode;
            tail = newNode;
            size = 1;
            return;
        }
        tail->next = newNode;
        tail = tail->next;
        size++;
    }

    void Prepend(const T& item) {
        Node* newNode = new Node(item);
        if (size == 0) {
            head = newNode;
            tail = newNode;
            size = 1;
            return;
        }
        newNode->next = head;
        head = newNode;
        size++;
    }

    void InsertAt(const size_t index, const T& item) {
        if (index > size) {
            throw std::out_of_range("IndexOutOfRange");
        }
        if (index == 0) {
            this->Prepend(item);
            return;
        }
        if (index == size) {
            this->Append(item);
            return;
        }
        Node* newNode = new Node(item);
        Node* prev = head;
        for (size_t i = 0; i < index - 1; i++) {
            prev = prev->next;
        }
        newNode->next = prev->next;
        prev->next = newNode;
        size++;
    }

    LinkedList *GetSubList(const size_t startIndex, const size_t endIndex) const {
        if (startIndex >= size || endIndex >= size || startIndex > endIndex) {
            throw std::out_of_range("IndexOutOfRange");
        }
        Node* current = head;
        for (size_t i = 0; i < startIndex; i++) {
            current = current->next;
        }
        auto newList = new LinkedList();
        for (size_t i = startIndex; i <= endIndex; i++) {
            newList->Append(current->data);
            current = current->next;
        }
        return newList;
    }

    LinkedList *Concat(const LinkedList &list){
        if (list.size == 0) {
            return this;
        }
        auto newList = new LinkedList(*this);
        Node* current = list.head;
        for (size_t i = 0; i < list.size; i++) {
            newList->Append(current->data);
            current = current->next;
        }
        return newList;
    }
private:
    struct Node {
        Node* next;
        T data;

        Node() : next(nullptr), data(T()) {}
        explicit Node(const T& data) : next(nullptr), data(data) {}
        ~Node() = default;
    };

    class LinkedListEnumerator : public IEnumerator<T> {
    public:
        explicit LinkedListEnumerator(const LinkedList *lst) : list(lst), current(nullptr) {}

        bool MoveNext() override {
            if (current == nullptr)
                current = list->head;
            else
                current = current->next;
            return current != nullptr;
        }

        T Current() const override {
            if (current == nullptr) throw std::out_of_range("Enumerator out of range");
            return current->data;
        }

        void Reset() override { current = nullptr; }
    private:
        const LinkedList *list;
        Node *current;
    };

    Node* head;
    Node* tail;
    size_t size;
};

#endif //INF_2_LAB_2_LINKEDLIST_H