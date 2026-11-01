g++ -std=c++26 -c pipeline.cpp -o pipeline.o
g++ -std=c++20 -c main.c -o main.o
g++ main.o pipeline.o -o main
# TD: Do the makefile