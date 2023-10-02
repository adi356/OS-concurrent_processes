cc = gcc
CFLAGS = -std=gnu99
TARGET = master
OBJS = master.o slave.o 
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)
master.o: master.c config.h
	$(CC) $(CFLAGS) -c master.c 
slave.o: slave.c config.h
	$(CC) $(CFLAGS) -c slave.c 
clean:
	/bin/rm -f *.o 
clean_all:
	/bin/rm -f *.o $(TARGET)