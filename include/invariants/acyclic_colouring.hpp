#include <iostream>
#include <set>

namespace ba_graph
{
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
}
