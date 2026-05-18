# catlib
Simple 2D programming library for game dev

Features:

-No external dependencies

-Written in C with snake_case

-Open sourced

Basic program example:
```c
#include <catlib.h>

int main()
{
    init_window(800, 600, "wowie");

    while (!window_should_close())
    {
        clear_color(COLOR_BLUE);

        start_drawing();

        draw_rect_centered(get_screen_center(), (vec2){100, 100}, COLOR_RED);

        end_drawing();
    }

    return 0;
}
```
