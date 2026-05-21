#ifndef CATLIB_H
#define CATLIB_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniaudio.h"

#define CATLIB_VERSION_MAJOR 0
#define CATLIB_VERSION_MINOR 1

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef CATLIB_BUILD
        #define CATAPI __declspec(dllexport)
    #else
        #define CATAPI __declspec(dllimport)
    #endif
#elif __GNUC__ >= 4 || defined(__clang__)
    #define CATAPI __attribute__((visibility("default")))
#else
    #define CATAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PI
    #define PI 3.14159265358979323846f
#endif

#ifndef DEG2RAD
    #define DEG2RAD (PI/180.0f)
#endif

#ifndef RAD2DEG
    #define RAD2DEG (180.0f/PI)
#endif

#define CAT_TERMINATE(msg) do\
{\
    fprintf(stderr, "CATLIB ERROR: %s\n", msg); \
    exit(EXIT_FAILURE); \
}while(0)

typedef struct vec2 {float x, y;} vec2;
typedef struct vec3 {float x, y, z;} vec3;
typedef struct rect {float x, y, width, height;} rect;

typedef struct color {float r, g, b, a;} color;

typedef struct shader {int id;} shader;

typedef struct camera2D
{
    vec2 pos;
    float zoom;
}camera2D;

typedef struct texture
{
    unsigned int id;
    int width;
    int height;
    int channels;
}texture;

typedef struct sound
{
    ma_sound *sound;
    ma_engine *engine;
    bool loaded;
    bool playing;
}sound;

typedef enum keyboard_key
{
    KEY_SPACE = 32,
    KEY_APOSTROPHE = 39,
    KEY_COMMA = 44,
    KEY_MINUS = 45,
    KEY_PERIOD = 46,
    KEY_SLASH = 47,
    KEY_0 = 48, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_SEMICOLON = 59,
    KEY_EQUAL = 61,
    KEY_A = 65, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I,
    KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
    KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    KEY_LEFT_BRACKET = 91,
    KEY_BACKSLASH = 92,
    KEY_RIGHT_BRACKET = 93,
    KEY_GRAVE = 96,
    KEY_ESCAPE = 256,
    KEY_ENTER = 257,
    KEY_TAB = 258,
    KEY_BACKSPACE = 259,
    KEY_INSERT = 260,
    KEY_DELETE = 261,
    KEY_RIGHT = 262,
    KEY_LEFT = 263,
    KEY_DOWN = 264,
    KEY_UP = 265,
    KEY_PAGE_UP = 266,
    KEY_PAGE_DOWN = 267,
    KEY_HOME = 268,
    KEY_END = 269,
    KEY_CAPS_LOCK = 280,
    KEY_SCROLL_LOCK = 281,
    KEY_NUM_LOCK = 282,
    KEY_PRINT_SCREEN = 283,
    KEY_PAUSE = 284,
    KEY_F1 = 290, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    KEY_LEFT_SHIFT = 340,
    KEY_LEFT_CONTROL = 341,
    KEY_LEFT_ALT = 342,
    KEY_LEFT_SUPER = 343,
    KEY_RIGHT_SHIFT = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT = 346,
    KEY_RIGHT_SUPER = 347,
    KEY_KB_MENU = 348
}keyboard_key;

typedef enum mouse_button
{
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_RIGHT = 1,
    MOUSE_BUTTON_MIDDLE = 2
}mouse_button;

//----------------
//Window
//----------------
CATAPI void init_window(int width, int height, const char *title);
CATAPI void close_window();
CATAPI void toggle_fullscreen();

CATAPI bool window_should_close();

CATAPI int get_screen_width();
CATAPI int get_screen_height();

CATAPI vec2 get_screen_dimensions();
CATAPI vec2 get_screen_center();

CATAPI void set_window_title(const char *title);
CATAPI void set_window_size(int width, int height);

//----------------
//Frames
//----------------
CATAPI int get_fps();
CATAPI float get_frame_time();

//----------------
//Buttons
//----------------
CATAPI bool is_key_pressed(int key);
CATAPI bool is_key_down(int key);
CATAPI bool is_key_released(int key);

//----------------
//Mouse
//----------------
CATAPI vec2 get_mouse_pos();
CATAPI void disable_cursor();
CATAPI void enable_cursor();
CATAPI void hide_cursor();
CATAPI void show_cursor();
CATAPI bool is_mouse_button_pressed(mouse_button button);
CATAPI bool is_mouse_button_down(mouse_button button);
CATAPI bool is_mouse_button_released(mouse_button button);
CATAPI vec2 get_mouse_wheel();

//----------------
//Drawing
//----------------
CATAPI void start_drawing();
CATAPI void end_drawing();
CATAPI void clear_bg(color col);

CATAPI void draw_shape(vec2 verts[], int vertCount, color col);
CATAPI void draw_shape_lines(vec2 verts[], int vertCount, float thick, color col);
CATAPI void draw_triangle(vec2 v1, vec2 v2, vec2 v3, color col);
CATAPI void draw_rect(vec2 pos, vec2 size, color col);
CATAPI void draw_rect_lines(vec2 pos, vec2 size, float thick, color col);
CATAPI void draw_rect_centered(vec2 pos, vec2 size, color col);
CATAPI void draw_circle(vec2 pos, float rad, int segments, color col);
CATAPI void draw_circle_lines(vec2 pos, float rad, int segments, float thick, color col);
CATAPI void draw_line(vec2 start, vec2 end, float thick, color col);
CATAPI void draw_line_angled(vec2 start, vec2 end, float angle, float thick, color col);

//----------------
//Camera
//----------------
CATAPI void start_2D(camera2D cam);
CATAPI void end_2D();

//----------------
//Color
//----------------
CATAPI color color_to_float(color col);
CATAPI color color_to_int(color col);
CATAPI color color_lerp(color start, color end, float amount);

#define COLOR_WHITE ((color){255, 255, 255, 255})
#define COLOR_BLACK ((color){0, 0, 0, 255})
#define COLOR_RED_PURE ((color){255, 0, 0, 255})
#define COLOR_GREEN_PURE ((color){0, 255, 0, 255})
#define COLOR_BLUE_PURE ((color){0, 0, 255, 255})
#define COLOR_YELLOW_PURE ((color){255, 255, 0, 255})
#define COLOR_PURPLE_PURE ((color){255, 0, 255, 255})
#define COLOR_CYAN_PURE ((color){0, 255, 255, 255})

#define COLOR_RED ((color){232, 53, 71, 255})
#define COLOR_GREEN ((color){109, 222, 78, 255})
#define COLOR_BLUE ((color){87, 173, 252, 255})
#define COLOR_ORANGE ((color){241, 166, 27, 255})
#define COLOR_YELLOW ((color){252, 234, 66, 255})
#define COLOR_PINK ((color){236, 128, 165, 255})

//----------------
//Shaders
//----------------
CATAPI shader load_shader(const char *vertPath, const char *fragPath);
CATAPI void unload_shader(shader shader);
CATAPI void use_shader(shader shader);

//----------------
//Textures
//----------------
CATAPI texture load_texture(const char *path);
CATAPI void unload_texture(texture tex);
CATAPI void draw_texture(texture texture, vec2 pos, vec2 size, color tint);
CATAPI void draw_texture_centered(texture tex, vec2 pos, vec2 size, color tint);
CATAPI void draw_texture_full(texture tex, vec2 pos, vec2 size, rect source, vec2 origin, float rotation, color tint);

//----------------

CATAPI vec2 get_rect_center(rect rec);

//----------------

//----------------
//Collisions
//----------------
CATAPI bool check_collision_recs(rect rec1, rect rec2);
CATAPI bool check_collision_point_rect(vec2 point, rect rec);
CATAPI bool check_collision_circle_rect(vec2 circlePos, float circleRad, rect rec);
CATAPI bool check_collision_point_circle(vec2 point, vec2 circlePos, float circleRad);

//----------------
//Audio
//----------------
CATAPI void init_audio();
CATAPI sound load_sound(const char *path);
CATAPI void unload_sound(sound snd);
CATAPI void play_sound(sound snd);
CATAPI void stop_sound(sound snd);
CATAPI void resume_sound(sound snd);

#ifdef __cplusplus
}
#endif

#endif
