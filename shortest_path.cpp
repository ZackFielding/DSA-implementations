#include <iostream>
#include <fstream>
#include <unordered_map>

// weighted_graph class will call on vertex ctors
class weighted_graph;
class vertex {
    std::string name;
    std::unordered_map<vertex*, int> adjacent_weight_map;
    // ctor
    vertex(const std::string str) : name {str}, adjacent_weight_map {} {}
    friend class ::weighted_graph;
};

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
    //vertex* depth_first_search(const std::string to_find);
};

vertex* weighted_graph::add_vertex(const std::string str){
     // create new vertex on heap && add to map
    vertex *n_vert = new vertex{str};
    auto check_insert {map.insert({str, n_vert})};
     // check to see if insertion works, i.e., all vertices must have unique names
    if (!check_insert.second){
        std::clog << "[ERROR]: failed to create new vertex (likely duplicate name).\n";
        delete n_vert;
        return nullptr;
    }
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

int main(){
    std::ifstream file {"shortest_path_data.txt"};
    if (file){
        weighted_graph wgraph {};
        createWeightedGraph(file, &wgraph);
        file.close();
    }

    return 0;
}