#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // memset, realloc
#include <string.h>
#include <assert.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "types.h"
#include "util.h"
#include "stb_truetype.h"
#include "stb_image_write.h"

Color style_get_bg_color(Style* style) {
    if (style->override_bg_color || !style->base)
        return style->bg_color;
    return style_get_bg_color(style->base);
}

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
    Bitmap bitmap;
    bitmap.width = pixel_size * 48;
    bitmap.height = pixel_size * 2;
    unsigned char* monochrome_data = malloc(bitmap.width * bitmap.height * 1);

    result->chars = malloc(sizeof(stbtt_bakedchar) * 96);
    stbtt_BakeFontBitmap(file.data, 0, pixel_size, monochrome_data, bitmap.width, bitmap.height, 32, 96, result->chars);

    // blow the alpha-only image up into 4 channel
    bitmap.data = malloc(bitmap.width * bitmap.height * bytes_per_pixel);
    unsigned char* a = monochrome_data;
    unsigned int* pixel = bitmap.data;
    for (int i = 0; i < bitmap.width * bitmap.height; i++) {
        *pixel = (*a << 24) | (0xFF << 16) | (*a << 8) | (*a);
        pixel++;
        a++;
    }
    free(monochrome_data);

    stbi_write_png("test_font.png", bitmap.width, bitmap.height, 1, bitmap.data, bytes_per_pixel * bitmap.width);

    result->surf = SDL_CreateRGBSurfaceWithFormatFrom(bitmap.data, bitmap.width, bitmap.height, 32, bytes_per_pixel * bitmap.width, SDL_PIXELFORMAT_RGBA32);

    return result;
}

SDL_Rect measure_text(Font* font, const char* text) {
    SDL_Rect result = { 0 };

    float x = 0;
    float y = 0;
    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->chars, font->surf->w, font->surf->h, *text - 32, &x, &y, &q, 0);//1=opengl & d3d10+,0=d3d9                        

            if (text[1] >= 32 && text[1] < 128) {
                result.x = x + (q.x1 - q.x0);
                result.y = y + (q.y1 - q.y0);
            }
        }
        ++text;
    }

    return result;
}

SDL_Texture* render_text(Font* font, const char* text, Color color) {
    SDL_Texture* result = 0;

    SDL_Rect surf_size = measure_text(font, text);
    SDL_Surface* temp_surf = SDL_CreateRGBSurfaceWithFormat(0, surf_size.w, surf_size.h, 32, SDL_PIXELFORMAT_RGBA32);

    float x = 0;
    float y = 0;
    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->chars, font->surf->w, font->surf->h, *text - 32, &x, &y, &q, 0);//1=opengl & d3d10+,0=d3d9

            SDL_Rect src_rect = { q.s0 * font->surf->w, q.t0 * font->surf->h, (q.s1 - q.s0) * font->surf->w, (q.t1 - q.t0) * font->surf->h };
            SDL_Rect dst_rect = { q.x0, q.y0, q.x1 - q.x0, q.y1 - q.y0 };
            SDL_BlitSurface(font->surf, &src_rect, temp_surf, &dst_rect);
            //glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y0);
            //glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y0);
            //glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y1);
            //glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y1);
        }
        ++text;
    }

    result = SDL_CreateTextureFromSurface(state->renderer, temp_surf);
    SDL_FreeSurface(temp_surf);

    return result;
}


Style get_aggregate_style(Slide* slide, TextBlock* block) {
    Style result = {0};

    if (block && block->style)
        result.bg_color = style_get_bg_color(block->style);
    else if (slide && slide->style)
        result.bg_color = style_get_bg_color(slide->style);
    else
        result.bg_color = state->default_style->bg_color;

    return result;
}

Slide* new_slide(void) {
    Slide* result = malloc(sizeof(Slide));
    return result;
}

Slide* new_slide_with_style(Style* style) {
    Slide* result = malloc(sizeof(Slide));

    memset(result, 0, sizeof(Slide));
    result->style = style;

    return result;
}

Slide* clone_slide(Slide* base) {
    Slide* result = malloc(sizeof(Slide));

    memcpy(result, base, sizeof(Slide));

    return result;
}

TextBlock* new_text_block(Slide* slide, const char* text) {    
    slide->text_blocks = realloc(slide->text_blocks, sizeof(TextBlock) * slide->num_text_blocks + 1);
    TextBlock* result = slide->text_blocks + slide->num_text_blocks;
    slide->num_text_blocks++;

    result->style = 0;
    result->v_align = Middle;
    result->h_align = Center;

    int line_count = 1;
    char* c = text;
    while(*c) {
        if (*c == '\n')
            line_count++;
        c++;
    }
    result->lines = malloc(sizeof(TextLine) * line_count);
    result->num_lines = line_count;
    result->text = text;
    c = text;
    for (int i=0; i<result->num_lines; i++) {
        TextLine* line = result->lines + i;
        line->text = c;
        while(*c && *c != '\n')
            c++;
        line->length = (c - line->text);
    }

    return result;
}

GlobalState* state;

void check_sdl_error(int line) {
#ifdef DEBUG
    const char* error = SDL_GetError();
    if (*error != '\0') {
        printf("SDL Error: %s\n", error);
        printf(" line: %d\n", line);
        SDL_ClearError();
    }
#endif
}

void set_color(GlobalState* state, Color color) {
    SDL_SetRenderDrawColor(state->renderer, color.r, color.g, color.b, color.a);
}

int main(int argc, char** argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to init SDL: %s\n", SDL_GetError());
        return 1;
    }

    state = malloc(sizeof(GlobalState));
    state->running = 0;
    
    state->window = SDL_CreateWindow("SimpleSlides",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        1280, 720, SDL_WINDOW_SHOWN);
    if (!state->window) {
        printf("Failed to create window/renderer: %s\n", SDL_GetError());
        return 1;
    }
    state->renderer = SDL_CreateRenderer(state->window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);


    SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND);
    
#if PLATFORM_WINDOWS
    const char* default_font_path = "c:/windows/fonts/arial.ttf";
#else
	const char* default_font_path = "";
	const char* cmd = "fc-match --format=%{file} arial 2>&1";
	FILE* stream = popen(cmd, "r");
	const int max_buffer = 256;
	char buffer[max_buffer];
	if (stream) {
		while(!feof(stream)) {
			if (fgets(buffer, max_buffer, stream) != 0)
				default_font_path = buffer;			
		}
	}
#endif
    Font* default_font = open_font(default_font_path, 75);    
    if (!default_font) {
        printf("Failed to open font file\n");
        return 1;
    }
        
    state->default_style = malloc(sizeof(Style));
    state->default_style->font = default_font;
    state->default_style->bg_color = white;
    state->default_style->fg_color = black;
    state->default_style->shadow_color = dark_grey;
    state->default_style->shadow_offset = (Vector){.x = 2, .y = 2};


    Slide* slide = new_slide_with_style(state->default_style);
    TextBlock* block = new_text_block(slide, "Hey look! Some Awesome Text! :-)\nThat even does\nmultiline text!");


    SDL_Event event;
    state->running = 1;
    while(state->running) {

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    state->running = 0;
                    break;

                

                default:
                    break;
            }
        }
    
        Style style = get_aggregate_style(slide, 0);
    
        set_color(state, style.bg_color);
        SDL_RenderClear(state->renderer);        

        SDL_Texture* texture = render_text(default_font, "Consigliere!", dark_grey);
        SDL_Rect dest_rect = { 0, 0, default_font->surf->w, default_font->surf->h };
        SDL_RenderCopy(state->renderer, texture, 0, &dest_rect);

#if 0
        for (int i=0; i<slide->num_text_blocks; i++) {
            TextBlock* block = slide->text_blocks + i;

            SDL_Surface* surf = render_text(style.font, block->text, style.fg_color);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(state->renderer, surf);

            int text_width = 0;
            int text_height = 0;
            if ((TTF_SizeText(style.font, block->text, &text_width, &text_height) < 0)) {
                printf("Error measuring text: %s\n", TTF_GetError());
                return 1;
            }
            
            SDL_Rect src_rect = {0, 0, text_width, text_height};

            int window_width = 0;
            int window_height = 0;
            SDL_GL_GetDrawableSize(state->window, &window_width, &window_height);
            SDL_Rect dst_rect = {0};
            if (block->v_align == Middle) {
                dst_rect.y = (window_height - text_height) * 0.5f;
            }
            else if (block->v_align == Bottom) {
                dst_rect.y = (window_height - text_height);
            }
            if (block->h_align == Center) {
                dst_rect.x = (window_width - text_width) * 0.5f;
            }
            else if (block->h_align == Right) {
                dst_rect.x = (window_width - text_width);
            }
            dst_rect.w = src_rect.w;
            dst_rect.h = src_rect.h;

            if (style.shadow_color.a != 0) {
                SDL_Rect shadow_rect = dst_rect;
                shadow_rect.x += style.shadow_offset.x;
                shadow_rect.y += style.shadow_offset.y;
                SDL_Surface* shadow = TTF_RenderText_Blended(style.font, block->text, style.shadow_color);
                SDL_Texture* shadow_texture = SDL_CreateTextureFromSurface(state->renderer, shadow);
                SDL_RenderCopy(state->renderer, shadow_texture, &src_rect, &shadow_rect);

                SDL_FreeSurface(shadow);
                SDL_DestroyTexture(shadow_texture);
            }

            SDL_RenderCopy(state->renderer, texture, &src_rect, &dst_rect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(texture);
        }
#endif

        SDL_RenderPresent(state->renderer);
    }

    // TODO(scott): close/destroy fonts
    
    //SDL_DestroyRenderer(state->renderer);
    //SDL_DestroyWindow(state->window);
    //SDL_Quit();

    return 0;
}
