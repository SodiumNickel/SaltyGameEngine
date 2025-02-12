# Salty Style Guide (C++)

## Naming Conventions
```C++
// Classes use PascalCase
class ClassName {
    private:
        // Variables use camelCase
        int variableName;
        // Constants are prefixed by 'k'
        const int kConstantName = 2;
    public:
        // Functions use PascalCase
        void FunctionName();
};

// Structs use Pascal Case
struct StructName {};

// Interfaces are prefixed by 'I'
class IInterface {
public:
    virtual void Begin() = 0;    
};

// Templates are prefixed by 'T'
template <typename TTypeName> ...

// Enumerators are prefixed by 'E'
enum EColor {
    // Enum internals are in CAPS
    RED, 
    GREEN, 
    BLUE, 
    ALPHA
};

// nlohmann::json objects are prefixed by 'J' 
json JVariableName;
```

## Header Files
dir/foo.h
```C++
#ifndef FOO_H
#define FOO_H
...
#endif // FOO_H
```
The #define guard is in CAPS and of the form FILENAME_H. A comment containing the guard should also be left after the endif for clarity.

### Order of Includes
dir/foo.cpp
```C++
#include "foo.h"

#include <iostream>
#include <vector>

#include <SDL.h>
#include <imgui.h>

#include "Engine/Engine.h"

#include "Game/Game.h"
```
1. dir/foo.h
2. *blank line*
3. C++ standard library headers (in alphabetical order)
4. *blank line*
5. Other libraries (in the order SDL, DearImGUI, glm, nlohmann)
6. *blank line*
7. Engine/.. header files (in alphabetical order)
8. *blank line*
9. Game/.. header files (order defined below)

Add *2 blank lines* after the list of includes (even when there are 0 includes).

Order for 9:
1. ECS (TODO: currently paired with json helper bc only used in stage, may remove jsonhelper)
2. Components (Transform, Sprite, Rigidbody, BoxCollider)
3. Systems
4. Asset Manager (TODO: dk if i like asset manager here)
5. Events (TODO)

### Some Notes
All src header includes should contain full path from src.
```C++
#include "Engine/History/EditHistory.h"
```

Include what you use, do not rely on transitive inclusions.

## Namespaces
ONLY used for nlohmann::json in this project, insert the *using* right below the include.
```C++
#include <json.hpp>
using json = nlohmann::json;
```

## Comments
TODO

## Formatting
TODO