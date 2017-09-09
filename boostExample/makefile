CC=g++
LIBS=/usr/local/boost_1_65_0
CFLAGS=-g -W -Wall -std=c++11
DEPS=/usr/local/boost_1_65_0/stage/lib/libboost_system.a /usr/local/boost_1_65_0/stage/lib/libboost_thread.a
OBJS=server.o client.o sync_timer.o async_timer.o async_timer_arguments.o async_timer_member.o \
async_timer_multhread.o

all: $(OBJS)

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -I $(LIBS) -o $@ $< $(DEPS)

clean:
	rm $(OBJS)
