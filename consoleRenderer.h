#ifndef CONSOLE_RENDERER_H
#define CONSOLE_RENDERER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ALPHA 4
#define info_depth 28
#define x_scale 2
//#define ASCII_VALUE "aa"

struct Vector {
  int x;
  int y;
};

struct Surface {
  short width;
  short height;
  char *alpha_values[5];
  char ***alpha_buffer;
  char ***buffer;
};

struct Rect {
  struct Vector pos;
  struct Vector size;
};

struct Color {
  short r;
  short g;
  short b;
  short a;
};

void draw_pixel(struct Surface *Surface, struct Vector pos, struct Color color);
void destroy_Surface(struct Surface *surface_ptr);
int init_Surface(struct Surface *surface_ptr, struct Rect rect);
void rgb_to_ansi(int r, int g, int b, int foreground, char *buffer);
void update_Screen(struct Surface *Surface);
void fill_Surface(struct Surface *Surface, struct Color color);
void create_alpha_gradient(struct Surface *surface, struct Color color);
void fill_gradient_Surface(struct Surface *Surface, struct Color color1,
                           struct Color color2);
void create_rainbow_spectrum(struct Surface *surface);
void create_diagonal_gradient(struct Surface *surface, struct Color start,
                              struct Color end);
void create_triangle_gradient(struct Surface *surface, struct Color c1,
                              struct Vector p1, struct Color c2,
                              struct Vector p2, struct Color c3,
                              struct Vector p3);
int blit_Surface(struct Surface *src_Surface, struct Surface *dest_Surface, struct Rect rect);
int draw_rect(struct Surface *Surface, struct Rect rect, struct Color color);
void cleanup(void);

#endif
