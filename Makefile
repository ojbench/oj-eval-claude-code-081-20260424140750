CC = gcc
CXX = g++
CFLAGS = -O3 -Wall
CXXFLAGS = -O3 -Wall

all: code

code: code.cpp
	$(CXX) $(CXXFLAGS) code.cpp -o code

clean:
	rm -f code
