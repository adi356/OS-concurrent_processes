cc = gcc
CFLAGS = -std=gnu99
TARGET = reverse
OBJS = master.o slave.o 
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)
master.o: master.c 
	$(CC) $(CFLAGS) -c master.c 
slave.o: slave.c 
	$(CC) $(CFlAGS) -c slave.c 
clean:
	/bin/rm -f *.o 
clean_all:
	/bin/rm -f *.o $(TARGET)