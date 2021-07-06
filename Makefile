CFLAGS=-std=c++17 -Wall -Wextra -pedantic -O2
HEADERS=model.hpp random.hpp domain.hpp
SFML=-lsfml-system -lsfml-window -lsfml-graphics

all: ascii.out overlapping.out

ascii.out: wfc_ascii.cpp $(HEADERS)
	g++ $(CFLAGS) wfc_ascii.cpp -o ascii.out

overlapping.out: wfc_overlapping.cpp $(HEADERS)
	g++ $(CFLAGS) wfc_overlapping.cpp -o overlapping.out $(SFML)

clean:
	rm -rf *.out
