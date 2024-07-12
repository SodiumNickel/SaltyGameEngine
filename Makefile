PROJECTNAME = saltyengine
BUILD_DIR = build

# -I"include/glm"
INCLUDE_DIRS = -Iinclude/SDL2 -Iinclude/imgui -Iinclude/glm -Iinclude/nlohmann -Isrc \

LIB_DIRS = -Llib

LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

SRC = $(wildcard src/*.cpp) \
	  $(wildcard imgui/*.cpp) \
	  src/Engine/Engine.cpp \
	  src/Engine/Stage/*.cpp src/Engine/Menu/*.cpp src/Engine/Tabs/*.cpp \
	  src/Engine/History/*.cpp src/Engine/History/Edits/*.cpp src/Engine/Debug/*.cpp \
	  src/Game/Game.cpp \
	  src/Game/ECS/*.cpp src/Game/AssetManager/*.cpp \
	  src/Game/Helpers/*.cpp \

# -fsanitize=address 
default: # engine build
	g++ $(SRC) -std=c++17 $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS) -o $(BUILD_DIR)/$(PROJECTNAME)

run:
	cd $(BUILD_DIR) && $(PROJECTNAME)

clean:
	cd $(BUILD_DIR) && del $(PROJECTNAME).exe && del imgui.ini;