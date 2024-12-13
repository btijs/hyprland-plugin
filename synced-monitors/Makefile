PLUGIN_NAME=sync-monitors

CXX=g++

# Source files
SOURCES = $(shell find ./src -type f -name "*.cpp")
HEADS = $(shell find ./src -type f -name "*.hpp")

OBJS = $(SRCS:.cpp=.o)
DEPS = Makefile.depend

# Intermediate object files
OBJECT_DIR=obj

# Enable parallel builds
MAKEFLAGS := --jobs=$(shell nproc)
MAKEFLAGS += --output-sync=target

# Compiler flags
COMPILE_FLAGS=-g -fPIC --no-gnu-unique -std=c++23
COMPILE_FLAGS+=-fdiagnostics-color=always
COMPILE_FLAGS+=-Iinclude
INCLUDES = `pkg-config --cflags pixman-1 libdrm hyprland libinput wayland-server xkbcommon`

# Linker flags
LINK_FLAGS=-shared

# Build
all: $(PLUGIN_NAME).so

# Ensure obj directory exists
$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: src/%.cpp $(HEADS) | $(OBJECT_DIR)
	$(CXX) $(COMPILE_FLAGS) $(INCLUDES) -c -o $@ $<

install: all
	cp $(PLUGIN_NAME).so ${HOME}/.local/share/hyprpm/${PLUGIN_NAME}/${PLUGIN_NAME}.so

$(PLUGIN_NAME).so: $(addprefix $(OBJECT_DIR)/, $(notdir $(SOURCES:.cpp=.o)))
	$(CXX) $(LINK_FLAGS) -o $@ $^ $(COMPILE_FLAGS)

clean:
	rm -rf $(OBJECT_DIR)/*.o
	rm -f ./$(PLUGIN_NAME).so