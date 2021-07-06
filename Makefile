CFLAGS=-std=c++17 -Wall -Wextra -pedantic -O2
HEADERS=model.hpp random.hpp domain.hpp
SFML=-lsfml-system -lsfml-window -lsfml-graphics

all: ascii.out tiles.out overlapping.out

overlapping.out: wfc_overlapping.cpp $(HEADERS)
	g++ $(CFLAGS) wfc_overlapping.cpp -o overlapping.out $(SFML)

tiles.out: wfc_tiles.cpp $(HEADERS)
	g++ $(CFLAGS) wfc_tiles.cpp -o tiles.out $(SFML)

ascii.out: wfc_ascii.cpp $(HEADERS)
	g++ $(CFLAGS) wfc_ascii.cpp -o ascii.out

clean:
	rm -rf *.out
