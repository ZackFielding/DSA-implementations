#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cstdio>

class vertex;
using svMAP = std::unordered_map<std::string, vertex*>;
class vertex {
    static vertex ver;
    inline static vertex *root {nullptr}; // single access point
    inline static svMAP map {}; // string - vertex* map

    std::string name;
    std::unordered_set<vertex*> adjacent;
public:
    // ctor & dtor
    vertex() : name {}, adjacent {} {}
    vertex(std::string str) : name {str}, adjacent {} { if (root == nullptr) root = this; }
    ~vertex(){}
    // member funcs
    void addAdjacents(std::ifstream&);
    void dfs_count(std::unordered_set<const vertex*>*, int*) const;

    friend vertex* getRoot();
    friend svMAP* getVertexMap();
    friend void freeVertices();
};

vertex* getRoot() { return vertex::root; }
svMAP* getVertexMap(){ return &vertex::map; }

void vertex::addAdjacents(std::ifstream& file){
    // read strings until newline char 
    std::string str;
    svMAP::iterator iter;

    int number_adjacents {};
    file >> number_adjacents;
    for (int i {0}; i < number_adjacents; ++i){
        file >> str;
        iter = map.find(str);
        if (iter == map.end()){
            vertex *t = new vertex{str};
            iter = (map.emplace(str, t)).first; 
        }
        this->adjacent.insert(iter->second);
    }
    return;
}

// undirected string graph generated based on text file
void createUndirectedGraphBasedOnFile(std::ifstream& file){

     // get vertex name
    std::string str;
    svMAP::iterator iter;
    svMAP* map_access = getVertexMap();

    while (file >> str){
        iter = map_access->find(str);
        if (iter != map_access->end()){
            iter->second->addAdjacents(file);
        } else { 
            vertex *temp = new vertex{str}; 
            temp->addAdjacents(file);
        }
    }
}

void freeVertices(){
    for (std::pair<std::string, vertex*> free_me : vertex::map){
        delete free_me.second;
    }
    return;
}

void vertex::dfs_count(std::unordered_set<const vertex*> *visited_set, int *count) const {

    // if already visited => retrun [base case]
    std::unordered_set<const vertex*>::iterator iter {visited_set->find(this)};
    if (iter != visited_set->end()){
        return;
    }

    ++(*count);
    visited_set->insert(this);
    for (const auto visit_me : adjacent) visit_me->dfs_count(visited_set, count);
    return;
}

int main(){
    // generate graph  
    std::ifstream f_data {"data_graph_us.txt"};
    if (!!f_data){
         // create graph based on .txt file
        createUndirectedGraphBasedOnFile(f_data);

         // visit all nodes using a depth-first search method
        std::unordered_set<const vertex*> visited_set; 
        int count {0}; 
        getRoot()->dfs_count(&visited_set, &count);
        std::printf("%d vertices counted in graph.\n", count);

         // free heap allocated vertices
        freeVertices();
    }

    return 0;
}