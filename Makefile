CFLAGS = -std=c++17
#CFLAGS += -I includes
LDFLAGS = -lSDL3 -lvulkan

all : engine


engine : engine.c
	g++ $(CFLAGS) engine.c -o engine $(LDFLAGS)


run :
	./engine
	