FLAGS = -std=c++20 -c -MP -MDD
DBGFLAGS = -std=c++20 -g -O0 -Wall -Wextra -c -MP -MDD

OBJ:
	mkdir -p build/obj
	OBJ = build/obj

BIN:
	mkdir -p build/bin

$(BIN)/simulator: $(OBJ)/%
	g++ $(OBJ)/% -o $@ -lboost_iostreams

pipeline.o: pipeline.cpp pipeline.hpp instruction.hpp
	g++ $(FLAGS) pipeline.cpp -o pipeline.o

simulator.o: simulator.cpp pipeline.hpp instruction.hpp
	g++ $(FLAGS) simulator.cpp -o simulator.o

gshare.o: gshare.cpp gshare.hpp
	g++ $(FLAGS) gshare.cpp -o gshare.o

clean:
	rm -f simulator.o pipeline.o gshare.o simulator


simulator_dbg: simulator_dbg.o pipeline_dbg.o
	g++ simulator_dbg.o pipeline_dbg.o -o simulator_dbg -lboost_iostreams

simulator_dbg.o: simulator.cpp pipeline.hpp instruction.hpp
	g++ $(DBGFLAGS) simulator.cpp -o simulator_dbg.o

pipeline_dbg.o: pipeline.cpp pipeline.hpp instruction.hpp
	g++ $(DBGFLAGS) pipeline.cpp -o pipeline_dbg.o

clean_dbg:
	rm -f simulator_dbg.o pipeline_dbg.o simulator_dbg

.PHONY:
	clean clean_dbg