#include <iostream>
#include <vector>
#include <chrono>

#include <impl/basic/include.hpp>
#include <invariants.hpp>
#include <io.hpp>

#include <sat/solver_cmsat.hpp>

using namespace ba_graph;

void print_graph(const Graph &g)
{
    for (auto &r : g)
    {
        std::cout << r.n() << ":";
        for (auto &n : r.neighbours())
            std::cout << " " << n;
        std::cout << std::endl;
    }
}

void test(std::string file, int max_k, bool sat = false)
{
    std::ifstream inputFile(file);

    std::string line;
    int i = 0;
    while (std::getline(inputFile, line))
    {
        i++;
        if (i % 10 == 0)
            std::cout << file << " " << i << std::endl;
        Graph g = read_graph6_line(line);

        int k = max_k;

        if (sat)
        {
            CMSatSolver solver;
            while (!has_acyclic_colouring_SAT(solver, g, k))
                k++;
        }
        else
        {
            while (!has_acyclic_colouring(g, k))
                k++;
        }

        if (k > max_k)
            std::cout << "Graph: " << line << " Should by <= " << max_k << ", is " << k << "\n";
    }
}

void test_vertex_colouring()
{
    std::cout << "Testing trees" << std::endl;
    test("trees.g6", 2);

    std::cout << "Testing max degree = 3" << std::endl;
    test("degree3.g6", 4);

    std::cout << "Testing max degree = 4" << std::endl;
    test("degree4.g6", 5);

    std::cout << "Testing max degree = 5" << std::endl;
    test("degree5.g6", 7);

    std::cout << "Testing max degree = 6" << std::endl;
    test("degree6.g6", 12);
}

void test_vertex_colouring_SAT()
{
    std::cout << "Testing trees with SAT" << std::endl;
    test("trees.g6", 2, true);

    std::cout << "Testing max degree = 3 with SAT" << std::endl;
    test("degree3.g6", 4, true);

    std::cout << "Testing max degree = 4 with SAT" << std::endl;
    test("degree4.g6", 5, true);

    std::cout << "Testing max degree = 5 with SAT" << std::endl;
    test("degree5.g6", 7, true);

    std::cout << "Testing max degree = 6 with SAT" << std::endl;
    test("degree6.g6", 12, true);
}

void find_edge_colouring_k_regular(std::string file, int time_limit, bool sat = false)
{
    std::ifstream inputFile(file + ".g6");
    std::ofstream outputFile(file + "_edge" + ((sat) ? "_sat" : "") + ".csv");

    outputFile << "graph,#vertices,#edges,acyclic edge chromatic index,duration" << std::endl;

    std::string line;

    std::vector<int> last_5_times = {0, 0, 0, 0, 0};
    while (std::getline(inputFile, line))
    {
        auto start = std::chrono::high_resolution_clock::now();
        Graph g = read_graph6_line(line);
        outputFile << line << "," << g.order() << "," << g.size() << "," << std::flush;

        Graph lg = line_graph(g);

        int k = g[0].degree() + 2;
        if (g[0].degree() == 3)
            k--;
        std::cout << k - 2 << " " << lg.order() << " " << lg.size() << " lg ";

        if (sat)
        {
            CMSatSolver solver;
            while (!has_acyclic_colouring_SAT(solver, lg, k))
            {
                std::cout << k << " ";
                k++;
            }
        }
        else
        {
            while (!has_acyclic_colouring(lg, k))
            {
                std::cout << k << " ";
                k++;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << line << " " << duration << std::endl;

        outputFile << k << "," << duration << std::endl;

        last_5_times.push_back(duration);
        last_5_times.erase(last_5_times.begin());

        int running_time_average = last_5_times[0] + last_5_times[1] + last_5_times[2] + last_5_times[3] + last_5_times[4];
        running_time_average /= 5;

        if (running_time_average > time_limit)
            break;
    }

    outputFile << "end" << std::endl;
    inputFile.close();
    outputFile.close();
}

void find_vertex_colouring_k_regular(std::string file, int time_limit, bool sat = false)
{
    std::ifstream inputFile(file + ".g6");
    std::ofstream outputFile(file + "_vertex" + ((sat) ? "_sat" : "") + ".csv");

    outputFile << "graph,#vertices,#edges,acyclic vertex chromatic index,duration" << std::endl;

    std::string line;

    std::vector<int> last_5_times = {0, 0, 0, 0, 0};
    while (std::getline(inputFile, line))
    {
        auto start = std::chrono::high_resolution_clock::now();
        Graph g = read_graph6_line(line);
        outputFile << line << "," << g.order() << "," << g.size() << "," << std::flush;

        int k = g[0].degree() + 2;
        if (g[0].degree() == 3)
            k--;
        std::cout << k - 2 << " " << g.order() << " " << g.size() << " ";

        if (sat)
        {
            CMSatSolver solver;
            while (!has_acyclic_colouring_SAT(solver, g, k))
            {
                std::cout << k << " ";
                k++;
            }
        }
        else
        {
            while (!has_acyclic_colouring(g, k))
            {
                std::cout << k << " ";
                k++;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << line << " " << duration << std::endl;

        outputFile << k << "," << duration << std::endl;

        last_5_times.push_back(duration);
        last_5_times.erase(last_5_times.begin());

        int running_time_average = last_5_times[0] + last_5_times[1] + last_5_times[2] + last_5_times[3] + last_5_times[4];
        running_time_average /= 5;

        if (running_time_average > time_limit)
            break;
    }

    outputFile << "end" << std::endl;
    inputFile.close();
    outputFile.close();
}

void find_acyclic_edge_chromatic_index(int time_limit)
{
    find_edge_colouring_k_regular("3-regular", time_limit);
    find_edge_colouring_k_regular("4-regular", time_limit);
    find_edge_colouring_k_regular("5-regular", time_limit);
    find_edge_colouring_k_regular("6-regular", time_limit);
    find_edge_colouring_k_regular("7-regular", time_limit);
    find_edge_colouring_k_regular("8-regular", time_limit);
    find_edge_colouring_k_regular("9-regular", time_limit);
    find_edge_colouring_k_regular("10-regular", time_limit);
}

void find_acyclic_edge_chromatic_index_SAT(int time_limit)
{
    find_edge_colouring_k_regular("3-regular", time_limit, true);
    find_edge_colouring_k_regular("4-regular", time_limit, true);
    find_edge_colouring_k_regular("5-regular", time_limit, true);
    find_edge_colouring_k_regular("6-regular", time_limit, true);
    find_edge_colouring_k_regular("7-regular", time_limit, true);
    find_edge_colouring_k_regular("8-regular", time_limit, true);
    find_edge_colouring_k_regular("9-regular", time_limit, true);
    find_edge_colouring_k_regular("10-regular", time_limit, true);
}

void find_acyclic_vertex_chromatic_index(int time_limit)
{
    find_vertex_colouring_k_regular("3-regular", time_limit);
    find_vertex_colouring_k_regular("4-regular", time_limit);
    find_vertex_colouring_k_regular("5-regular", time_limit);
    find_vertex_colouring_k_regular("6-regular", time_limit);
    find_vertex_colouring_k_regular("7-regular", time_limit);
    find_vertex_colouring_k_regular("8-regular", time_limit);
    find_vertex_colouring_k_regular("9-regular", time_limit);
    find_vertex_colouring_k_regular("10-regular", time_limit);
}

void find_acyclic_vertex_chromatic_index_SAT(int time_limit)
{
    find_vertex_colouring_k_regular("3-regular", time_limit, true);
    find_vertex_colouring_k_regular("4-regular", time_limit, true);
    find_vertex_colouring_k_regular("5-regular", time_limit, true);
    find_vertex_colouring_k_regular("6-regular", time_limit, true);
    find_vertex_colouring_k_regular("7-regular", time_limit, true);
    find_vertex_colouring_k_regular("8-regular", time_limit, true);
    find_vertex_colouring_k_regular("9-regular", time_limit, true);
    find_vertex_colouring_k_regular("10-regular", time_limit, true);
}

void test_max_input(std::string file, int time_limit, bool sat = false)
{
    std::ifstream inputFile(file + ".g6");
    std::ofstream outputFile(file + ((sat) ? "_sat" : "") + ".csv");

    outputFile << "graph,#vertices,#edges,max degree,acyclic vertex chromatic index,duration" << std::endl;

    std::string line;

    std::vector<int> last_5_times = {0, 0, 0, 0, 0};
    while (std::getline(inputFile, line))
    {
        auto start = std::chrono::high_resolution_clock::now();
        Graph g = read_graph6_line(line);
        outputFile << line << "," << g.order() << "," << g.size() << "," << max_deg(g) << "," << std::flush;

        int k = max_deg(g) + 2;
        std::cout << max_deg(g) << " " << g.order() << " " << g.size() << " : ";

        if (sat)
        {
            ba_graph::CMSatSolver solver;
            while (!has_acyclic_colouring_SAT(solver, g, k))
            {
                std::cout << k << " ";
                k++;
            }
        }
        else
        {
            while (!has_acyclic_colouring(g, k))
            {
                std::cout << k << " ";
                k++;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << line << " " << duration << std::endl;

        outputFile << k << "," << duration << std::endl;

        last_5_times.push_back(duration);
        last_5_times.erase(last_5_times.begin());

        int running_time_average = last_5_times[0] + last_5_times[1] + last_5_times[2] + last_5_times[3] + last_5_times[4];
        running_time_average /= 5;

        if (running_time_average > time_limit)
            break;
    }

    inputFile.close();
    outputFile.close();
}

void test_max()
{
    test_max_input("test_max_3", 10);
    test_max_input("test_max_4", 10);
    test_max_input("test_max_5", 10);
    test_max_input("test_max_6", 10);
    test_max_input("test_max_7", 10);
    test_max_input("test_max_8", 10);
    test_max_input("test_max_9", 10);
    test_max_input("test_max_10", 10);
}

void test_max_SAT()
{
    test_max_input("test_max_3", 10, true);
    test_max_input("test_max_4", 10, true);
    test_max_input("test_max_5", 10, true);
    test_max_input("test_max_6", 10, true);
    test_max_input("test_max_7", 10, true);
    test_max_input("test_max_8", 10, true);
    test_max_input("test_max_9", 10, true);
    test_max_input("test_max_10", 10, true);
}

int main(int argc, char *argv[])
{
    // test_vertex_colouring_SAT();
    // test_vertex_colouring();

    // find_acyclic_edge_chromatic_index_SAT(10);
    // find_acyclic_edge_chromatic_index(10);

    // find_acyclic_vertex_chromatic_index_SAT(10);
    // find_acyclic_vertex_chromatic_index(10);

    test_max_SAT();
    // test_max();

    test_max_input("max_20_vertices", 60, true);
    // test_max_input("max_20_vertices", 10);

    return 0;
}