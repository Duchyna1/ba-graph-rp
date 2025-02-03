CFLAGS = -std=c++20 -fopenmp -I../include
LDFLAGS =
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG
COMPILE_DBG = $(CXX) $(CFLAGS) $(DBGFLAGS)
COMPILE = $(CXX) $(CFLAGS) -DNDEBUG -O3

test:
	$(COMPILE) test_acyclic_colouring.cpp -o test_acyclic_colouring $(LDFLAGS)
	./test_acyclic_colouring

clean:
	rm -rf *.out