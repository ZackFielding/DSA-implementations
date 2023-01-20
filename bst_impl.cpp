#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <unordered_set>
#include <random>
#include <functional>
#include <cstring>

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
    bstree<T>* RemoveNode(T, bstree<T> *);
    bstree<T>* FindNode(T);
    void DeleteBST();
};

// MEMBER FUNC DEFINITIONS template <typename T> void bstree<T>::RemoveNode(T aval)
template <typename T>
bstree<T>* bstree<T>::RemoveNode(T aval, bstree<T> *prev_node){
    // case 0.5: value does not exist in list (do nothing)
    // case 1: no child nodes => just delete it
    // case 2: node has one child (replace node with child, delete child node)
    // case 3: deleting parent node with 2 children => replace node with next highest node
        // approach: visit right child of node, then keep visiting left nodes until lowest left node level
        // if this node has a right child => fill in spot of lowest left with its right child node
    // base case
    if (this->val == aval){
        // [PASSED] case 1 => no child nodes 
        if (this->left == nullptr && this->right == nullptr){
             // figure out which ptr of parent to null
            if (this == prev_node->right) prev_node->right = nullptr;
            else prev_node->left = nullptr;
            delete this;
            return nullptr;
        } else {
            // right swap is used x2 so use lambda
            auto swap_right = [](bstree<T>* obj)mutable {
                // copy over all of right childs members 
                bstree<T> *to_delete {obj->right};
                obj->val = to_delete->val;
                obj->right = to_delete->right;
                obj->left = to_delete->left;
                delete to_delete; // dealloc right child
            };

        if (this->left == nullptr && this->right != nullptr){
            // [PASSED] case 2a => one right child node
            swap_right(this);

        } else if (this->left != nullptr && this->right == nullptr){
            // case 2b => copy over all of left childs members 
            this->val = this->right->val;
            this->right = this->right->right;
            this->left = this->right->left;
            delete this->right; // dealloc left child

        } else if (this->left !=nullptr && this->right != nullptr){ 
            // case 3 => if right child node has not children
            if (this->right->right == nullptr && this->right->left == nullptr){
                this->val = this->right->val;
                this->right = nullptr;
                this->left = nullptr;

            } else {
                 // iterate through left nodes until terminal left node found
                bstree<T> *replace_me {this}, *current {this->right}, *previous {nullptr};
                while (current->left != nullptr){
                    previous = current;
                    current = current->left;
                }

                // set replace node with terminal left node value
                replace_me->val = current->val; 

                // if terminal left has not right child
                if (current->right == nullptr){
                    previous->left = nullptr; // set parent node of terminal left, left ptr to null
                    delete current; // dealloc this
                } else {
                    swap_right(current);
                }
            }
        }
        return this;
        }
    }

    // recursive node finding
    if (this->left != nullptr) return this->left->RemoveNode(aval, this);
    if (this->right != nullptr) return this->right->RemoveNode(aval, this);
    return nullptr; // case 0.5 => value does not exits in tree
}

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
void CreateIncorrectVector(std::vector<T> *vec_incorrect, std::unordered_set<T> *included_set,
    std::function<bool(std::unordered_set<int>*, const int)> comparison_func){
    // psuedo random num generator set up
    std::random_device rd;
    std::minstd_rand g (rd());
    std::uniform_int_distribution<> uid (1, 100);

    // generate random numbers not in BST for testing purposes
    int hold {0};
    for (auto viter {vec_incorrect->begin()}; viter != vec_incorrect->end(); ++viter){
        // loop until rand generator returns a value not included in the BST
        do {
            hold = uid(g); // get random
        } while (comparison_func(included_set, hold));
        *viter = hold;
    }
    return;
}


int main(int argvc, char **argv){
    std::ifstream file {"bt data.txt."};
    if (!!file){
         // create binary search tree
        std::vector<int> vec; // will be used to test tree
        std::unordered_set<int> included_set; // used to track included values
        [[maybe_unused]] bstree<int> *root {CreateBTSFile<int>(file, &vec, &included_set)};

        // command line arg will determine what lamda to use for incorrect_vector value determination
        std::function<bool(std::unordered_set<int>*, const int)> comparison_func;
        if (argvc > 1){
            constexpr char uc [] {"-u"}, nuc [] {"-n"};
            if (std::strncmp(argv[1], uc, sizeof uc) == 0){
                std::clog << "....... generating unique values ......\n";
                comparison_func = [](std::unordered_set<int> *included_set, const int num)mutable {
                    // return opposite of returned bool
                    auto ibp {included_set->insert(num)};
                    return (ibp.second) ? false : true;
                    };
            } else{
                if (std::strncmp(argv[1], nuc, sizeof nuc) == 0){
                    std::clog << "....... generating non-unique values ......\n";
                } else {
                    std::clog << "Unrecognized command line argument passed - defaulting to non-unique values.\n";
                }
                comparison_func = [](std::unordered_set<int> *included_set, const int num)mutable {
                    // return false if number is not in list (stop)
                    return (included_set->find(num) != included_set->end());
                };
            }
        }

        // create file of random values that are not in file for testing
        std::vector<int> vec_incorrect (vec.size(), 0);
        CreateIncorrectVector(&vec_incorrect, &included_set, comparison_func);

        // test to ensure BST includes all and only values from file
        auto recieve_test {TestBST(root, &vec, &vec_incorrect)};
        if (!recieve_test.first) std::clog << "No erros found in BST.\n";

        for (const auto n : vec_incorrect) std::cout << n << ' ';
        std::cout << '\n';

        // lambda for finding node in bst and relaying result to user
        auto PrintFindValue = [](int val_to_find, bstree<int>* root){
            bstree<int> *found = root->FindNode(val_to_find);
            if (found != nullptr) std::printf("%d found in binary search tree.\n", found->val);
            else std::printf("%d not found in binary search tree.\n", val_to_find);
        };

        // find node
        constexpr int find_remove_int {4};
        PrintFindValue(find_remove_int, root);

        // remove node terminal node
        // test: node with no children
        root->RemoveNode(find_remove_int, nullptr);
        PrintFindValue(find_remove_int, root);

        // test: node with only one (right) terminal child
        root->RemoveNode(10, nullptr);
        PrintFindValue(10, root);

        // dealloc tree
        root->DeleteBST();
    }
    return 0;
}