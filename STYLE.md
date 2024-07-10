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
```

## Header Files

#### Order of Includes
dir/foo.cpp
```C++
#include "foo.h"

#include <iostream>
#include <vector>

#include <SDL.h>
#include <imgui.h>


#include "dir2/bar.h"

#include "dir3/
```

## Namespaces
Avoid using namespaces. They are only used for nlohmann::json in this project.