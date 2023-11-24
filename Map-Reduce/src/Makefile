CC := g++
OBJDIR := objects

all: main.out

main.out: $(OBJDIR)/main.o $(OBJDIR)/logger.o 
	$(CC) $(OBJDIR)/main.o $(OBJDIR)/logger.o -o main.out

$(OBJDIR)/main.o: main.cpp logger.cpp logger.hpp
	$(CC) -c main.cpp -o $(OBJDIR)/main.o

$(OBJDIR)/logger.o: logger.cpp logger.hpp color.hpp 
	$(CC) -c logger.cpp -o $(OBJDIR)/logger.o

.PHONY: clean

clean:
	rm -r *.o
