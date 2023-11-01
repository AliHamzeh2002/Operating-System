CC := gcc
OBJDIR := objects

all: customer.out restaurant.out supplier.out

restaurant.out: $(OBJDIR)/restaurant.o $(OBJDIR)/user.o $(OBJDIR)/udp.o $(OBJDIR)/utils.o $(OBJDIR)/tcp.o $(OBJDIR)/cJSON.o
	$(CC) $(OBJDIR)/restaurant.o $(OBJDIR)/user.o $(OBJDIR)/udp.o $(OBJDIR)/utils.o $(OBJDIR)/tcp.o $(OBJDIR)/cJSON.o -o restaurant.out

supplier.out: $(OBJDIR)/supplier.o $(OBJDIR)/user.o $(OBJDIR)/udp.o $(OBJDIR)/utils.o $(OBJDIR)/tcp.o $(OBJDIR)/cJSON.o
	$(CC) $(OBJDIR)/supplier.o $(OBJDIR)/user.o $(OBJDIR)/udp.o $(OBJDIR)/utils.o $(OBJDIR)/tcp.o $(OBJDIR)/cJSON.o -o supplier.out

customer.out: $(OBJDIR)/customer.o $(OBJDIR)/user.o $(OBJDIR)/udp.o $(OBJDIR)/utils.o $(OBJDIR)/tcp.o $(OBJDIR)/cJSON.o
	$(CC) $(OBJDIR)/customer.o $(OBJDIR)/user.o $(OBJDIR)/udp.o $(OBJDIR)/utils.o $(OBJDIR)/tcp.o $(OBJDIR)/cJSON.o -o customer.out

$(OBJDIR)/restaurant.o: restaurant.c user.c restaurant.h user.h 
	$(CC) -c restaurant.c -o $(OBJDIR)/restaurant.o

$(OBJDIR)/supplier.o: supplier.c user.c supplier.h user.h 
	$(CC) -c supplier.c -o $(OBJDIR)/supplier.o

$(OBJDIR)/customer.o: customer.c user.c customer.h user.h 
	$(CC) -c customer.c -o $(OBJDIR)/customer.o

$(OBJDIR)/user.o: user.c tcp.c udp.c utils.c user.h tcp.h udp.h utils.h 
	$(CC) -c user.c -o $(OBJDIR)/user.o
	
$(OBJDIR)/utils.o: utils.c utils.h consts.h
	$(CC) -c utils.c -o $(OBJDIR)/utils.o

$(OBJDIR)/cJSON.o: cJSON.c cJSON.h
	$(CC) -c cJSON.c -o $(OBJDIR)/cJSON.o

$(OBJDIR)/udp.o: udp.c udp.h consts.h
	$(CC) -c udp.c -o $(OBJDIR)/udp.o

$(OBJDIR)/tcp.o: tcp.c tcp.h consts.h
	$(CC) -c tcp.c -o $(OBJDIR)/tcp.o

.PHONY: clean

clean:
	rm -r *.o
