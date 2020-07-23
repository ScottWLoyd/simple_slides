#ifndef RENDERER_H
#define RENDERER_H

typedef union Color {
    struct {
        unsigned char r, g, b, a;
    };
    unsigned int value;
} Color;

typedef struct Color4f {
    float r, g, b, a;
} Color4f;

extern const Color4f transparent;
extern const Color4f white;
extern const Color4f black;
extern const Color4f light_blue;
extern const Color4f blue_green;
extern const Color4f light_yellow;
extern const Color4f medium_grey;
extern const Color4f dark_grey;

typedef struct Bitmap {
    int width;
    int height;
    unsigned char* data;
} Bitmap;

extern Color color_f2i(Color4f c);
inline Color color_f2i(Color4f c) {
    Color result;

    result.r = 255 * c.r;
    result.g = 255 * c.g;
    result.b = 255 * c.b;
    result.a = 255 * c.a;

    return result;
}

extern Color4f color_i2f(Color c);
inline Color4f color_i2f(Color c) {
    Color4f result;

    result.r = (float)c.r / 255.0f;
    result.g = (float)c.g / 255.0f;
    result.b = (float)c.b / 255.0f;
    result.a = (float)c.a / 255.0f;

    return result;
}

int init_renderer(void);
void render_clear(Color4f c);
void render_start_batch(void);
void render_end_batch(void);

#endif