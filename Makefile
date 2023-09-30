CC := g++ -O2 -std=c++20
main: main.o GameState.o GameSettings.o coord.o BinaryLotMachine.o
	$(CC) GameState.o GameSettings.o BinaryLotMachine.o coord.o main.o -o main

main.o: main.cpp GameState.hpp GameSettings.hpp
	$(CC) main.cpp -c

GameState.o: GameState.hpp GameState.cpp coord.hpp GameSettings.hpp
	$(CC) -c GameState.cpp -c

GameSettings.o: GameSettings.hpp GameSettings.cpp coord.hpp
	$(CC) -c GameSettings.cpp -c

coord.o: coord.cpp coord.hpp
	$(CC) -c coord.cpp

BinaryLotMachine.o: BinaryLotMachine.hpp BinaryLotMachine.cpp
	$(CC) BinaryLotMachine.cpp -c