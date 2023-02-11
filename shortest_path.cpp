#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <cstdio>
#include <vector>
#include <string_view>

// weighted_graph class will call on vertex ctors
class weighted_graph;
class vertex {
    std::string name;
    std::unordered_map<vertex*, int> adjacent_weight_map;
    // ctor
    vertex(const std::string str) : name {str}, adjacent_weight_map {} {}
    friend class ::weighted_graph;
    friend std::string_view getVertexName(vertex* vert) { return std::string_view {vert->name.data(), vert->name.size()}; }
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
    std::pair<int, std::forward_list<vertex*>> cheapestPath(const std::string, const std::string);
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

vertex* findShortestKnownVertex(std::unordered_map<vertex*, int> *map, std::unordered_set<vertex*> *visited_map){
    
    // this method is not very memory efficient, but it saves sorting complex pair types
    // cache all known vertices that have been visited
    std::vector<std::pair<vertex*, int>> vec;
    for (const auto p : *map){
        if (visited_map->find(p.first) == visited_map->end()){
            vec.push_back(p);
        }
    }

    // handle edge cases
    if (vec.size() == 0) return nullptr;
    if (vec.size() == 1) return vec[0].first;

    // iterate through vector [assume first is lowest] => find vertex with lowest known distance from origin && return ptr
    std::pair<vertex*, int> lowest_known_vertex {vec[0]};
    for (std::size_t idx {1}; idx < vec.size(); ++idx){
        if (vec[idx].second < lowest_known_vertex.second){
            lowest_known_vertex = vec[idx];
        }
    }

    return lowest_known_vertex.first;
}

void createSinglyLinkedListShortestPath(vertex *origin, vertex* dest, std::forward_list<vertex*> *sll, std::unordered_map<vertex*, vertex*> *map){

     // insert destinations prior stop
    sll->push_front(dest);
    std::forward_list<vertex*>::iterator sll_iter = sll->insert_after(sll->begin(), map->find(dest)->second);
    vertex *previous {nullptr};    

    while (1){
        // 1. Find previously inserted's prior stop
        previous = map->find(*sll_iter)->second;
        // 2. Insert into list
        sll_iter = sll->insert_after(sll_iter, previous);
        // 3. If origin was just inserted => break
        if (previous == origin) break;
    }
    sll->reverse(); // reverse list
    return;
}

// Dijkstra's algorithm
std::pair<int, std::forward_list<vertex*>> weighted_graph::cheapestPath(const std::string origin_str, const std::string dest_str){

    // convert origin & destination to vertex*
    vertex *origin {this->getVertexAccess(origin_str)};
    
     // track cheapest path to all visted vertices (cities)
    std::unordered_map<vertex*, int> m_cheapest_path_cost {};
    m_cheapest_path_cost.insert({origin, 0}); // cheapest path to self (0)

     // track cheapest path vertices: key => stopover, value => current vertex (W)
     // in order to get the cheapest flight from X to Y, we need to visit W before Y
    std::unordered_map<vertex*, vertex*> m_cheapest_path_vertices {};

    // breadth-first search
    std::unordered_set<vertex*> visited_set {};
    visited_set.insert(this->getVertexAccess(origin_str));

    vertex *current_vertex {origin};
    //std::pair<vertex*, int> next_pair {nullptr, std::numeric_limits<int>::max()};
    std::unordered_map<vertex*, int>::iterator iter;
    int current_path_length {0}, length {0};

    while (current_vertex != nullptr){
        visited_set.insert(current_vertex); // mark current vertex as visisted

        // iterate through all of the current vertex's adjacents
        for (const std::pair<vertex*, int> p : current_vertex->adjacent_weight_map){
            iter = m_cheapest_path_cost.find(p.first);
            length = p.second + current_path_length;

            if (iter == m_cheapest_path_cost.end()){ // adjacent has not been `discovered` yet
                m_cheapest_path_cost.insert({p.first, length}); 
                m_cheapest_path_vertices.insert({p.first, current_vertex});

            } else { // adjacent has been disovered
                if (length < iter->second){ // if LESS THAN current listed...
                    iter->second = length; // update with new, shortest path to vertex
                    m_cheapest_path_vertices.find(p.first)->second = current_vertex; // update path tracker with current as new prior shortest stopover to get to adjacent
                }
            }
        }

        // next city to visit is the shortest KNOWN city that HAS NOT BEEN VISITED
        // iterate through known map and find next city
        current_vertex = findShortestKnownVertex(&m_cheapest_path_cost, &visited_set); 
        if (current_vertex != nullptr){
            current_path_length = m_cheapest_path_cost.find(current_vertex)->second;
        }
    }

     // generate singly linked list to illustrate lowest cost path
    std::forward_list<vertex*> sll {};
    createSinglyLinkedListShortestPath(origin, this->getVertexAccess(dest_str), &sll, &m_cheapest_path_vertices);

    // now that all vertices have been visited...
    // find `dest_str` in cheapest path map && return its value as a pair with the singly linked list of optimal path
    return {m_cheapest_path_cost.find(this->getVertexAccess(dest_str))->second, sll};
}

int main(){
    std::ifstream file {"shortest_path_data.txt"};
    if (file){
        weighted_graph wgraph {};
        createWeightedGraph(file, &wgraph);
        file.close();

        std::unordered_set<const vertex*> track;
        //wgraph.depth_first_search(wgraph.getVertexAccess("ElPaso"), to_print::PRINT, &track);

        std::string origin {"Atlanta"}, dest {"ElPaso"};
        std::pair<int, std::forward_list<vertex*>> result_pair {wgraph.cheapestPath(origin, dest)};
        std::printf("Cheapest path from %s to %s is %d.\n", origin.data(), dest.data(), result_pair.first);

        std::cout << "Optimal path: ";
        for (const auto v : result_pair.second) std::cout << getVertexName(v) << ' ';
    }

    return 0;
}