FLAGS = -std=c++20 -c
DBGFLAGS = -std=c++20 -g -O0 -Wall -Wextra -c

main: pipeline.o main.o
	g++ main.o pipeline.o -o main -lboost_iostreams

pipeline.o: pipeline.cpp pipeline.hpp instruction.hpp
	g++ $(FLAGS) pipeline.cpp -o pipeline.o

main.o: main.cpp pipeline.hpp instruction.hpp
	g++ $(FLAGS) main.cpp -o main.o

clean:
	rm -f main.o pipeline.o main


main_dbg: main_dbg.o pipeline_dbg.o
	g++ main_dbg.o pipeline_dbg.o -o main_dbg -lboost_iostreams

main_dbg.o: main.cpp pipeline.hpp instruction.hpp
	g++ $(DBGFLAGS) main.cpp -o main_dbg.o

pipeline_dbg.o: pipeline.cpp pipeline.hpp instruction.hpp
	g++ $(DBGFLAGS) pipeline.cpp -o pipeline_dbg.o

clean_dbg:
	rm -f main_dbg.o pipeline_dbg.o main_dbg

.PHONY:
	clean clean_dbg