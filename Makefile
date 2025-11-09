CFLAGS = -std=c++17
#CFLAGS += -I includes
LDFLAGS = -lSDL3 -lvulkan

app_main = engine.o engine_init.o support_check.o swapchain_init.o image_utils.o render_passes.o pipeline_builder.o descriptors_util.o shader_loader.o render_primitives.o

all : CompileApp

CompileApp : $(app_main)
	g++ $(CFLAGS) $(app_main) -o app $(LDFLAGS)

engine.o : engine.c
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

engine_init.o : engine_init.c
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

support_check.o : support_check.c 
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

swapchain_init.o : swapchain_init.c 
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

image_utils.o : image_utils.c
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

render_passes.o : render_passes.c
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

pipeline_builder.o : pipeline_builder.c
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

descriptors_util.o : descriptors_util.c
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

render_primitives.o : render_primitives.c
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

shader_loader.o : shader_loader.c
	g++ $(CFLAGS) $< -o $@ -c $(LDFLAGS)

run : CompileApp
	./app
	
cleanup :
	rm -f app $(app_main)