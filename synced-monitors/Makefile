PLUGIN_NAME=sync-monitors

SOURCE_FILES=$(wildcard src/*.cpp)

COMPILE_FLAGS=-shared -g -fPIC --no-gnu-unique -std=c++23 -Wall
COMPILE_FLAGS+=-Iinclude
COMPILE_FLAGS+=-DWLR_USE_UNSTABLE
INCLUDES = `pkg-config --cflags pixman-1 libdrm hyprland libinput wayland-server xkbcommon`

all: $(PLUGIN_NAME).so

$(PLUGIN_NAME).so: $(SOURCE_FILES) $(INCLUDE_FILES)
	g++ -O2 $(COMPILE_FLAGS) $(INCLUDES) $(COMPILE_DEFINES) $(SOURCE_FILES) -o $(PLUGIN_NAME).so

debug: $(SOURCE_FILES) $(INCLUDE_FILES)
	g++ -DDEBUG $(COMPILE_FLAGS) $(INCLUDES) $(COMPILE_DEFINES) $(SOURCE_FILES) -o $(PLUGIN_NAME).so

clean:
	rm -f ./$(PLUGIN_NAME).so

install: $(PLUGIN_NAME).so
	cp $(PLUGIN_NAME).so ${HOME}/.local/share/hyprpm/${PLUGIN_NAME}/${PLUGIN_NAME}.so