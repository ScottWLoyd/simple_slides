#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // memset, realloc
#include <string.h>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "types.h"
#include "util.h"
#include "renderer.h"
#include "font_renderer.h"

Color4f style_get_bg_color(Style* style) {
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

static void glfw_error_callback(int error, const char* description) {
    printf("Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

int initialize_graphics(void) {
    if (!glfwInit()) {
        printf("Failed to init GLFW\n");
        return 0;
    }

    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    int window_width = 1280;
    int window_height = 720;
    state->window = glfwCreateWindow(window_width, window_height, "SimpleSlides", 0, 0);
    if (!state->window) {
        printf("failed to create window\n");
        return 0;
    }

    glfwSetKeyCallback(state->window, key_callback);
    glfwSwapInterval(1);

    if (!init_renderer()) {
        printf("failed to initialize renderer\n");
        return 0;
    }

    return 1;
}

int main(int argc, char** argv) {

    state = malloc(sizeof(GlobalState));
    state->running = 0;

    if (!initialize_graphics()) {
        printf("Exiting...\n");
        return 1;
    }

    
    
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
    state->default_style->bg_color = light_blue;
    state->default_style->fg_color = black;
    state->default_style->shadow_color = dark_grey;
    state->default_style->shadow_offset = (Vector){.x = 2, .y = 2};


    Slide* slide = new_slide_with_style(state->default_style);
    TextBlock* block = new_text_block(slide, "Hey look! Some Awesome Text! :-)\nThat even does\nmultiline text!");
        
    while(!glfwWindowShouldClose(state->window)) {

        glfwPollEvents();
        
        Style style = get_aggregate_style(slide, 0);
    
        render_clear(style.bg_color);

        //SDL_Texture* texture = render_text(default_font, "Consigliere!", dark_grey);
        //SDL_Rect dest_rect = { 0, 0, default_font->surf->w, default_font->surf->h };
        //SDL_RenderCopy(state->renderer, texture, 0, &dest_rect);
        render_start_batch();
        render_fill_rect(50, 50, 250, 250, light_yellow);
        render_end_batch();

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

        glfwSwapBuffers(state->window);
    }

    // TODO(scott): close/destroy fonts
    glfwDestroyWindow(state->window);
    glfwTerminate();

    return 0;
}
