#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <string_view>
#include <cstdio>
#include <vector>

enum class prefix : int {PREFIX_ONLY = 1, ENTIRE_STRING};
class trie;
using cTMAP = std::unordered_map<char, trie*>; 

class trie {
    inline static trie *root {nullptr};
    std::unordered_map<char, trie*> map;
public:
    // ctor [default]
    trie () : map {} {}
    ~trie () {};
    // dealloc of nodes is NOT handled by destructor
    void r_delete_trie_node(trie*);
    void insert_string(const std::string*);
    std::string_view find_string(const std::string*, const prefix) const;
    trie* getRoot() const { return root; }
    void collectAllWordsFromHere(std::vector<std::string>*, std::string, trie*);
};

/* 
>> Deallocation of nodes cannot be done by dtor
>> calling `delete` will call the obj deconstrutor,
>> which results in an infinite loop
*/
void trie::r_delete_trie_node(trie *node){
    cTMAP::iterator citer;
    citer = node->map.find('*');
     // base case [only map key is *]
    if (node->map.size() == 1 && citer != node->map.end()){
        delete node;
        return;
    }

     // recrusively call on each child node that is not '*' [* value == nullptr]
    for (auto iter {node->map.begin()}; iter != node->map.end(); ++iter){
        if (iter->first != '*') this->r_delete_trie_node(iter->second);
    }
    delete node;
    return;
}

void trie::insert_string(const std::string *str){
     // handle null root
    if (root == nullptr){
        root = new trie{};
    }

    trie *node {root}; 
    cTMAP::iterator iter; 
    for (const char c : *str){
        iter = node->map.find(c); // search current node's map for character
        if (iter == node->map.end()){
            trie *temp = new trie{};
            node->map.insert({c, temp}); // insert char && new node into current map
            node = temp; // `jump` to new node
        } else node = iter->second; // if char does exist -> go to assoc. child node
    }

    // check to see if full word has already been inserted
    /*
    iter = node->map.find('*');
    if (iter == node->map.end()) node->map.insert({'*', nullptr});
    */
    node->map.insert({'*', nullptr});
    return;
}

std::string_view trie::find_string(const std::string* str, const prefix check) const{
    trie *node = root;
    cTMAP::const_iterator citer;

    size_t c {0};
    for (; c < str->size(); ++c){
        citer = node->map.find(str->at(c));        
        if (citer == node->map.cend()){
            return std::string_view (&str->at(0), c);
        } else node = citer->second;
    }

    // check to ensure it's a complete word and not a prefix
    if (check == prefix::ENTIRE_STRING){
        citer = node->map.find('*');
        if (citer != node->map.cend()) std::cout << "Entire string found: ";
        else std::cout << "Only prefix found: ";
    } else std::cout << "Prefix found: ";
    return std::string_view (&str->at(0), c);
}

// takes any node and computes all strings starting from said node
// currently not very memory efficient but a very simple approach
void trie::collectAllWordsFromHere(std::vector<std::string>* vec_str_p, 
                                   std::string being_built,
                                   trie* node = root){
    cTMAP::const_iterator citer {node->map.find('*')};
     // <<base case>> check to see if terminal child node found
    if (node->map.size() == 1 && citer != node->map.cend()){
        vec_str_p -> push_back(being_built); 
        return;
    }

     // iterate through current node's map
    for (std::pair<char, trie*> p : node->map){
         // not '*'
        if (p.second != nullptr){
            collectAllWordsFromHere(vec_str_p, (being_built + p.first), p.second);
        } else { // '*' case
            vec_str_p -> push_back(being_built);
        }
    }
    return;
}

int main(){
    // create empty trie
    trie string_tri {};

    // read data into trie from file
    std::ifstream data_file {"trie_strings.txt"};
    if (!!data_file){
        std::string str {};
        while (data_file){
            data_file >> str;
            string_tri.insert_string(&str);
        }
        data_file.close();
    }

    std::string str2 {"dog"}; 
    std::string_view rec = string_tri.find_string(&str2, prefix::PREFIX_ONLY);
     // handle empty sv return
    if (rec.size() > 0) std::cout << rec << '\n';
    else std::cout << '\'' << str2 << '\'' << " not found.\n";

    std::vector<std::string> vec_str;
    std::string base;
    string_tri.collectAllWordsFromHere(&vec_str, base);

    // test
    for (const std::string& str : vec_str) std::cout << str << '\n';

     // dealloc nodes && exit
    string_tri.r_delete_trie_node(string_tri.getRoot());
    return 0;
}