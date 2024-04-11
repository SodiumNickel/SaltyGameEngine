#ifndef INENGINE
// Defines portions of code only availible in engine and not during game
#define INENGINE // Will be removed in game builds
#endif

/* -------------------------------------- */

#ifdef INENGINE
#ifndef INENGINE_H
#define INENGINE_H

enum ComponentVars {
    POSITION_X,
    POSITION_Y,
    SCALE_X,
    SCALE_Y,
    ROTATION
};

#endif
#endif