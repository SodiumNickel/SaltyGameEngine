# Engine builds
ENGINE_NAME = saltyengine
BUILD_DIR = build

INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/imgui -Iinclude/glm -Iinclude/nlohmann -Isrc

LIB_DIRS = -Llib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

SRC = src/enginemain.cpp \
	  $(wildcard imgui/*.cpp) \
	  src/Engine/Engine.cpp \
	  src/Engine/Stage/*.cpp src/Engine/Menu/*.cpp src/Engine/Tabs/*.cpp \
	  src/Engine/History/*.cpp src/Engine/History/Edits/*.cpp src/Engine/Debug/*.cpp \
	  src/Game/Game.cpp \
	  src/Game/ECS/*.cpp src/Game/AssetManager/*.cpp \
	  src/Game/Helpers/*.cpp \

# -fsanitize=address 
default: # Compiles engine
	g++ -g $(SRC) -std=c++17 $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) -o $(BUILD_DIR)/$(ENGINE_NAME)

run:
	cd $(BUILD_DIR) && $(ENGINE_NAME)

debug:
	cd $(BUILD_DIR) && gdb ./$(ENGINE_NAME)

clean:
	cd $(BUILD_DIR) && del $(ENGINE_NAME).exe && del imgui.ini;


# Game builds - for testing commands to be called by std::system()
G_NAME = game
G_DIR = game-build

G_INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/glm -Iinclude/nlohmann -Isrc/Game

G_LIB_DIRS = -Llib
G_LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

G_SRC = src/main.cpp \
	  	src/Game/Game.cpp \
	  	src/Game/ECS/*.cpp src/Game/AssetManager/*.cpp \
	  	src/Game/Helpers/*.cpp \

game: # Compiles game
	g++ -g $(G_SRC) -std=c++17 $(G_INCLUDE_DIRS) $(G_LIB_DIRS) $(G_LIBS) -o $(G_BUILD_DIR)/$(G_ENGINE_NAME)