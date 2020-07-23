#include "font_renderer.h"
#include "util.h"

#include <stdlib.h> // malloc
#include <stdio.h>

Font* open_font(const char* path, float pixel_size) {
   Font* result = 0;

   ReadFileResult file = read_file(path);
   if (!file.success) {
      printf("Failed to open file!");
      return result;
   }

   result = malloc(sizeof(Font));
   result->path = path;
   result->point_size = pixel_size;

   int bytes_per_pixel = 4;   
   result->bitmap.width = pixel_size * 48;
   result->bitmap.height = pixel_size * 2;
   unsigned char* monochrome_data = malloc(result->bitmap.width * result->bitmap.height * 1);

   result->chars = malloc(sizeof(stbtt_bakedchar) * 96);
   stbtt_BakeFontBitmap(file.data, 0, pixel_size, monochrome_data, result->bitmap.width, result->bitmap.height, 32, 96, result->chars);

   // blow the alpha-only image up into 4 channel
   result->bitmap.data = malloc(result->bitmap.width * result->bitmap.height * bytes_per_pixel);
   unsigned char* a = monochrome_data;
   unsigned int* pixel = result->bitmap.data;
   for (int i = 0; i < result->bitmap.width * result->bitmap.height; i++) {
      *pixel = (*a << 24) | (0xFF << 16) | (*a << 8) | (*a);
      pixel++;
      a++;
   }
   free(monochrome_data);

   //stbi_write_png("test_font.png", bitmap.width, bitmap.height, 1, bitmap.data, bytes_per_pixel * bitmap.width);   

   return result;
}

void measure_text(Font* font, const char* text, int* width, int* height) {

   float max_x = 0;
   float max_y = 0;
   float x = 0;
   float y = 0;
   while (*text) {
      if (*text >= 32 && *text < 128) {
         stbtt_aligned_quad q;
         stbtt_GetBakedQuad(font->chars, font->bitmap.width, font->bitmap.height, *text - 32, &x, &y, &q, 0);//1=opengl & d3d10+,0=d3d9                        

         if (text[1] >= 32 && text[1] < 128) {
            max_x = x + (q.x1 - q.x0);
            max_y = y + (q.y1 - q.y0);
         }
      }
      ++text;
   }

   *width = max_x;
   *height = max_y;
}

Bitmap render_text(Font* font, const char* text, Color4f color) {
   Bitmap result;

   measure_text(font, text, &result.width, &result.height);
   result.data = malloc(result.width * result.height * 4 /* bytes per pixel */);

   float x = 0;
   float y = 0;
   while (*text) {
      if (*text >= 32 && *text < 128) {
         stbtt_aligned_quad q;
         stbtt_GetBakedQuad(font->chars, font->bitmap.width, font->bitmap.width, *text - 32, &x, &y, &q, 0);//1=opengl & d3d10+,0=d3d9

         //SDL_Rect src_rect = { q.s0 * font->surf->w, q.t0 * font->surf->h, (q.s1 - q.s0) * font->surf->w, (q.t1 - q.t0) * font->surf->h };
         //SDL_Rect dst_rect = { q.x0, q.y0, q.x1 - q.x0, q.y1 - q.y0 };
         //SDL_BlitSurface(font->surf, &src_rect, temp_surf, &dst_rect);
         // TODO(scott): render to a texture and return that texture
         //glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y0);
         //glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y0);
         //glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y1);
         //glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y1);
      }
      ++text;
   }

   return result;
}
