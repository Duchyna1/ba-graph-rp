CFLAGS = -std=c++20 -fopenmp -I../include -I../thirdparty/cryptominisat/src
LDFLAGS = -L/usr/local/lib
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG
COMPILE_DBG = $(CXX) $(CFLAGS) $(DBGFLAGS)
COMPILE = $(CXX) $(CFLAGS) -DNDEBUG -O3
CMSAT_FLAGS = -D COMPILE_WITH_CRYPTOMINISAT -l cryptominisat5

test:
	$(COMPILE) test_acyclic_colouring.cpp -DLARGEMEM=ON -o test_acyclic_colouring $(LDFLAGS) $(CMSAT_FLAGS)

clean:
	rm -rf *.out