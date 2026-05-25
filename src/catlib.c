#define _CRT_SECURE_NO_WARNINGS

#ifndef CATLIB_BUILD
    #define CATLIB_BUILD
#endif

#include "catlib.h"
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

static GLFWwindow *window;
static bool windowClosed = false;

static int windowWidth;
static int windowHeight;

static unsigned int VAO;
static unsigned int VBO;

static shader defaultShader = {0};

static bool drawingInit = false;

static bool keysPressed[512] = {0};
static bool keysPressedPrevious[512] = {0};
static int lastKeyPressed = 0;
static bool mouseButtonsPressed[8] = {0};
static bool mouseButtonsPressedPrevious[8] = {0};
static float mouseScrollX = 0.0;
static float mouseScrollY = 0.0;

static camera2D currentCam = {{0, 0}, 1.0f};

static unsigned int textureVAO = 0;
static unsigned int textureVBO = 0;
static unsigned int textureEBO = 0;
static shader defaultTextureShader = {0};
static bool textureInit = false;

static int frameCount = 0.0f;
static int lastTime = 0.0f;
static int fps = 0.0f;
static float deltaTime = 0.0f;
static double lastFrameTime = 0.0f;

static ma_engine audioEngine = {0};

static const char *textureVertShader = 
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform vec2 screenSize;\n"
    "uniform vec2 camPos;\n"
    "uniform float camZoom;\n"
    "uniform vec2 position;\n"
    "uniform vec2 size;\n"
    "uniform vec2 origin;\n"
    "uniform float rotation;\n"
    "void main()\n"
    "{\n"
        "vec2 pos = aPos - origin;\n"
        "pos *= size;\n"
        "float s = sin(rotation);\n"
        "float c = cos(rotation);\n"
        "pos = vec2(pos.x * c - pos.y * s, pos.x * s + pos.y * c);\n"
        "pos += position;\n"
        "vec2 translated = pos - camPos;\n"
        "vec2 zoomed = (translated - screenSize * 0.5) * camZoom + screenSize * 0.5;\n"
        "vec2 ndcPos = (zoomed / screenSize) * 2.0 - 1.0;\n"
        "ndcPos.y = -ndcPos.y;\n"
        "gl_Position = vec4(ndcPos, 0.0, 1.0);\n"
        "TexCoord = aTexCoord;\n"
    "}\0";

static const char *textureFragShader = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform vec4 tintColor;\n"
    "void main()\n"
    "{\n"
        "vec4 texColor = texture(textureSampler, TexCoord);\n"
        "FragColor = texColor * tintColor;\n"
    "}\0";

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 512)
    {
        switch (action)
        {
            case GLFW_PRESS:
                keysPressed[key] = true;
                lastKeyPressed = key;
                break;
            case GLFW_RELEASE:
                keysPressed[key] = false;
                break;
        }
    }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button >= 0 && button < 8)
    {
        switch (action)
        {
            case GLFW_PRESS:
                mouseButtonsPressed[button] = true;
                break;
            case GLFW_RELEASE:
                mouseButtonsPressed[button] = false;
                break;
        }
    }
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    mouseScrollX = xoffset;
    mouseScrollY = yoffset;
}

//----------------
//Window
//----------------
void init_window(int width, int height, const char *name)
{
    windowClosed = false;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name, NULL, NULL);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //glfwSwapInterval(0);

    windowWidth = width;
    windowHeight = height;
}

void close_window()
{
    if (!windowClosed)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        windowClosed = true;
    }
}

void toggle_fullscreen()
{   
    static int xpos, ypos, width, height;

    if (glfwGetWindowMonitor(window))
    {
        glfwSetWindowMonitor(window, NULL, xpos, ypos, width, height, 0);
    }

    else
    {
        glfwGetWindowPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &width, &height);

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
}

bool window_should_close()
{
    if (windowClosed) return true;
    if (is_key_pressed(KEY_ESCAPE)) return true;
    return glfwWindowShouldClose(window);
}

int get_screen_width(){return windowWidth;}
int get_screen_height(){return windowHeight;}

vec2 get_screen_dimensions(){return (vec2){windowWidth, windowHeight};}
vec2 get_screen_center(){return (vec2){(float)windowWidth / 2, (float)windowHeight / 2};}

void set_window_title(const char *title)
{
    glfwSetWindowTitle(window, title);
}

void set_window_size(int width, int height)
{
    glfwSetWindowSize(window, width, height);
}

void set_window_size(int width, int height);

//----------------
//Frames
//----------------
static void update_fps()
{
    double currentTime = glfwGetTime();
    deltaTime = (float)(currentTime - lastFrameTime);
    lastFrameTime = currentTime;
    
    frameCount++;
    if (currentTime - lastTime >= 1.0)
    {
        fps = frameCount;
        frameCount = 0;
        lastTime = currentTime;
    }
}

CATAPI int get_fps()
{
    return fps;
}

CATAPI float get_frame_time()
{
    return deltaTime;
}

//----------------
//Buttons
//----------------
bool is_key_pressed(int key)
{
    return keysPressed[key] && !keysPressedPrevious[key];
}

bool is_key_down(int key)
{
    return keysPressed[key];
}

bool is_key_released(int key)
{
    return !keysPressed[key] && keysPressedPrevious[key];
}

//----------------
//Mouse
//----------------
vec2 get_mouse_pos()
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return (vec2){x, y};
}

void disable_cursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void enable_cursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void hide_cursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void show_cursor()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool is_mouse_button_pressed(mouse_button button)
{
    return mouseButtonsPressed[button] && !mouseButtonsPressedPrevious[button];
}

bool is_mouse_button_down(mouse_button button)
{
    return mouseButtonsPressed[button];
}

bool is_mouse_button_released(mouse_button button)
{
    return !mouseButtonsPressed[button] && mouseButtonsPressedPrevious[button];
}

vec2 get_mouse_wheel()
{
    return (vec2){mouseScrollX, mouseScrollY};
}

//----------------
//Drawing
//----------------

static void init_drawing()
{
    if (drawingInit) return;

    const char* defaultVertShader = 
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "uniform vec2 screenSize;\n"
        "uniform vec2 camPos;\n"
        "uniform float camZoom;\n"
        "void main()\n"
        "{\n"
            "vec2 worldPos = aPos;\n"
            "vec2 translated = worldPos - camPos;\n"
            "vec2 zoomed = (translated - screenSize * 0.5) * camZoom + screenSize * 0.5;\n"
            "vec2 ndcPos = (zoomed / screenSize) * 2.0 - 1.0;\n"
            "ndcPos.y = -ndcPos.y;\n"
            "gl_Position = vec4(ndcPos, 0.0, 1.0);\n"
        "}\0";
    
    const char* defaultFragShader = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 color;\n"
        "void main()\n"
        "{\n"
        "    FragColor = color;\n"
        "}\0";
    
    unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &defaultVertShader, NULL);
    glCompileShader(vertShader);
    
    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &defaultFragShader, NULL);
    glCompileShader(fragShader);
    
    defaultShader.id = glCreateProgram();
    glAttachShader(defaultShader.id, vertShader);
    glAttachShader(defaultShader.id, fragShader);
    glLinkProgram(defaultShader.id);
    
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    drawingInit = true;
}

void start_drawing()
{
    memcpy(keysPressedPrevious, keysPressed, sizeof(keysPressed));
    memcpy(mouseButtonsPressedPrevious, mouseButtonsPressed, sizeof(mouseButtonsPressed));
    
    glfwPollEvents();
    
    lastKeyPressed = 0;

    update_fps();
}

void end_drawing()
{
    glfwSwapBuffers(window);
}

void clear_bg(color col)
{
    color normalizedColor = color_to_float(col);
    glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, normalizedColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw_shape(vec2 verts[], int vertCount, color col)
{
    init_drawing();
    use_shader(defaultShader);

    int screenSizeLoc = glGetUniformLocation(defaultShader.id, "screenSize");
    glUniform2f(screenSizeLoc, (float)windowWidth, (float)windowHeight);

    int colorLoc = glGetUniformLocation(defaultShader.id, "color");
    color normalizedColor = color_to_float(col);
    glUniform4f(colorLoc, normalizedColor.r, normalizedColor.g, normalizedColor.b, normalizedColor.a);

    int camPosLoc = glGetUniformLocation(defaultShader.id, "camPos");
    glUniform2f(camPosLoc, currentCam.pos.x, currentCam.pos.y);
    
    int camZoomLoc = glGetUniformLocation(defaultShader.id, "camZoom");
    glUniform1f(camZoomLoc, currentCam.zoom);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vec2), verts, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
    glEnableVertexAttribArray(0);
    
    GLenum drawMode = GL_TRIANGLE_FAN;
    if (vertCount == 3) drawMode = GL_TRIANGLES;
    
    glDrawArrays(drawMode, 0, vertCount);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_shape_lines(vec2 verts[], int vertCount, float thick, color col)
{
    for (int i = 0; i < vertCount; i++)
    {
        draw_line((vec2){verts[i].x, verts[i].y}, (vec2){verts[i + 1].x, verts[i + 1].y}, thick, col);
    }
}

void draw_triangle(vec2 v1, vec2 v2, vec2 v3, color col)
{
    vec2 verts[3] = {v1, v2, v3};
    draw_shape(verts, 3, col);
}

void draw_rect(vec2 pos, vec2 size, color col)
{
    vec2 verts[4] =
    {
        pos,
        {pos.x + size.x, pos.y},
        {pos.x + size.x, pos.y + size.y},
        {pos.x, pos.y + size.y}
    };

    draw_shape(verts, 4, col);
}

void draw_rect_lines(vec2 pos, vec2 size, float thick, color col)
{
    vec2 verts[4] =
    {
        pos,
        {pos.x + size.x, pos.y},
        {pos.x + size.x, pos.y + size.y},
        {pos.x, pos.y + size.y}
    };

    for (int i = 0; i < 4; i++)
    {
        draw_line(verts[i], verts[(i + 1) % 4], thick, col);
    }
}

void draw_rect_centered(vec2 pos, vec2 size, color col)
{
    vec2 verts[4] =
    {
        {pos.x - size.x / 2.0f, pos.y - size.y / 2.0f},
        {pos.x + size.x / 2.0f, pos.y - size.y / 2.0f},
        {pos.x + size.x / 2.0f, pos.y + size.y / 2.0f},
        {pos.x - size.x / 2.0f, pos.y + size.y / 2.0f}
    };

    draw_shape(verts, 4, col);
}

void draw_circle(vec2 pos, float rad, int segments, color col)
{
    if (segments < 3) segments = 3;
    vec2 *verts = (vec2*)malloc((segments + 2) * sizeof(vec2));
    verts[0] = pos;
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / segments * 2.0f * PI;
        verts[i + 1] = (vec2){pos.x + cos(angle) * rad, pos.y + sin(angle) * rad};
    }
    
    draw_shape(verts, segments + 2, col);
    free(verts);
}

void draw_circle_lines(vec2 pos, float rad, int segments, float thick, color col)
{
    if (segments < 3) segments = 3;
    vec2 *verts = (vec2*)malloc((segments + 2) * sizeof(vec2));
    verts[0] = pos;
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / segments * 2.0f * PI;
        verts[i + 1] = (vec2){pos.x + cos(angle) * rad, pos.y + sin(angle) * rad};
        if (i == 0) continue;
        draw_line(verts[i], verts[i + 1], thick, col);
    }
    
    free(verts);
}

void draw_line(vec2 start, vec2 end, float thick, color col)
{
    vec2 dir = {end.x - start.x, end.y - start.y};
    float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (length < 0.00001f) return;
    vec2 normalized = {dir.x / length, dir.y / length};
    vec2 perp = {-normalized.y, normalized.x};
    float halfThick = thick / 2.0f;

    vec2 verts[4] = 
    {
        {start.x + perp.x * halfThick, start.y + perp.y * halfThick},
        {end.x + perp.x * halfThick, end.y + perp.y * halfThick},
        {end.x - perp.x * halfThick, end.y - perp.y * halfThick},
        {start.x - perp.x * halfThick, start.y - perp.y * halfThick}
    };

    draw_shape(verts, 4, col);
}

void draw_line_angled(vec2 start, vec2 end, float angle, float thick, color col)
{
    vec2 dir = {end.x - start.x, end.y - start.y};
    float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (length < 0.00001f) return;

    float c = cosf(angle);
    float s = sinf(angle);
    vec2 rotatedDir = {dir.x * c - dir.y * s, dir.x * s + dir.y * c};
    vec2 normalized = {rotatedDir.x / length, rotatedDir.y / length};
    vec2 perp = {-normalized.y, normalized.x};
    float halfThick = thick / 2.0f;

    vec2 verts[4] = 
    {
        {start.x + perp.x * halfThick, start.y + perp.y * halfThick},
        {start.x + rotatedDir.x + perp.x * halfThick, start.y + rotatedDir.y + perp.y * halfThick},
        {start.x + rotatedDir.x - perp.x * halfThick, start.y + rotatedDir.y - perp.y * halfThick},
        {start.x - perp.x * halfThick, start.y - perp.y * halfThick}
    };

    draw_shape(verts, 4, col);
}

void draw_pixel(vec2 pos, color col)
{
    draw_rect(pos, (vec2){1, 1}, col);
}

//----------------
//Camera
//----------------
void start_2D(camera2D cam)
{
    currentCam = cam;
}

void end_2D()
{
    currentCam = (camera2D){{0, 0}, 1.0f};
}

//----------------
//Color
//----------------
color color_to_float(color col) {return (color){col.r / 255.0f, col.g / 255.0f, col.b / 255.0f, col.a / 255.0f};}
color color_to_int(color col) {return (color){col.r * 255.0f, col.g * 255.0f, col.b * 255.0f, col.a * 255.0f};}

color color_lerp(color start, color end, float amount)
{
    return (color)
    {
        (1.0f - amount) * start.r + amount * end.r,
        (1.0f - amount) * start.g + amount * end.g,
        (1.0f - amount) * start.b + amount * end.b,
        (1.0f - amount) * start.a + amount * end.a,
    };
}

//----------------
//Shaders
//----------------
shader load_shader(const char *vertPath, const char *fragPath)
{
    FILE *vertFile = fopen(vertPath, "r");
    FILE *fragFile = fopen(fragPath, "r");

    fseek(vertFile, 0, SEEK_END);
    long vertSize = ftell(vertFile);
    fseek(vertFile, 0, SEEK_SET);
    
    char *vertSource = (char*)malloc(vertSize + 1);
    fread(vertSource, 1, vertSize, vertFile);
    vertSource[vertSize] = '\0';
    fclose(vertFile);

    fseek(fragFile, 0, SEEK_END);
    long fragSize = ftell(fragFile);
    fseek(fragFile, 0, SEEK_SET);
    
    char *fragSource = (char*)malloc(fragSize + 1);
    fread(fragSource, 1, fragSize, fragFile);
    fragSource[fragSize] = '\0';
    fclose(fragFile);

    unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, (const char *const*)&vertSource, NULL);
    glCompileShader(vertShader);

    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, (const char *const*)&fragSource, NULL);
    glCompileShader(fragShader);

    free(vertSource);
    free(fragSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    
    return (shader){shaderProgram};
}

void unload_shader(shader shader)
{
    if (shader.id != 0) glDeleteProgram(shader.id);
}

void use_shader(shader shader)
{
    glUseProgram(shader.id);
}

//----------------
//Textures
//----------------

static void init_texture()
{
    if (textureInit) return;
    
    unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &textureVertShader, NULL);
    glCompileShader(vertShader);
    
    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &textureFragShader, NULL);
    glCompileShader(fragShader);
    
    defaultTextureShader.id = glCreateProgram();
    glAttachShader(defaultTextureShader.id, vertShader);
    glAttachShader(defaultTextureShader.id, fragShader);
    glLinkProgram(defaultTextureShader.id);
    
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    
    float verts[] =
    {
        0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f
    };
    
    unsigned int inds[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    
    glGenVertexArrays(1, &textureVAO);
    glGenBuffers(1, &textureVBO);
    glGenBuffers(1, &textureEBO);
    
    glBindVertexArray(textureVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    textureInit = true;
}

texture load_texture(const char *path)
{
    init_texture();
    
    texture tex = {0};

    stbi_set_flip_vertically_on_load(1);
    
    unsigned char *data = stbi_load(path, &tex.width, &tex.height, &tex.channels, 0);
    
    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    GLenum format;
    if (tex.channels == 1) format = GL_RED;
    else if (tex.channels == 3) format = GL_RGB;
    else if (tex.channels == 4) format = GL_RGBA;
    else format = GL_RGBA;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, tex.width, tex.height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return tex;
}

void unload_texture(texture tex)
{
    if (tex.id != 0) glDeleteTextures(1, &tex.id);
}

void draw_texture(texture tex, vec2 pos, vec2 size, color tint)
{
    if (tex.id == 0) return;
        
    init_texture();
    use_shader(defaultTextureShader);
    
    int screenSizeLoc = glGetUniformLocation(defaultTextureShader.id, "screenSize");
    glUniform2f(screenSizeLoc, (float)windowWidth, (float)windowHeight);
    
    int camPosLoc = glGetUniformLocation(defaultTextureShader.id, "camPos");
    glUniform2f(camPosLoc, currentCam.pos.x, currentCam.pos.y);
    
    int camZoomLoc = glGetUniformLocation(defaultTextureShader.id, "camZoom");
    glUniform1f(camZoomLoc, currentCam.zoom);
    
    int posLoc = glGetUniformLocation(defaultTextureShader.id, "position");
    glUniform2f(posLoc, pos.x, pos.y);
    
    int sizeLoc = glGetUniformLocation(defaultTextureShader.id, "size");
    glUniform2f(sizeLoc, size.x, size.y);
    
    color normalizedColor = color_to_float(tint);
    int tintLoc = glGetUniformLocation(defaultTextureShader.id, "tintColor");
    glUniform4f(tintLoc, normalizedColor.r, normalizedColor.g, normalizedColor.b, normalizedColor.a);
     
    float verts[] =
    {
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 
        1.0f, 1.0f, 1.0f, 1.0f,
        
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glUniform1i(glGetUniformLocation(defaultTextureShader.id, "textureSampler"), 0);
    
    glBindVertexArray(textureVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_texture_centered(texture tex, vec2 pos, vec2 size, color tint)
{
    draw_texture(tex, (vec2){pos.x - size.x / 2.0f, pos.y - size.y / 2.0f}, size, tint);
}

vec2 get_rect_center(rect rec)
{
    return (vec2){rec.x + rec.width / 2.0f, rec.y + rec.height / 2.0f};
}

//----------------
//Collisions
//----------------
bool check_collision_recs(rect rec1, rect rec2)
{
    return
    (
        rec1.x <= rec2.x + rec2.width &&
        rec1.x + rec1.width >= rec2.x &&
        rec1.y <= rec2.y + rec2.height &&
        rec1.y + rec1.height >= rec2.y
    );
}

bool check_collision_point_rect(vec2 point, rect rec)
{
    return
    (
        point.x >= rec.x && 
        point.x <= rec.x + rec.width && 
        point.y >= rec.y && 
        point.y <= rec.y + rec.height
    );
}

static float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

bool check_collision_circle_rect(vec2 circlePos, float circleRad, rect rec)
{
    float closestX = clamp(circlePos.x, rec.x, rec.x + rec.width);
    float closestY = clamp(circlePos.y, rec.y, rec.y + rec.height);
    float distX = circlePos.x - closestX;
    float distY = circlePos.y - closestY;
    
    return ((distX * distX) + (distY * distY)) <= (circleRad * circleRad);
}

bool check_collision_point_circle(vec2 point, vec2 circlePos, float circleRad)
{
    vec2 dist = (vec2){circlePos.x - point.x, circlePos.y - point.y};
    return (dist.x || dist.y) <= circleRad;
}

//----------------
//Audio
//----------------

void init_audio()
{
    ma_engine_init(NULL, &audioEngine);
}

sound load_sound(const char *path)
{
    sound snd = {0};
    snd.engine = &audioEngine;

    snd.sound = (ma_sound*)malloc(sizeof(ma_sound));

    ma_result result = ma_sound_init_from_file(&audioEngine, path, 0, NULL, NULL, snd.sound);

    if (result != MA_SUCCESS)
    {
        free(snd.sound);
        snd.sound = NULL;
        snd.loaded = false;
        return snd;
    }

    snd.loaded = true;
    snd.playing = false;
    return snd;
}

void unload_sound(sound snd)
{
    if (snd.sound != NULL)
    {
        ma_sound_uninit(snd.sound);
        free(snd.sound);
        snd.sound = NULL;
        snd.loaded = false;
    }
}

void play_sound(sound snd)
{
    if (!snd.loaded) return; 
    ma_sound_seek_to_pcm_frame(snd.sound, 0);
    ma_sound_start(snd.sound);
    snd.playing = true;
}

void stop_sound(sound snd)
{
    ma_sound_stop(snd.sound);
    snd.playing = false;
}

void resume_sound(sound snd)
{
    if (!snd.loaded) return; 
    ma_sound_start(snd.sound);
    snd.playing = true;
}
