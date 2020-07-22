#ifndef TYPES_H
#define TYPES_H

#include <SDL.h>
#include <stb_truetype.h>

typedef SDL_Color Color;

static const Color transparent = {0, 0, 0, 0};
static const Color white = {255, 255, 255, 255};
static const Color black = {0, 0, 0, 255};
static const Color light_blue = {123, 220, 240, 255};
static const Color blue_green = {78, 201, 176, 255};
static const Color light_yellow = {230, 230, 170, 255};
static const Color medium_grey = {133, 133, 133, 255};
static const Color dark_grey = {60, 60, 60, 255};


typedef struct Vector {
    float x, y;
} Vector;

typedef struct Bitmap {
    int width;
    int height;
    unsigned char* data;
} Bitmap;
unsigned char* bitmap;

typedef struct Font {
    const char* path;
    float point_size;
    stbtt_bakedchar* chars;
    //Bitmap bitmap;
    SDL_Surface* surf;
} Font;

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

    Color fg_color;
    Color bg_color;
    Color shadow_color;
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
    SDL_Window* window;
    SDL_Renderer* renderer;
    //SDL_GLContext context;
    int running;

    Style* default_style;    
} GlobalState;
extern GlobalState* state;

#endif