FLAGS = -std=c++20 -c

main: pipeline.o main.o
	g++ main.o pipeline.o -o main

pipeline.o: pipeline.cpp pipeline.hpp instruction.hpp
	g++ $(FLAGS) pipeline.cpp -o pipeline.o

main.o: main.cpp pipeline.hpp instruction.hpp
	g++ $(FLAGS) main.cpp -o main.o

run: main
	./main trace_first_run.bin

clean:
	rm -f main.o pipeline.o main

.PHONY:
	run clean