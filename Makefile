CC = gcc
CFLAGS = -std=gnu99
TARGETS = master slave
OBJS = master.o slave.o 

all: $(TARGETS)

master: master.o 
	$(CC) -o master master.o 
slave: slave.o 
	$(CC) -o slave slave.o
master.o: master.c config.h
	$(CC) $(CFLAGS) -c master.c 
slave.o: slave.c config.h
	$(CC) $(CFLAGS) -c slave.c 
clean:
	/bin/rm -f *.o 
clean_all:
	/bin/rm -f *.o $(TARGETS)