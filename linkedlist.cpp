#include <iostream>
#include <vector>
#include <cstdio>
#include <random>

template <typename T>
class node {
public:
    inline static node* head {nullptr}; //c++17
    inline static node* previous_node {nullptr}; //c++17
    T val;
    node *next;
    // ctor (used for creating list)
    node(T arg) : val {arg}, next {nullptr} {
         // if head -> set and don't try to set previous->next
        if (head == nullptr) head = this;
        else {
            [[maybe_unused]] node<int>* prev = previous_node;
            previous_node->next = this;
        }
        previous_node = this;
    }
    // ctor (used for getting head of list)
    node () : val {}, next {nullptr} {}
};

node<int>* CreateSingleLL(std::vector<int> *vec){
    for (const auto val : *vec){
        [[maybe_unused]] node<int> *nn = new node<int> {val};
    }
    // return head
    node<int> t {};
    return t.head;
}

template <typename T>
void ReadListForward(node<T> *head){
    node<T> *current_node = head;
    while (current_node != nullptr){
        std::cout << current_node->val << ' ';
        current_node = current_node->next;
    }
    std::cout << '\n';
}

template <typename T>
bool DeleteLL(node<T> *head){
    node<T> *free_me {head}, *next {head->next};
    while (1){
        delete free_me; // delete current
        free_me = next; // set it to next
        if (free_me != nullptr) next = free_me->next; // set next to next of current
        else break;
    }

    // check to ensure all nodes were dealloc'd
    return (free_me == nullptr) ? true : false;
}

// not used currently => need to better understand how the c++ random STL functions
int RandomLLElmentGenerator(int imin, int imax){
    static std::random_device rd; 
    std::minstd_rand g(rd()); 
    static std::uniform_int_distribution<> ld(imin, imax);

    // return pair
    return ld(g);
}

template <typename T>
node<T>* RemoveNode(node<T> *head, int val_to_remove){
    // iterate through list and find node that == val
    node<T> *current {head}, *previous {nullptr}, *removed_node {nullptr};

    while (current != nullptr){
        if (current->val == val_to_remove){ // if current node needs to be removed...
        // most likely -> node is NOT head node
            if (current != current->head){
                previous->next = current->next; 
                previous = current;
                removed_node = current;
                 // ensure node to-be-deleted is not null => free if not
                if (removed_node != nullptr) delete removed_node;
                else std::clog << "Could not free single deleted node [heap allocated].\n";
                current = previous->next; // go to next node
                break; // assume vales are unique in array
            } else {
        // least likely -> node IS head node
        // previous in this case will act as temp storage for head->next ptr
                previous = current->next;
                delete current;
                current = previous;
                current->head = current; // set new head ptr
                previous = nullptr;
            }
        } else {
            previous = current;
            current = current->next;
        }
    }
    return node<T> {}.head; // return new head node
}

template <typename T>
node<T>* ReverseLL(node<T> *current_node){
    // base case
    if (current_node->next == nullptr){
        current_node->head = current_node; // establish this as new head
        return current_node; // will be set as next node -> previous
    }

    // recursive => pass in next node : will return new previous node,
        // which you set that to point to this;
    node<T> *next_node {current_node->next};
    next_node = ReverseLL(next_node);
    next_node->next = current_node;
    return current_node;
}

template <typename T>
void IgnorantNodeRemove(node<T> *remove_me){
    // garunteed that remove_me != head of list (thus, don't need to return head)
    node<T> *current_node {remove_me}, *next {nullptr};
    while (current_node->next != nullptr){
        current_node->val = current_node->next->val; // set to next val
        next = current_node->next; // save next to avoid skipping nodes
        current_node->next = current_node->next->next; // now set next to +2
        current_node = next;
    }

    delete current_node;
    return;
}

int main(){
    std::vector <int> vec {1,2,3,4,5};
    // create linked list
    node<int> *head {CreateSingleLL(&vec)};
     
     ReadListForward<int>(head);

    // insert element at random position
    //int node_to_remove {RandomLLElmentGenerator(1, (int)vec.size())};
    head = RemoveNode(head, 2);

    std::cout << ":: Value 2 was removed from linked list => new linked list is: "; 
    ReadListForward<int>(head);

    // reverse list
    {
        // returns new tail => set tail->next TO nullptr
        node<int>* ntail {ReverseLL(head)};
        ntail->next = nullptr;
        head = ntail->head;
    }

    std::cout << "::: Reversed list: ";
    ReadListForward(head);


    // remove given element, but you're unaware of all elements
        // that proceed this element
    node<int> *random_access_ptr = head->next->next;
    IgnorantNodeRemove(random_access_ptr);
    std::cout << ":::: List after random access removal: ";
    ReadListForward(head);

    std::cout << "::::: " << std::boolalpha
        << DeleteLL<int>(head) << " => all heap allocs were deallocated\n";
        
    return 0;
}