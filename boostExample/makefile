CC=g++
LIBS=/usr/local/boost_1_65_0
CFLAGS=-g -W -Wall -std=c++11
DEPS=/usr/local/boost_1_65_0/stage/lib/libboost_system.a
OBJS=server.o client.o sync_timer.o

all: $(OBJS)

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -I $(LIBS) -o $@ $< $(DEPS)

clean:
	rm $(OBJS)
