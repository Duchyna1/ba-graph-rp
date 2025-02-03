#ifndef BA_GRAPH_INVARIANTS_CONNECTIVITY_HPP
#define BA_GRAPH_INVARIANTS_CONNECTIVITY_HPP

#include "distance.hpp"
#include "../operations/undo.hpp"
#include "../operations/copies.hpp"

namespace ba_graph
{

inline bool is_connected(const Graph &G)
{
    if (G.order() <= 1) {
        // TODO
        throw std::invalid_argument("graph too small for connectivity computations");
    }

    Number v = G.find(RP::all())->n();
    auto d = distances(G, v);
    for (auto &r : G) {
        if (d.count(r.n()) == 0) {
            // vertex r.n() not reachable
            return false;
        }
    }
    return true;
}

inline bool has_cut_vertex(const Graph &G)
{
    if (G.order() <= 1) {
        // TODO
        throw std::invalid_argument("graph too small for connectivity computations");
    }

    Factory f;
    for (auto &r: G) {
        Graph H = copy_other_factory(G, f);
        deleteV(H, r.n(), f);
        if (!is_connected(H))
            return true;
    }
    return false;
}

// is vertex k-connected
inline bool is_connected(const Graph &G, int k)
{
    if (G.order() <= 1) {
        // TODO
        throw std::invalid_argument("graph too small for connectivity computations");
    }

    if (k == 1) {
        return is_connected(G);
    } else if (k == 2) {
        return !has_cut_vertex(G);
    } else if (k == 3) {
        Factory f;
        for (auto &r: G) {
            Graph H = copy_other_factory(G, f);
            deleteV(H, r.n(), f);
            if (has_cut_vertex(H))
                return false;
        }
        return true;
    } else {
        std::string msg = "vertex connectivity not implemented for k = " + std::to_string(k);
        throw std::invalid_argument(msg);
    }
}

// TODO only works up to connectivity 3
// TODO https://www.cse.msu.edu/~cse835/Papers/Graph_connectivity_revised.pdf
inline int vertex_connectivity(const Graph &G)
{
    if (is_connected(G, 3)) return 3;
    if (is_connected(G, 2)) return 2;
    if (is_connected(G, 1)) return 1;
    return 0;
}

inline std::vector<Number> cut_vertices(const Graph &G)
{
    std::vector<Number> result;
    Factory f;
    auto H = copy_other_factory(G, f);
    for (auto &rv : H) {
        Number v = rv.n();
        auto cleared = clear_edges(H, {v}, f);
        auto cs = components(H);
        if (cs.size() > 2)
            result.push_back(v);
        restore_edges(H, cleared, f);
    }
    return result;
}

inline bool has_cut_edge(const Graph &G)
{
    Factory f;
    auto H = copy_other_factory(G, f);
    for (auto &loc : H.list(RP::all(), IP::primary(), IT::l())) {
        auto cleared = clear_edges(H, {loc}, f);
        auto cs = components(H);
        if (cs.size() > 1)
            return true;
        restore_edges(H, cleared, f);
    }
    return false;
}

inline std::vector<Location> cut_edges(const Graph &G)
{
    std::vector<Location> result;
    Factory f;
    auto H = copy_other_factory(G, f);
    for (auto &loc : H.list(RP::all(), IP::primary(), IT::l())) {
        auto cleared = clear_edges(H, {loc}, f);
        auto cs = components(H);
        if (cs.size() > 1)
            result.push_back(loc);
        restore_edges(H, cleared, f);
    }
    return result;
}

}  // namespace end

#endif
