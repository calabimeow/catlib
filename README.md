# catlib
Simple graphics library

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
