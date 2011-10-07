CC=g++
CFLAGS=-I. -O2 -Wall -pedantic
BASE_HEADERS=base/timer.h
SORTERS_HEADERS=sorters/sorter_interface.h sorters/stl_sorters.h
GENERATORS_HEADERS=generators/generator_interface.h \
			generators/random_generator.h


all: tester

tester: tester.o timer.o
	g++ tester.o timer.o -o tester

tester.o: tester.cc $(BASE_HEADERS) $(SORTERS_HEADERS) $(GENERATORS_HEADERS)
	g++ $(CFLAGS) -c tester.cc

timer.o: base/timer.cc base/timer.h
	g++ $(CFLAGS) -c base/timer.cc base/timer.h

clean:
	rm tester *.o

.PHONY: clean
