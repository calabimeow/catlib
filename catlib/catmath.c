#define CATLIB_BUILD

#include "catmath.h"
#include <math.h>

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
    return (vec2)
    {
        v2.x == 0 ? 0 : v1.x / v2.x,
        v2.y == 0 ? 0 : v1.y / v2.y
    };
    return (vec2){v1.x / v2.x, v1.y / v2.y};
}

vec2 vec2_scale(vec2 v, float value)
{
    return(vec2){v.x * value, v.y * value};
}

vec2 vec2_normalize(vec2 v)
{
    float length = vec2_length(v);
    return (vec2) {v.x / length, v.x / length};
}

vec2 vec2_negate(vec2 v)
{
    return (vec2){-v.x, -v.y};
}

float vec2_dot(vec2 v1, vec2 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

float vec2_cross(vec2 v1, vec2 v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

float vec2_length(vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

float vec2_dist(vec2 v1, vec2 v2)
{
    return sqrtf(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
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
    return (vec3)
    {
        v2.x == 0 ? 0 : v1.x / v2.x,
        v2.y == 0 ? 0 : v1.y / v2.y,
        v2.z == 0 ? 0 : v1.z / v2.z
    };
}

vec3 vec3_scale(vec3 v, float value)
{
    return(vec3){v.x * value, v.y * value, v.z * value};
}

vec3 vec3_normalize(vec3 v)
{
    float length = vec3_length(v);
    return (vec3) {v.x / length, v.x / length, v.z / length};
}

vec3 vec3_negate(vec3 v)
{
    return (vec3){-v.x, -v.y, -v.z};
}

float vec3_dot(vec3 v1, vec3 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

vec3 vec3_cross(vec3 v1, vec3 v2)
{
    return (vec3)
    {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.z
    };
}

float vec3_length(vec3 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float vec3_dist(vec3 v1, vec3 v2)
{
    return sqrtf(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
}

