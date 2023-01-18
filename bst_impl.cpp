#include <iostream>
#include <fstream>
#include <vector>

template <typename T>
class bstree {
public:
    inline static bstree<T> *root {nullptr};    
    T val;
    bstree<T> *left;
    bstree<T> *right;
    // creation ctor
    bstree(T aval) : val {aval}, left {nullptr}, right {nullptr} {
         // if root
        if (root == nullptr) root = this;

         // if not root, need to find what parent this needs to be assigned to
        bstree<T> *current_node {root}; // start at root
        while (1){
            if (this->val < current_node->val){ // if lhs... (less than)
                if (current_node->left == nullptr){ // if empty slot => assign this
                    current_node->left = this;
                    break; 
                } else {
                    current_node = current_node->left; // keep searching left-sub-nodes
                }
            } else { // if rhs ... (greater than)
                if (current_node->right == nullptr){ // same process but for rhs nodes
                    current_node->right = this;
                    break;
                } else {
                    current_node = current_node->right;
                }
            }
        }

    } 
};

template <typename T>
bstree<T>* CreateBTSFile(std::ifstream& file){
    T hold; // T can only be int, floats, or chars
     // create binary search tree on heap
    while (file >> hold) bstree<T> *t = new bstree<T>{hold};
     // return root
    return bstree<T> {}.root;
}

int main(){
    std::ifstream file {"bt data.txt."};
    if (!!file){
         // create binary search tree
        bstree<int> *root {CreateBTSFile<int>(file)};
    }
    return 0;
}