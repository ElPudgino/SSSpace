CFLAGS = -std=c++17 #-fsanitize=address
#CFLAGS += -Imaterials
LDFLAGS = -lSDL3 -lvulkan 

MATS_DIR := materials
SOURCES := $(wildcard *.c)
SHADERS := $(wildcard $(MATS_DIR)/*/*.c)

all : CompileApp

CompileApp : $(SOURCES) $(SHADERS)
	bash materials/CompileShaders.sh
	g++ -DDEBUG=0 $(CFLAGS) $(SOURCES) $(SHADERS) -o app $(LDFLAGS)

run : CompileApp
	export VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_validation
	./app

debug : $(SOURCES) $(SHADERS)
	bash materials/CompileShaders.sh
	g++ -DDEBUG=1 $(CFLAGS) $(SOURCES) $(SHADERS) -o appd $(LDFLAGS)
	
rund : debug
	export VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_validation
	./appd

cleanup :
	rm -f app 