#include <iostream>
#include <vector>
#include <cstring>
#define TEMP template<typename T>

// object-otriented implementation of a max-heap
TEMP
class mHeap {
inline static auto getParent = [](T pos) { return (pos-1)/2; };
inline static auto getLeftChild = [](T pos) { return (pos*2)+1; };
inline static auto getRightChild = [](T pos) { return (pos*2)+2; };
    T *heap;
    long long size; // array size => NOT BYTE SIZE
    long long right_most_pos;
public:
    //ctor no-arg
    mHeap() : heap {new T[5]}, size {5}, right_most_pos {0} {}
    //dtor
    ~mHeap() { delete [] heap; }

    // insertion
    void insert(T val); 
    // deletion
    void remove();
    // read array (for testing)
    void read_as_array();
};

TEMP
void mHeap<T>::insert(T val){
    // insert value into right-most available node
    // first check size of array and if re-sizing needs to take place
    if (right_most_pos >= size){
        const auto old_byte_size {4*size}; // required for memcpy
        size += 5; // increase size by 50
        T *new_heap =  new T[size]; // allocate new heap
        std::memcpy(new_heap, heap, old_byte_size); // copy contents to new heap 
        delete [] heap; // dealloc hold heap
        heap = new_heap; // point to new heap alloc
    }

    // assign to right most position (new last-node) && ++right most
    heap[right_most_pos++] = val;

    // now trickle-up, checking parent nodes
    long long idx {right_most_pos-1}, parent_idx {getParent(idx)};  
    long temp {0};
    while ((parent_idx >= 0) && (heap[idx] > heap[parent_idx])){
        temp = heap[parent_idx]; // cache parent val
        heap[parent_idx] = heap[idx]; // swap parent and new node
        heap[idx] = temp; // parent to child node swap
        idx = parent_idx; // assign new nodes position as parent node post-swap
        parent_idx = getParent(idx); // compute new parent node (parent of parent)
    }
}

TEMP
void mHeap<T>::remove(){
    // 1. replace root node with last node (-- right most to comp for deletion)
    right_most_pos -= 1;
    heap[0] = heap[right_most_pos];

    // now trickle down, stopping once current node > greatest child node
    long long idx {0}, left {getLeftChild(idx)}, right {getRightChild(idx)}, greatest_child {0};
    int temp {0};
    while (left < right_most_pos){ // while-cond checks if node has atleast a left child    
        // check to see if right child node exists
         // if it does => determine the greater of the values
        if (right < right_most_pos){
            greatest_child = (heap[left] > heap[right]) ? left : right;
        } else greatest_child = left;

        // if greatest-child node is greater than current node => swap
            // else => stop here (correct position found)
        if (heap[greatest_child] > heap[idx]){
            temp = heap[greatest_child]; // cache greatest child node value
            heap[greatest_child] = heap[idx]; // current parent node assigned to greatest child node
            heap[idx] = temp; // assign greatest child node as new parent
            idx = greatest_child;
            left = getLeftChild(idx);
            right = getRightChild(idx);
        } else break;
    }
}

TEMP
void mHeap<T>::read_as_array(){
    for (long long i {0}; i < right_most_pos; ++i) std::cout << heap[i] << ' ';
    std::cout << '\n';
    return;
}

int main(){
    // generate int heap
    mHeap<int> heap {};

    {
         // don't need insertion vector post-insertion
        std::vector<int> to_insert {100, 46, 9, 12, 35, 98, 76, 54, 2};
        for (const int val : to_insert) heap.insert(val);
    }
    
    // read (test)
    heap.read_as_array();
    heap.remove();
    heap.read_as_array();

    return 0;
}