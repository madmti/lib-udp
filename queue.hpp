template<typename T>
class Queue {
private:
    struct Node {
        Node* next = nullptr;
        T val;
        Node() {};
        Node(T _val) { val = _val; };
    };
    Node* head = nullptr;
    Node* tail = nullptr;
    int n = 0;
public:
    Queue();
    ~Queue();

    void clear();
    int size();

    void push(T val);

    bool pop(T& val);
    bool pop();
};

template class Queue<char>;
template class Queue<char*>;
template class Queue<const char*>;
template class Queue<int>;
template class Queue<int*>;
template class Queue<bool>;
