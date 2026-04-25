# Compiler and tools
CC     := gcc
CXX    := g++
GLSLC  := glslc

# Flags – adjust to your needs
CFLAGS   := -Wall -Wextra -O2
CXXFLAGS := -Wall -Wextra -O2
LDFLAGS  := -lm -lSDL3 -lvulkan -lstdc++ -lm  # example; add Vulkan libs etc.

# Target executable name
TARGET := app

DEBUG := 0

# ---- Source files ----
# All .c files in the project root
SRCS       := $(wildcard *.c)
# vma.cpp (compiled as C++)
VMA_SRC    := vma.cpp
# .c files inside materials subfolders
MAT_SRCS   := $(wildcard materials/*/*.c)

# All object files (placed next to their source)
OBJS := $(SRCS:.c=.o) $(MAT_SRCS:.c=.o) $(VMA_SRC:.cpp=.o)

# ---- Shader compilation ----
# Every materials subfolder contains a .c and optionally .vert/.frag named as the folder
FOLDERS := $(notdir $(wildcard materials/*))
# Build the list of desired .spv outputs if the corresponding shader source exists
SPV_TARGETS := $(foreach f,$(FOLDERS),\
                 $(if $(wildcard materials/$f/$f.vert),spvs/$f.vert.spv)\
                 $(if $(wildcard materials/$f/$f.frag),spvs/$f.frag.spv))

# Default goal
all: DEBUG = 0
all: $(TARGET) $(SPV_TARGETS)

Debug: DEBUG = 1
Debug: $(TARGET) $(SPV_TARGETS)

# ---- Linking ----
# Link with g++ because vma.o is C++ and may need libstdc++
$(TARGET): $(OBJS)
	$(CXX) -DDEBUG=$(DEBUG) $^ -o $@ $(LDFLAGS)

# ---- C compilation ----
# Pattern rule for every .c -> .o (works for root and materials/ subdirs)
%.o: %.c
	$(CC) -DDEBUG=$(DEBUG) -c $(CFLAGS) $< -o $@

# ---- C++ compilation (vma.cpp only) ----
vma.o: vma.cpp
	$(CXX) -DDEBUG=$(DEBUG) -c $(CXXFLAGS) $< -o $@

# ---- Shader rules ----
# Ensure spvs/ directory exists (order-only prerequisite)
spvs:
	mkdir -p spvs

# Template to define a rule for one folder and one shader extension
define shader_rule
spvs/$(1).$(2).spv: materials/$(1)/$(1).$(2) | spvs
	$$(GLSLC) $$< -o $$@
endef

# Evaluate a rule for each folder that has a .vert
$(foreach f,$(FOLDERS),\
  $(if $(wildcard materials/$f/$f.vert),\
    $(eval $(call shader_rule,$f,vert))))

# Evaluate a rule for each folder that has a .frag
$(foreach f,$(FOLDERS),\
  $(if $(wildcard materials/$f/$f.frag),\
    $(eval $(call shader_rule,$f,frag))))

# ---- Housekeeping ----
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf spvs