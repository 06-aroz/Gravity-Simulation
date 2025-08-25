#ifndef _COLORS_H
#define _COLORS_H

#include "math.hpp"
#include "types.hpp"
#include <random>

color randomColor();

// Mapping colors based on the speed of the body and a mapping function
color mapColors(Vec2 obj_velocity, color (*Mapper)(float));
color mapPeriodColors(float t, color (*Mapper)(float));

// Current mapping function
color mapCosine(float t);

#endif // !_COLORS_H
