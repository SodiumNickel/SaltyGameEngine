# TEMP
# EXTRA = -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc

# Engine builds
ENGINE_NAME = saltyengine
BUILD_DIR = build

INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/imgui -Iinclude/glm -Iinclude/nlohmann -Iinclude/soloud -Ilibsrc/soloud/wav -Isrc

LIB_DIRS = -Llib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

# TODO: a lot of these folders just have 1 .h and 1. cpp file, could just write it out tbh...
SRC = src/enginemain.cpp \
	  libsrc/imgui/*.cpp \
	  libsrc/soloud/core/*.cpp libsrc/soloud/sdl2_static/*.cpp libsrc/soloud/wav/*.cpp libsrc/soloud/wav/stb_vorbis.c \
	  src/Engine/Engine.cpp \
	  src/Engine/Stage/*.cpp src/Engine/Tabs/*.cpp src/Engine/Menu/*.cpp \
	  src/Engine/History/*.cpp src/Engine/History/Edits/*.cpp \
	  src/Game/Game.cpp \
	  src/Game/ECS/*.cpp src/Game/AssetManager/*.cpp \
	  src/Game/Salty/*.cpp \

# Compiles SoLoud statically with SDL2
FLAGS = -DWITH_SDL2_STATIC

# TODO: Supress console window  -mwindows


# TODO: consider optimization levels like -O1 or -O2
# -fsanitize=address 
default: # Compiles engine
	g++ -g $(SRC) -std=c++17 $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) $(FLAGS) -o $(BUILD_DIR)/$(ENGINE_NAME)
	xcopy /E /I /Y .\src\Game .\$(BUILD_DIR)\Make\src\Game > nul
	xcopy /E /I /Y .\src\main.cpp .\$(BUILD_DIR)\Make\src > nul
	xcopy /E /I /Y .\src\webmain.cpp .\$(BUILD_DIR)\Make\src > nul
	xcopy /E /I /Y .\src\SaltyEngine.h .\$(BUILD_DIR)\Make\src > nul
# TODO: this needs to clear it too, in case of moving stuff around

run:
	cd $(BUILD_DIR) && $(ENGINE_NAME)

debug:
	cd $(BUILD_DIR) && gdb ./$(ENGINE_NAME)

clean:
	cd $(BUILD_DIR) && del $(ENGINE_NAME).exe && del imgui.ini;

copygame:
	xcopy /E /I /Y .\src\Game .\$(BUILD_DIR)\Make\src\Game > nul

# To compile SoLoud
SOL_SRC = libsrc/soloud/core/*.cpp libsrc/soloud/sdl2_static/*.cpp libsrc/soloud/wav/*.cpp libsrc/soloud/wav/stb_vorbis.c 
SOL_INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/soloud -Ilibsrc/soloud/wav

SOL_FLAGS = -DWITH_SDL2_STATIC -lstdc++

# Creates and indexes libsoloud static
libsoloud.a: 
	g++ -c $(SOL_SRC) -std=c++17 $(SOL_INCLUDE_DIRS) $(SOL_FLAGS)
	ar rcs lib/libsoloud.a *.o
	ranlib lib/libsoloud.a
	del *.o

cleansoloud: 
	del *.o

# Game builds - for testing commands to be called by std::system()
G_NAME = game
G_DIR = game-build
W_DIR = gweb-build

# TODO: note
# NOTE: I include the entire source here so the headers have the right path, but should be restricted in actual engine
G_INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/glm -Iinclude/nlohmann -Iinclude/soloud -Ilibsrc/soloud/wav -Isrc

G_LIB_DIRS = -Llib
G_LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

G_SRC = src/main.cpp \
	  	src/Game/Game.cpp \
	  	$(wildcard src/Game/ECS/*.cpp) $(wildcard src/Game/AssetManager/*.cpp) \
	  	$(wildcard src/Game/Helpers/*.cpp) $(wildcard src/Game/Salty/*.cpp) \

# em++ won't compile unless space seperated (hence wildcard)
W_SRC = src/webmain.cpp \
	  	src/Game/Game.cpp \
	  	$(wildcard src/Game/ECS/*.cpp) $(wildcard src/Game/AssetManager/*.cpp) \
	  	$(wildcard src/Game/Helpers/*.cpp) $(wildcard src/Game/Salty/*.cpp) \
		$(wildcard libsrc/soloud/core/*.cpp) $(wildcard libsrc/soloud/sdl2_static/*.cpp) \
		$(wildcard libsrc/soloud/wav/*.cpp) libsrc/soloud/wav/stb_vorbis.c \

# TODO: do i need SDL2_IMAGE_FORMATS?
W_FLAGS = -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["jpg","png"]' -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH=1
SL_FLAGS = -DWITH_SDL2_STATIC
# Load Unique folder into the virtual filesystem Unique dir (preserves filepaths)
W_PRELOAD = --preload-file $(BUILD_DIR)/Unique@Unique --use-preload-plugins
# TODO: decide between embed and preload, browser games shouldnt have too many files...
# W_EMBED = 
# Preload: Good for large assets, reduces initial download size, loads files at runtime.
# Embed: Good for small assets, immediate access, increases initial download size.

# TODO: remember not to include -g and to include -O2 in actual compilation
g: # Compiles game
	g++ -g $(G_SRC) -std=c++17 $(G_INCLUDE_DIRS) $(G_LIB_DIRS) $(G_LIBS) -o $(G_DIR)/$(G_NAME)

grun:
	cd $(G_DIR) && $(G_NAME)

gdebug:
	cd $(G_DIR) && gdb ./$(G_NAME)

gweb: # Compiles game for web
	em++ $(W_SRC) -std=c++17 $(G_INCLUDE_DIRS) $(W_FLAGS) $(SL_FLAGS) $(W_PRELOAD) -o $(W_DIR)/index.html
# TODO: remember to make this only updates json later