#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <unordered_set>

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
        if (root == nullptr){
            root = this;
            return;
        }

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
    // no arg (ONLY for accessing static root var)
    bstree<T>() : val {0}, left {nullptr}, right {nullptr} {}
    
    // member functions
    void RemoveNode(T aval);
    bstree<T>* FindNode(T aval);
    void DeleteBST();
};

// MEMBER FUNC DEFINITIONS template <typename T> void bstree<T>::RemoveNode(T aval){
template <typename T>
bstree<T>* bstree<T>::FindNode(T aval){
    // base cases
    if (this->val == aval) return this;
    
    // recursion
    // 1. Check to see if left/right childnode exists
        // if it does => recurse => if it doesn't => return nullptr
    if (aval < this->val){
        if (this->left != nullptr) return this->left->FindNode(aval);
        else return nullptr;
    }

    // same steps but for right child case (val > node)
    if (aval > this->val){
        if (this->right != nullptr) return this->right->FindNode(aval);
        else return nullptr;
    }
    return nullptr;
}

template <typename T>
void bstree<T>::DeleteBST(){
    // base case
    if (this->left == nullptr && this->right == nullptr){
        delete this;
        return;
    }

    // delete all child nodes prior to deleting parent
    if (this->left != nullptr) this->left->DeleteBST();
    if (this->right != nullptr) this->right->DeleteBST();
    return;
}
// end member func definitions

template <typename T>
bstree<T>* CreateBTSFile(std::ifstream& file, std::vector<T> *vecp, std::unordered_set<T> *included_set){
    T hold; // T can only be int, floats, or chars
     // create binary search tree on heap
    while (file >> hold){
        new bstree<T>{hold};
        vecp->push_back(hold);
        included_set->insert(hold);
    }
     // return root
    return bstree<T> {}.root;
}

// test if all values exist in binary tree
using i2DVEC = std::vector<std::vector<int>>;
template <typename T>
std::pair<bool, i2DVEC> TestBST(bstree<T> *root, std::vector<int>* vecp_included,
    std::vector<int> *vecp_not_included){
     // 2D vector -> n x m [n = included & excluded tests, m = incorrect values found]
    i2DVEC vec_mismatch (2, std::vector<int>(0,0));

    // test values that should be in tree
    for (const int test_val : *vecp_included){
        //.FindNode() will return nullptr if value not found in tree
        if (root->FindNode(test_val) == nullptr) vec_mismatch[0].push_back(test_val);
    }

    for (const int test_val : *vecp_not_included){
        if (root->FindNode(test_val) != nullptr) vec_mismatch[1].push_back(test_val);
    }

    // no errors in BST return values
    if (vec_mismatch[0].size() == 0 && vec_mismatch[1].size() == 0){
        return std::make_pair(false, vec_mismatch);
    }
    // errors in BST return values 
    return std::make_pair(true, vec_mismatch);
}

template <typename T>
void CreateIncorrectVector(std::vector<T> *vec_incorrect, std::unordered_set<T> *included_set){
    
}


int main(){
    std::ifstream file {"bt data.txt."};
    if (!!file){
         // create binary search tree
        std::vector<int> vec; // will be used to test tree
        std::unordered_set<int> included_set; // used to track included values
        [[maybe_unused]] bstree<int> *root {CreateBTSFile<int>(file, &vec, &included_set)};

        // create file of random values that are not in file for testing
        std::vector<int> vec_incorrect;
        CreateIncorrectVector(&vec_incorrect, &included_set);

        // test to ensure BST includes all and only values from file
        auto recieve_test {TestBST(root, vec)};

        constexpr int find_me {33};
        bstree<int> *found = root->FindNode(find_me);
        if (found != nullptr) std::printf("%d found in binary search tree.\n", found->val);
        else std::printf("%d not found in binary search tree.\n", find_me);
        root->DeleteBST();
    }
    return 0;
}