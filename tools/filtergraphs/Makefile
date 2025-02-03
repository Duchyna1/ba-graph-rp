CFLAGS = -std=c++20 -fopenmp -I../../include -I/usr/include/nauty
LDFLAGS = -lstdc++fs -lnauty
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG -D_GLIBCXX_DEBUG
COMPILE_DBG = $(CXX) $(CFLAGS) $(DBGFLAGS)
COMPILE = $(CXX) $(CFLAGS) -DNDEBUG -O3

all: filtergraphs filtermultipoles

dbg: filtergraphs_dbg filtermultipoles_dbg


filtergraphs: filtergraphs.cpp
	$(COMPILE) filtergraphs.cpp -o filtergraphs $(LDFLAGS)

filtergraphs_dbg: filtergraphs.cpp
	$(COMPILE_DBG) filtergraphs.cpp -o filtergraphs_dbg.out $(LDFLAGS)

filtermultipoles: filtermultipoles.cpp
	$(COMPILE) filtermultipoles.cpp -o filtermultipoles $(LDFLAGS)

filtermultipoles_dbg: filtermultipoles.cpp
	$(COMPILE_DBG) filtermultipoles.cpp -o filtermultipoles_dbg.out $(LDFLAGS)


clean:
	rm -rf filtergraphs filtergraphs_dbg.out filtermultipoles filtermultipoles_dbg.out


.PHONY: filtergraph filtergraph_dbg filtermultipoles filtermultipoles_dbg
