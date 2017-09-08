CC=g++
LIBS=/usr/local/boost_1_65_0
CFLAGS=-g -W -Wall -std=c++11
OBJS=/usr/local/boost_1_65_0/stage/lib/libboost_system.a

all: server client

server: server.cpp
	$(CC) $(CFLAGS) -I $(LIBS) -o $@ $< $(OBJS)

client: client.cpp
	$(CC) $(CFLAGS) -I $(LIBS) -o $@ $< $(OBJS)
	
clean:
	rm server client
