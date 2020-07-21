#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // memset, realloc
#include <assert.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#include "types.h"

Color style_get_bg_color(Style* style) {
    if (style->override_bg_color || !style->base)
        return style->bg_color;
    return style_get_bg_color(style->base);
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

int main(int argc, char** argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to init SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        printf("Failed to init SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }

    state = malloc(sizeof(GlobalState));
    state->running = 0;
    
    if (SDL_CreateWindowAndRenderer(1280, 720, 0, &state->window, &state->renderer)) {
        printf("Failed to create window/renderer: %s\n", SDL_GetError());
        return 1;
    }
    
#if _WIN32
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
    TTF_Font* default_font = TTF_OpenFont(default_font_path, 75);    
    if (!default_font) {
        printf("Failed to open font file: %s\n", TTF_GetError());
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

#if 0
    Slide* slide = new_slide_with_style(&base_style);
    slide->bg_color = medium_grey; //(Color){132, 133, 140, 255};
    TextBlock* block = new_text_block("Hey look! Some Awesome Text! :-)", light_yellow, font, ; //(Color){155, 215, 232, 255};
    slide.v_align = Middle;
    slide.h_align = Center;
    slide.shadow_color = (Color){26, 26, 26, 10};
    slide.shadow_offset = (Vector){3, 3};
    TTF_Font* font = TTF_OpenFont("../data/Karmina Regular.otf", 75);    
    if (!font) {
        printf("Failed to open font file: %s\n", TTF_GetError());
        return 1;
    }
#endif

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
    
        SDL_SetRenderDrawColor(state->renderer, style.bg_color.r, style.bg_color.g, style.bg_color.b, style.bg_color.a);
        SDL_RenderClear(state->renderer);

        for (int i=0; i<slide->num_text_blocks; i++) {
            TextBlock* block = slide->text_blocks + i;

            SDL_Surface* surf = TTF_RenderText_Blended(style.font, block->text, style.fg_color);
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

        SDL_RenderPresent(state->renderer);
    }

    TTF_CloseFont(state->default_style->font);
    
    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
