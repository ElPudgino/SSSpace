CFLAGS = -std=c++17
#CFLAGS += -I includes
LDFLAGS = -lSDL3 -lvulkan 

OBJ_DIR := objs
OBJECTS := $(wildcard $(OBJ_DIR)/*.o)
SHADERS := $(wildcard materials/*/*.c)

all : CompileApp

CompileApp : $(OBJECTS)
	g++ $(CFLAGS) $(OBJECTS) -o app $(LDFLAGS)

$(OBJ_DIR)/%.o : %.c
	g++ $(CFLAGS) $< -o objs/$@ -c $(LDFLAGS)

$(OBJ_DIR)/Shaders.o : $(SHADERS)
	g++ $(CFLAGS) $< -o objs/$@ -c $(LDFLAGS)

run : CompileApp
	./app
	
cleanup :
	rm -f app $(app_main)