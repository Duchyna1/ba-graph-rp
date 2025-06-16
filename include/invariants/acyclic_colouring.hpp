#include <iostream>
#include <set>
#include <sat/solver.hpp>
#include <sat/cnf.hpp>
#include <sat/exec_solver.hpp>
#include <map>
#include <algorithms/paths.hpp>
#include <impl/basic/graph.h>

namespace ba_graph
{
    std::map<std::string, int> string_to_int;
    namespace internal
    {
        bool detect_cycle(const Graph &G, std::set<int> &allowed_colours, std::map<Number, int> &colouring, Number current, std::vector<Number> &parent)
        {
            for (auto &neighbour : G[current].neighbours())
            {
                // parent
                if (parent[current.to_int()] == neighbour)
                    continue;

                // cycle
                if (parent[neighbour.to_int()] != Number(-1))
                    return true;

                // wrong colour
                if (allowed_colours.find(colouring[neighbour]) == allowed_colours.end())
                    continue;

                parent[neighbour.to_int()] = current;
                if (detect_cycle(G, allowed_colours, colouring, neighbour, parent))
                    return true;
            }

            return false;
        }

        bool contains_cycle(const Graph &G, int c1, int c2, std::map<Number, int> &colouring)
        {
            std::vector<Number> parent(G.order(), Number(-1));
            std::set<int> allowed_colours;
            allowed_colours.insert(c1);
            allowed_colours.insert(c2);

            for (int i = 0; i < G.order(); i++)
            {
                // wrong colour
                if (colouring[Number(i)] != c1 && colouring[Number(i)] != c2)
                    continue;

                if (parent[i] == -1)
                {
                    parent[i] = i;
                    if (detect_cycle(G, allowed_colours, colouring, i, parent))
                        return true;
                }
            }

            return false;
        }

        bool is_acyclic(const Graph &G, std::map<Number, int> &colouring, int k)
        {
            for (int i = 0; i < k; i++)
            {
                for (int j = i + 1; j < k; j++)
                {
                    if (contains_cycle(G, i, j, colouring))
                        return false;
                }
            }

            return true;
        }

        bool is_regular(const Graph &G, std::map<Number, int> &colouring)
        {
            for (auto &r : G)
            {
                for (auto &n : r.neighbours())
                {
                    if (colouring[r.n()] == colouring[n])
                        return false;
                }
            }

            return true;
        }

        bool will_stay_regular(const Graph &G, std::map<Number, int> &colouring, Number v, int c)
        {
            for (auto &n : G[v].neighbours())
            {
                if (colouring[n] == c)
                    return false;
            }

            return true;
        }

        bool colour(const Graph &G, std::map<Number, int> &colouring, int k, Number v)
        {
            if (!G.contains(v))
                return is_acyclic(G, colouring, k);

            for (int c = 0; c < k; c++)
            {
                if (!will_stay_regular(G, colouring, v, c))
                    continue;
                colouring[v] = c;
                colour(G, colouring, k, v + 1);
                if (colour(G, colouring, k, v + 1))
                    return true;
            }

            colouring[v] = -1;
            return false;
        }

        int c(int vertex, int color)
        {
            return string_to_int["c_" + std::to_string(vertex) + "_" + std::to_string(color)];
        }

        int d(int vertex, int c1, int c2)
        {
            if (c1 > c2)
                std::swap(c1, c2);

            return string_to_int["d_" + std::to_string(vertex) + "_" + std::to_string(c1) + "_" + std::to_string(c2)];
        }
    }

    bool has_acyclic_colouring(const Graph &G, int k)
    {
        std::map<Number, int> colouring;
        for (auto &r : G)
            colouring[r.n()] = -1;

        if (!internal::colour(G, colouring, k, Number(0)))
            return false;

        return true;
    }

    bool has_acyclic_colouring_SAT(const SatSolver &solver, const Graph &G, int k)
    {
        CNF cnf;

        int lit_count = 0;

        // Preparing literals
        for (int i = 0; i < G.order(); i++)
        {
            for (int j = 0; j < k; j++)
            {
                string_to_int["c_" + std::to_string(i) + "_" + std::to_string(j)] = lit_count++;
            }
        }

        for (int i = 0; i < G.order(); i++)
        {
            for (int j1 = 0; j1 < k; j1++)
            {
                for (int j2 = j1 + 1; j2 < k; j2++)
                {
                    string_to_int["d_" + std::to_string(i) + "_" + std::to_string(j1) + "_" + std::to_string(j2)] = lit_count++;
                }
            }
        }

        cnf.first = lit_count;

        // At least one color per vertex
        for (int i = 0; i < G.order(); i++)
        {
            Clause clause;

            for (int j = 0; j < k; j++)
            {
                clause.push_back(Lit(internal::c(i, j), false));
            }

            cnf.second.push_back(clause);
        }

        // Max one coulor per vertex
        for (int i = 0; i < G.order(); i++)
        {
            for (int j1 = 0; j1 < k; j1++)
            {
                for (int j2 = j1 + 1; j2 < k; j2++)
                {
                    Clause clause;
                    clause.push_back(Lit(internal::c(i, j1), true));
                    clause.push_back(Lit(internal::c(i, j2), true));

                    cnf.second.push_back(clause);
                }
            }
        }

        // Regular coloring
        for (auto edge : G.list(RP::all(), IP::primary(), IT::e()))
        {
            int i1 = edge.v1().to_int();
            int i2 = edge.v2().to_int();

            for (int j = 0; j < k; j++)
            {
                Clause clause;
                clause.push_back(Lit(internal::c(i1, j), true));
                clause.push_back(Lit(internal::c(i2, j), true));
                cnf.second.push_back(clause);
            }
        }

        // Acyclic coloring
        // d <=> (not c1 and not c2)
        for (int i = 0; i < G.order(); i++)
        {
            for (int j1 = 0; j1 < k; j1++)
            {
                for (int j2 = j1 + 1; j2 < k; j2++)
                {
                    Clause clause1;
                    clause1.push_back(Lit(internal::d(i, j1, j2), true));
                    clause1.push_back(Lit(internal::c(i, j1), true));
                    cnf.second.push_back(clause1);

                    Clause clause2;
                    clause2.push_back(Lit(internal::d(i, j1, j2), true));
                    clause2.push_back(Lit(internal::c(i, j2), true));
                    cnf.second.push_back(clause2);

                    Clause clause3;
                    clause3.push_back(Lit(internal::d(i, j1, j2), false));
                    clause3.push_back(Lit(internal::c(i, j1), false));
                    clause3.push_back(Lit(internal::c(i, j2), false));
                    cnf.second.push_back(clause3);
                }
            }
        }

        std::set<Circuit> circuits = all_circuits(G);
        for (const auto &circuit : circuits)
        {
            std::vector<Number> vertices = circuit.vertices();
            int n = vertices.size();

            for (int j1 = 0; j1 < k; j1++)
            {
                for (int j2 = j1 + 1; j2 < k; j2++)
                {
                    Clause clause;
                    for (int i = 0; i < n; i++)
                    {
                        int vertex = vertices[i].to_int();
                        clause.push_back(Lit(internal::d(vertex, j1, j2), false));
                    }
                    cnf.second.push_back(clause);
                }
            }
        }

        return satisfiable(solver, cnf);
    }
}
