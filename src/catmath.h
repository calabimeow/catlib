#ifndef CATMATH_H
#define CATMATH_H

#include "catlib.h"

CATAPI float lerp(float start, float end, float amount);
CATAPI float clamp(float value, float min, float max);

CATAPI vec2 vec2_lerp(vec2 start, vec2 end, float amount);
CATAPI vec2 vec2_add(vec2 v1, vec2 v2);
CATAPI vec2 vec2_sub(vec2 v1, vec2 v2);
CATAPI vec2 vec2_mul(vec2 v1, vec2 v2);
CATAPI vec2 vec2_div(vec2 v1, vec2 v2);
CATAPI vec2 vec2_scale(vec2 v, float value);
CATAPI vec2 vec2_normalize(vec2 v);
CATAPI vec2 vec2_negate(vec2 v);
CATAPI float vec2_dot(vec2 v1, vec2 v2);
CATAPI float vec2_cross(vec2 v1, vec2 v2);
CATAPI float vec2_length(vec2 v);
CATAPI float vec2_dist(vec2 v1, vec2 v2);

CATAPI vec3 vec3_lerp(vec3 start, vec3 end, float amount);
CATAPI vec3 vec3_add(vec3 v1, vec3 v2);
CATAPI vec3 vec3_sub(vec3 v1, vec3 v2);
CATAPI vec3 vec3_mul(vec3 v1, vec3 v2);
CATAPI vec3 vec3_div(vec3 v1, vec3 v2);
CATAPI vec3 vec3_scale(vec3 v, float value);
CATAPI vec3 vec3_normalize(vec3 v);
CATAPI vec3 vec3_negate(vec3 v);
CATAPI float vec3_dot(vec3 v1, vec3 v2);
CATAPI vec3 vec3_cross(vec3 v1, vec3 v2);
CATAPI float vec3_length(vec3 v);
CATAPI float vec3_dist(vec3 v1, vec3 v2);

#endif
