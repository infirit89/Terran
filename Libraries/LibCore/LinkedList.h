#pragma once

namespace Terran {
namespace Core {

template<typename T>
class LinkedList {
public:
    using reference = T&;
    using const_reference = T&;
    struct Node {
        T data;
        Node* next;
    };



private:
    Node* m_head;
};

}
}
