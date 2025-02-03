#include "implementation.h"

#include <config/configuration.hpp>

using namespace ba_graph;

int main()
{
    Configuration cfg;
    cfg.load_from_file("ba_graph.cfg");

    assert(cfg.storage_dir() == "/home/vyskum_data");
    assert(cfg.tmp_dir() == "/tmp");
    assert(cfg.num_threads() == 1);
    assert(cfg.sat_solver_command(0) == "./sbva_wrapped cadical_for_sbva_2023");
    assert(cfg.sat_solver_output_type(0) == "SAT_COMPETITION");
    assert(cfg.allsat_solver_command(0) == "./bddminisat_static");
    assert(cfg.allsat_solver_output_type(0) == "BDD_MINISAT");
}
