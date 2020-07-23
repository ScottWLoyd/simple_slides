#ifndef TYPES_H
#define TYPES_H

#include <renderer.h>
#include <font_renderer.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_truetype.h>

typedef struct Vector {
    float x, y;
} Vector;

typedef enum VerticalAlignment {
    Top,
    Middle,
    Bottom
} VerticalAlignment;

typedef enum HorizontalAlignment {
    Left,
    Center,
    Right
} HorizontalAlignment;

struct Style;
typedef struct Style {
    struct Style* base;

    int override_fg_color;    
    int override_bg_color;
    int override_shadow_color;
    int override_shadow_offset;
    int override_font;

    Color4f fg_color;
    Color4f bg_color;
    Color4f shadow_color;
    Vector shadow_offset;
    Font* font;
} Style;

typedef struct TextLine {
    size_t length;
    const char* text;
} TextLine;

typedef struct TextBlock {    
    Style* style;    

    VerticalAlignment v_align;
    HorizontalAlignment h_align;

    const char* text;
    int num_lines;
    TextLine* lines;
} TextBlock;

typedef struct Slide {
    Style* style;    

    int num_text_blocks;
    TextBlock* text_blocks;
} Slide;

typedef struct GlobalState {
    GLFWwindow* window;
    
    int running;

    Style* default_style;    
} GlobalState;
extern GlobalState* state;

#endif