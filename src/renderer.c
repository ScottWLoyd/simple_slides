#include "renderer.h"
#include "types.h"
#include "glad/glad.h"

#include <stdio.h>

const Color4f transparent = { 0, 0, 0, 0 };
const Color4f white = { 1, 1, 1, 1 };
const Color4f black = { 0, 0, 0, 1 };
const Color4f light_blue = { 0.48f, 0.86f, 0.94f, 1 };
const Color4f blue_green = { 0.30f, 0.79f, 0.69f, 1 };
const Color4f light_yellow = { 0.9f, 0.9f, 0.67f, 1 };
const Color4f medium_grey = { 0.52f, 0.52f, 0.52f, 1 };
const Color4f dark_grey = { 0.24f, 0.24f, 0.24f, 1 };

#if 0
typedef struct RendererState {
    size_t num_quads;

} RendererState;
static RendererState* renderer_state;
#endif

int init_renderer(void) {
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    state->context = SDL_GL_CreateContext(state->window);
    if (!state->context) {
        printf("Failed to create OpenGL Context: %s\n", SDL_GetError());
        return 0;
    }

    SDL_GL_MakeCurrent(state->window, state->context);
    
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Attempted to load GLAD without an OpenGL Context!\n");
        return 0;
    }

    int window_width, window_height;
    SDL_GL_GetDrawableSize(state->window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    printf("OpenGL Version: %d.%d\n", GLVersion.major, GLVersion.minor);
    printf("OpenGL Shader Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));

    return 1;
}

void render_clear(Color4f c) {
    glClearColor(c.r, c.g, c.b, c.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void render_start_batch(void) {

}

void render_end_batch(void) {

}

void render_quad(float x, float y, float w, float h, Color4f color) {
    glBegin(GL_TRIANGLES);
    glColor4f(color.r, color.g, color.b, color.a);
    glVertex2f(x, y);
    glVertex2f(x, y + h);
    glVertex2f(x + w, y + h);
    glVertex2f(x + w, y + h);
    glVertex2f(x + w, y);
    glVertex2f(x, y);
    glEnd();
}

void render_fill_rect(float x, float y, float w, float h, Color4f color) {
    render_quad(x, y, w, h, color);
}
