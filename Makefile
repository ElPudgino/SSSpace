CFLAGS = -std=c++17
#CFLAGS += -I includes
LDFLAGS = -lSDL3 -lvulkan

all : CompileApp

CompileApp : engine.o engine_init.o support_check.o swapchain_init.o engine_utils.o render_passes.o
	g++ $(CFLAGS) engine.o engine_init.o support_check.o swapchain_init.o engine_utils.o render_passes.o -o app $(LDFLAGS)

engine.o : engine.c
	g++ $(CFLAGS) engine.c -o engine.o -c $(LDFLAGS)

engine_init.o : engine_init.c
	g++ $(CFLAGS) engine_init.c -o engine_init.o -c $(LDFLAGS)

support_check.o : support_check.c 
	g++ $(CFLAGS) support_check.c -o support_check.o -c $(LDFLAGS)

swapchain_init.o : swapchain_init.c 
	g++ $(CFLAGS) swapchain_init.c -o swapchain_init.o -c $(LDFLAGS)

engine_utils.o : engine_utils.c
	g++ $(CFLAGS) engine_utils.c -o engine_utils.o -c $(LDFLAGS)

render_passes.o : render_passes.c
	g++ $(CFLAGS) render_passes.c -o render_passes.o -c $(LDFLAGS)

run : CompileApp
	./app
	
cleanup :
	rm -f app engine_init.o engine.o support_check.o swapchain_init.o engine_utils.o render_passes.o