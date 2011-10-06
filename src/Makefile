CC=g++
CFLAGS=-I. -O2 -Wall -pedantic

all: tester

tester: tester.o timer.o
	g++ tester.o timer.o -o tester

tester.o: tester.cc base/timer.h sort/sorter_interface.h sort/stl_sorters.h
	g++ $(CFLAGS) -c tester.cc

timer.o: base/timer.cc base/timer.h
	g++ $(CFLAGS) -c base/timer.cc base/timer.h

clean:
	rm tester *.o

.PHONY: clean
