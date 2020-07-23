#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include "renderer.h"
#include <stb_truetype.h>

typedef struct Font {
   const char* path;
   float point_size;
   stbtt_bakedchar* chars;
   Bitmap bitmap;
   //SDL_Surface* surf;
} Font;

Font* open_font(const char* path, float pixel_size);
void measure_text(Font* font, const char* text, int* width, int* height);
Bitmap render_text(Font* font, const char* text, Color4f color);

#endif