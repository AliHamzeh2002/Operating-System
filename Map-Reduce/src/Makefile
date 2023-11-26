CC := g++
OBJDIR := objects

all: main.out building.out resource.out bills.out

main.out: $(OBJDIR)/main.o $(OBJDIR)/logger.o 
	$(CC) $(OBJDIR)/main.o $(OBJDIR)/logger.o -o main.out

building.out: $(OBJDIR)/building.o $(OBJDIR)/logger.o 
	$(CC) $(OBJDIR)/building.o $(OBJDIR)/logger.o -o building.out

resource.out: $(OBJDIR)/resource.o $(OBJDIR)/logger.o  $(OBJDIR)/csvhandler.o
	$(CC) $(OBJDIR)/resource.o $(OBJDIR)/logger.o $(OBJDIR)/csvhandler.o -o resource.out

bills.out: $(OBJDIR)/bills.o $(OBJDIR)/logger.o $(OBJDIR)/csvhandler.o
	$(CC) $(OBJDIR)/bills.o $(OBJDIR)/logger.o $(OBJDIR)/csvhandler.o -o bills.out

$(OBJDIR)/main.o: main.cpp logger.cpp utils.cpp logger.hpp
	$(CC) -c main.cpp -o $(OBJDIR)/main.o

$(OBJDIR)/building.o: building.cpp logger.cpp utils.cpp logger.hpp
	$(CC) -c building.cpp -o $(OBJDIR)/building.o

$(OBJDIR)/resource.o: resource.cpp logger.cpp utils.cpp csvhandler.cpp logger.hpp csvhandler.hpp
	$(CC) -c resource.cpp -o $(OBJDIR)/resource.o

$(OBJDIR)/bills.o: bills.cpp logger.cpp csvhandler.cpp logger.hpp csvhandler.hpp
	$(CC) -c bills.cpp -o $(OBJDIR)/bills.o

$(OBJDIR)/csvhandler.o: csvhandler.cpp csvhandler.hpp
	$(CC) -c csvhandler.cpp -o $(OBJDIR)/csvhandler.o

$(OBJDIR)/logger.o: logger.cpp logger.hpp color.hpp 
	$(CC) -c logger.cpp -o $(OBJDIR)/logger.o

.PHONY: clean

clean:
	rm -r *.o
