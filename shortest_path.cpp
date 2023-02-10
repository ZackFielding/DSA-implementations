#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

// weighted_graph class will call on vertex ctors
class weighted_graph;
class vertex {
    std::string name;
    std::unordered_map<vertex*, int> adjacent_weight_map;
    // ctor
    vertex(const std::string str) : name {str}, adjacent_weight_map {} {}
    friend class ::weighted_graph;
};

enum class to_print : int {NO_PRINT = 0, PRINT = 1};

class weighted_graph {
    std::unordered_map<std::string, vertex*> map;
public:
    weighted_graph() : map {} {};
    ~weighted_graph()
    {
         // iterate through map of graphs and delete vertices
        for (std::pair<std::string, vertex*> p : map) delete p.second;
    }
    vertex* add_vertex(const std::string);
    void add_adjacent(vertex*, const std::string, const int weight);
    // TO DO: implement DFS (? search for a vertex by string ? option to read out all nodes as the algo executes)
    void depth_first_search(vertex* vert, const to_print, std::unordered_set<const vertex*>*);
     // return an access point for search algos
    vertex* getVertexAccess(const std::string str) { return map.find(str)->second; }
};

vertex* weighted_graph::add_vertex(const std::string str){
    // check to see if vertex has already been created
    auto check_if_exist_iter {map.find(str)};

    // if it doesn't exist => create new one, ELSE => assign ptr
    vertex* n_vert {nullptr};
    if (check_if_exist_iter == map.end()){
        n_vert = new vertex{str};
        map.insert({str, n_vert});
    } else n_vert = check_if_exist_iter->second;

    return n_vert;
}

void weighted_graph::add_adjacent(vertex* current_vert, const std::string str, int weight = 1){
    // need to get ptr to correct adjacent => check to see if adjacent node exists
    auto iter = map.find(str);
    if (iter == map.end()){
        this->add_vertex(str);
        iter = map.find(str);
        
        if (iter == map.end()){
            std::cerr << "[ERROR]: failure to add adjacent; likely an edge to itself occured.\n";
            return;
        }
    }
     // insert adjacent ptr and weight to current vertex's adjacent map
    current_vert->adjacent_weight_map.insert({iter->second, weight});
    return;
}

void createWeightedGraph(std::ifstream& file, weighted_graph *wgraph){
    std::string str;
    int num_adjacents {0}, weight_to_add {0};
    vertex* current_vertex {nullptr};
    
    while (file >> str >> num_adjacents){
        current_vertex = wgraph->add_vertex(str);
        for (int lv {0}; lv < num_adjacents; ++lv){
            file >> str >> weight_to_add;
            wgraph->add_adjacent(current_vertex, str, weight_to_add); 
        }
    }
    return;
}

void weighted_graph::depth_first_search(vertex* vert, const to_print e_print, std::unordered_set<const vertex*> *track){
    std::unordered_set<const vertex*>::iterator iter {track->find(vert)};
     // base case
    if (iter != track->end()){
        return;
    }

     // handle print enum
    switch (e_print) {
        case (to_print::PRINT): {
           std::cout << vert->name << ' '; 
           break;
        }
        case (to_print::NO_PRINT) : {
            break;
        }
    }

    track->insert(vert);
    for (std::pair<vertex*, int> p : vert->adjacent_weight_map){
        this->depth_first_search(p.first, e_print, track);
    }
    return;
}

int main(){
    std::ifstream file {"shortest_path_data.txt"};
    if (file){
        weighted_graph wgraph {};
        createWeightedGraph(file, &wgraph);
        std::unordered_set<const vertex*> track;
        wgraph.depth_first_search(wgraph.getVertexAccess("ElPaso"), to_print::PRINT, &track);
        file.close();
    }

    return 0;
}