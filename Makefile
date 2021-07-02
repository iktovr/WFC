CFLAGS=-std=c++17 -Wall -Wextra -pedantic -g
HEADERS=model.hpp random.hpp domain.hpp

all: ascii

ascii: wfc_ascii.cpp $(HEADERS)
	g++ $(CFLAGS) wfc_ascii.cpp -o ascii.out

clean:
	rm -rf *.out
