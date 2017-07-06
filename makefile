EXE = a3search
OBJS = porter2_stemmer.o
CC = g++ -std=c++11 -Wall -pedantic
CCOPTS = -O3
LINKER = g++ -std=c++11 -Wall -pedantic

all: $(EXE)

$(EXE): $(OBJS) a3search.cpp
	$(LINKER) a3search.cpp -o $@ $(OBJS)

porter2_stemmer.o: porter2_stemmer.tar
	tar xvf porter2_stemmer.tar
	$(CC) $(CCOPTS) -c porter2_stemmer.cpp -o $@

clean:
	rm -f *.o $(EXE)
