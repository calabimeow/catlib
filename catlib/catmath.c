#define CATLIB_BUILD

#include "catmath.h"

float lerp(float start, float end, float amount)
{
    return (1.0f - amount) * start + amount * end;
}

float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

vec2 vec2_lerp(vec2 start, vec2 end, float amount)
{
    return (vec2)
    {
        (1.0f - amount) * start.x + amount * end.x,
        (1.0f - amount) * start.y + amount * end.y,
    };
}


vec2 vec2_add(vec2 v1, vec2 v2)
{
    return (vec2){v1.x + v2.x, v1.y + v2.y};
}

vec2 vec2_sub(vec2 v1, vec2 v2)
{
    return (vec2){v1.x - v2.x, v1.y - v2.y};
}

vec2 vec2_mul(vec2 v1, vec2 v2)
{
    return (vec2){v1.x * v2.x, v1.y * v2.y};
}

vec2 vec2_div(vec2 v1, vec2 v2)
{
    if (v2.x == 0 || v2.y == 0) return (vec2){0, 0};
    return (vec2){v1.x / v2.x, v1.y / v2.y};
}

vec2 vec2_scale(vec2 v, float value)
{
    return(vec2){v.x * value, v.y * value};
}

float vec2_dot(vec2 v1, vec2 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

vec3 vec3_lerp(vec3 start, vec3 end, float amount)
{
    return (vec3)
    {
        (1.0f - amount) * start.x + amount * end.x,
        (1.0f - amount) * start.y + amount * end.y,
        (1.0f - amount) * start.z + amount * end.z,
    };
}


vec3 vec3_add(vec3 v1, vec3 v2)
{
    return (vec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3 vec3_sub(vec3 v1, vec3 v2)
{
    return (vec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

vec3 vec3_mul(vec3 v1, vec3 v2)
{
    return (vec3){v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

vec3 vec3_div(vec3 v1, vec3 v2)
{
    if (v2.x == 0 || v2.y == 0 || v2.z == 0) return (vec3){0, 0};
    return (vec3){v1.x / v2.x, v1.y / v2.y};
}

vec3 vec3_scale(vec3 v, float value)
{
    return(vec3){v.x * value, v.y * value, v.z * value};
}

float vec3_dot(vec3 v1, vec3 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) * (v1.z * v2.z);
}

