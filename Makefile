CC = g++
CFLAGS = -g -std=c++20

INCLUDES = -I include/

build/main: main.cpp build/dfa.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

build/dfa.o: src/dfa.cpp include/dfa.hpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf build/*.o build/main