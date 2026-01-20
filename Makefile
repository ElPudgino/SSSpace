CFLAGS = -std=c++17
#CFLAGS += -Imaterials
LDFLAGS = -lSDL3 -lvulkan 

MATS_DIR := materials
SOURCES := $(wildcard *.c)
SHADERS := $(wildcard $(MATS_DIR)/*/*.c)

all : CompileApp

CompileApp : $(SOURCES) $(SHADERS)
	g++ $(CFLAGS) $(SOURCES) $(SHADERS) -o app $(LDFLAGS)

run : CompileApp
	export VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_validation
	./app
	
cleanup :
	rm -f app 