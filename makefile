FLAGS = -std=c++20 -c -MP -MMD

BIN = build/bin
OBJ = build/obj

all: $(BIN)/simulator

$(BIN)/simulator: $(OBJ)/simulator.o $(OBJ)/pipeline.o $(OBJ)/gshare.o | $(BIN)
	g++ $^ -o $@ -lboost_iostreams

$(OBJ)/%.o: src/%.cpp | $(OBJ)
	g++ $(FLAGS) $< -o $@

$(OBJ) $(BIN):
	mkdir -p $@

-include $(OBJ)/simulator.d $(OBJ)/pipeline.d $(OBJ)/gshare.d

debug:
	make FLAGS="$(FLAGS) -g -O0 -Wall -Wextra"

clean:
	rm -rf build

.PHONY:
	clean debug