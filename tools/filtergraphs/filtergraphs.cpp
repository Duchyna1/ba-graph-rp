#include <climits>
#include <iomanip>
#include <iostream>
#include <omp.h>

#include <impl/basic/include.hpp>
#include <config/configuration.hpp>
#include <algorithms/cyclic_connectivity.hpp>
//#include <algorithms/isomorphism/isomorphism_nauty.hpp>
#include <snarks/colouring.hpp>
#include <snarks/colouring_nagy.hpp>
#include <io/graph6.hpp>
#include <util/cxxopts.hpp>
#include <util/parallel.hpp>

using namespace ba_graph;

const int lineBufferSize = 100000;

cxxopts::Options options("filtergraphs", "\nFilter input graphs (g6/s6) from stdin according to order, cyclic connectivity, girth, 3-edge-colourability etc.\nCan canonicalize output if desired.\n");

void writeGraphs(std::vector<std::string> *s6graphs)
{
	for (auto &s : *s6graphs)
		std::cout << s;
	s6graphs->clear();
}

void wrong_usage()
{
	std::cerr << options.help() << std::endl;
	exit(1);
}

bool compareValue(int graphValue, int argValue, std::string comparator)
{
	if (comparator == "<") {
		return graphValue < argValue;
	} else if (comparator == "<=") {
		return graphValue <= argValue;
	} else if (comparator == "==") {
		return graphValue == argValue;
	} else if (comparator == ">=") {
		return graphValue >= argValue;
	} else if (comparator == ">") {
		return graphValue > argValue;
	} else if (comparator == "!=") {
		return graphValue != argValue;
	} else {
		std::cerr << "wrong comparator: " << comparator << std::endl;
		exit(1);
	}
}

bool satisfiesNumericalRequirement(int value, std::string paramName, cxxopts::ParseResult options)
{
	if (options.count(paramName)) {
		int paramValue = options[paramName].as<int>();

		if (options.count("comparator") != 1) {
			std::cerr << "wrong comparator" << std::endl;
			exit(1);
		}
		std::string comparator = options["comparator"].as<std::string>();

		return compareValue(value, paramValue, comparator);
	}
	return true;
}

/*
 * Takes input file with g6/s6 graphs, applies a filter and puts the resulting graphs into the output s6 file (as canonicalized string if desired).
 */
int main(int argc, char **argv) {
	try {
		options.add_options()
			("h, help", "print help")

			("order", "order", cxxopts::value<int>())
			("size", "size (number of edges)", cxxopts::value<int>())
			("girth", "girth", cxxopts::value<int>())
			("vertex-connectivity", "vertex connectivity [ONLY WORKS UP TO 3]", cxxopts::value<int>())
			("cc", "cyclic connectivity [only for cubic]", cxxopts::value<int>())
			("cut-vertices", "number of cut-vertices", cxxopts::value<int>())
			("cut-edges", "number of cut-edges", cxxopts::value<int>())
			("loops", "number of loops", cxxopts::value<int>())
			("parallel-edges", "number of pairs of vertices joined by a parallel edge", cxxopts::value<int>())
			("chi", "chromatic index", cxxopts::value<int>())

			("comparator", "symbol to use for comparison: <  <=  ==  >=  >  !=", cxxopts::value<std::string>())

			("simple", "output only simple graphs")
			("claw-free", "output only claw-free graphs")
			// ("multiple-edges", "# of multiple edges >=", cxxopts::value<int>())
			// ("noloops", "output only multigraps without loops")
			// ("loops", "# of loops >=", cxxopts::value<int>())

			// ("s, snark", "output only uncolourable graphs [only for subcubic]")

//			("canonical", "canonicalise output [nauty, only for g >= 3]")

			("chunk", "size of input chunk for each thread (in lines)", cxxopts::value<int>()->default_value(std::to_string(1000)))
			;
		auto o = options.parse(argc, argv);

		if (o.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}

		Configuration cfg;
		try {
			cfg.load_from_file("ba_graph.cfg");
			omp_set_num_threads(cfg.num_threads());
		} catch (...) {
			omp_set_num_threads(1);
		}

		auto processGraph = [&](std::string &sG, Graph &G, Factory &, std::vector<std::string> *s6graphs) {
			if (!satisfiesNumericalRequirement(G.order(), "order", o)) return;
			if (!satisfiesNumericalRequirement(G.size(), "size", o)) return;
			if (!satisfiesNumericalRequirement(girth(G), "girth", o)) return;
			if (!satisfiesNumericalRequirement(vertex_connectivity(G), "vertex-connectivity", o)) return;
			if (!satisfiesNumericalRequirement(cyclic_connectivity(G), "cc", o)) return;
			if (!satisfiesNumericalRequirement(cut_vertices(G).size(), "cut-vertices", o)) return;
			if (!satisfiesNumericalRequirement(cut_edges(G).size(), "cut-edges", o)) return;
			if (!satisfiesNumericalRequirement(G.order(), "loops", o)) return;
			if (!satisfiesNumericalRequirement((int)G.list(RP::all(), IP::loop() && IP::primary()).size(), "parallel-edges", o)) return;

			int chi = -1;
			if (max_deg(G) == 3 && min_deg(G) == 3) {
				chi = is_colourable<NagyColouriser>(G) ? 3 : 4;
			} else {
				chi = chromatic_index_basic(G);
			}
			if (!satisfiesNumericalRequirement(chi, "chi", o)) return;

			if (o.count("simple")) {
				if (has_loop(G) || has_parallel_edge(G)) return;
			}

			if (o.count("claw-free")) {
				if (!is_claw_free(G)) return;
			}

			// G satisfies all the requirements
			std::string sG_out;
			// if (o.count("canonical")) {
			// 	sG_out = canonical_sparse6(G) + "\n";
			// } else {
				sG_out = sG + "\n";
				// if not canonicalising, keep the original string for G
			// }

			#pragma omp critical
			{
				s6graphs->push_back(sG_out);
				if (s6graphs->size() > lineBufferSize)
					writeGraphs(s6graphs);
			}
		};

		std::vector<std::string> s6graphs;
		ReadGraphsParams params;
		params.chunk_size = o["chunk"].as<int>();
		read_graph6_stream_chunks<std::vector<std::string>>(
				std::cin, params, processGraph, &s6graphs);
		writeGraphs(&s6graphs);
	} catch (const cxxopts::exceptions::exception& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
