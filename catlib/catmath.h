#ifndef CATMATH_H
#define CATMATH_H

#include "catlib.h"

CATAPI float lerp(float start, float end, float amount);
CATAPI float clamp(float value, float min, float max);

//VECTORS
CATAPI vec2 vec2_lerp(vec2 start, vec2 end, float amount);
CATAPI vec2 vec2_add(vec2 v1, vec2 v2);
CATAPI vec2 vec2_sub(vec2 v1, vec2 v2);
CATAPI vec2 vec2_mul(vec2 v1, vec2 v2);
CATAPI vec2 vec2_div(vec2 v1, vec2 v2);
CATAPI vec2 vec2_scale(vec2 v, float value);
CATAPI float vec2_dot(vec2 v1, vec2 v2);

CATAPI vec3 vec3_lerp(vec3 start, vec3 end, float amount);
CATAPI vec3 vec3_add(vec3 v1, vec3 v2);
CATAPI vec3 vec3_sub(vec3 v1, vec3 v2);
CATAPI vec3 vec3_mul(vec3 v1, vec3 v2);
CATAPI vec3 vec3_div(vec3 v1, vec3 v2);
CATAPI vec3 vec3_scale(vec3 v, float value);
CATAPI float vec3_dot(vec3 v1, vec3 v2);

#endif
