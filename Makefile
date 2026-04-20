#CFLAGS = -std=c11 #-fsanitize=address
#CFLAGS += -Imaterials
LDFLAGS = -lSDL3 -lvulkan -lstdc++ -lm

MATS_DIR := materials
SOURCES := $(wildcard *.c)
SHADERS := $(wildcard $(MATS_DIR)/*/*.c)

all : CompileApp

vma.o : vma.cpp
	g++ $(CFLAGS) -c vma.cpp -o vma.o $(LDFLAGS)

CompileApp : vma.o $(SOURCES) $(SHADERS)
	bash materials/CompileShaders.sh
	gcc -DDEBUG=0 $(CFLAGS) $(SOURCES) $(SHADERS) vma.o -o app $(LDFLAGS)

run : CompileApp
	export VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_validation
	./app

debug : $(SOURCES) $(SHADERS)
	bash materials/CompileShaders.sh
	gcc -DDEBUG=1 $(CFLAGS) vma.o $(SOURCES) $(SHADERS) -o appd $(LDFLAGS)
	
rund : debug
	export VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_validation
	./appd

cleanup :
	rm -f app 