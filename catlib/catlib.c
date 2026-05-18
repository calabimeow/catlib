#define _CRT_SECURE_NO_WARNINGS
#define CATLIB_BUILD

#include "catlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

static camera2D currentCam = {{0, 0}, 1.0f};

static unsigned int textureVAO = 0;
static unsigned int textureVBO = 0;
static unsigned int textureEBO = 0;
static shader defaultTextureShader = {0};
static bool textureInit = false;

static const char* textureVertShader = 
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

static const char* textureFragShader = 
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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

//WINDOW
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

void toggle_fulscreen()
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
    return glfwWindowShouldClose(window);  
}

int get_screen_width(){return windowWidth;}
int get_screen_height(){return windowHeight;}

vec2 get_screen_dimensions(){return (vec2){windowWidth, windowHeight};}
vec2 get_screen_center(){return (vec2){(float)windowWidth / 2, (float)windowHeight / 2};}

//BUTTONS
bool is_key_pressed(int key)
{
    return keysPressed[key] && !keysPressedPrevious[key];
}

bool is_key_down(int key)
{
    return keysPressed[key];
}

//MOUSE
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

//DRAWING

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
    
    glfwPollEvents();
    
    lastKeyPressed = 0;
}

void end_drawing()
{
    glfwSwapBuffers(window);
}

void clear_color(color col)
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
    else if (vertCount == 4) drawMode = GL_TRIANGLE_FAN;
    
    glDrawArrays(drawMode, 0, vertCount);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void draw_rect_centered(vec2 pos, vec2 size, color col)
{
    vec2 verts[4] =
    {
        {pos.x - size.x / 2.0f, pos.y - size.y / 2.0f},
        {pos.x + size.x - size.x / 2.0f, pos.y - size.y / 2.0f},
        {pos.x + size.x - size.x / 2.0f, pos.y + size.y - size.y / 2.0f},
        {pos.x - size.x / 2.0f, pos.y + size.y - size.y / 2.0f}
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

void draw_line(vec2 start, vec2 end, float thick, color col)
{
    vec2 dir = {end.x - start.x, end.y - start.y};
    float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
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

//CAMERA
void start_2D(camera2D cam)
{
    currentCam = cam;
}

void end_2D()
{
    currentCam = (camera2D){{0, 0}, 1.0f};
}

//COLOR
color color_to_float(color col) {return (color){col.r / 255.0f, col.g / 255.0f, col.b / 255.0f, col.a / 255.0f};}
color color_to_int(color col) {return (color){col.r * 255.0f, col.g * 255.0f, col.b * 255.0f, col.a * 255.0f};}

//SHADERS
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

void use_shader(shader shader)
{
    glUseProgram(shader.id);
}

//TEXTURE

static void init_texture_system()
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
    init_texture_system();
    
    texture tex = {0};

    stbi_set_flip_vertically_on_load(1);
    
    unsigned char *data = stbi_load(path, &tex.width, &tex.height, &tex.channels, 0);
    
    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
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
        
    init_texture_system();
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
    return (vec2){rec.x - rec.width / 2.0f, rec.y - rec.height / 2.0f};
}

//COLLISIONS
bool check_collision_recs(rect rec1, rect rec2)
{
    return
    (
        rec1.x < rec2.x + rec2.width &&
        rec1.x + rec1.width > rec2.x &&
        rec1.y < rec2.y + rec2.height &&
        rec1.y + rec1.height > rec2.y
    );
}
