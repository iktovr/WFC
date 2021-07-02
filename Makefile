CFLAGS=-std=c++17 -Wall -Wextra -pedantic -g

all: main

main: main.cpp random.hpp domain.hpp
	g++ $(CFLAGS) main.cpp -o solution.out

run: main
	./solution.out

clean:
	rm -rf solution.out
