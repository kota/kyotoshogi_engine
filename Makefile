CC = gcc
OBJS = main.o board.o ini.o data.o io.o search.o

OPT = -DNDEBUG

kyoto: $(OBJS)
	$(CC) -o $@ $(OBJS) -lm

.c.o:
	$(CC) -c $< $(OPT) -Wall -O3

$(OBJS): header.h

clean:
	rm*.o
	rm kyoto.exe
	rm kyoto

