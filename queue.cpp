#include "queue.hpp"

template<typename T>
Queue<T>::Queue() {};

template<typename T>
Queue<T>::~Queue() {
    clear();
};

template<typename T>
void Queue<T>::clear() {
    Node* temp = head;
    Node* ante;
    while (temp != nullptr) {
        ante = temp;
        temp = temp->next;
        delete ante;
    };
    head = nullptr;
    tail = nullptr;
};
template<typename T>
int Queue<T>::size() {
    return n;
};

template<typename T>
void Queue<T>::push(T val) {
    if (n == 0) head = tail = new Node(val);
    else {
        tail->next = new Node(val);
        tail = tail->next;
    };
    n++;
};

template<typename T>
bool Queue<T>::pop(T& val) {
    if (n == 0) return false;
    Node* temp = head;
    head = head->next;
    val = temp->val;
    delete temp;
    n--;
    return true;
};
template<typename T>
bool Queue<T>::pop() {
    if (n == 0) return false;
    Node* temp = head;
    head = head->next;
    delete temp;
    n--;
    return true;
};