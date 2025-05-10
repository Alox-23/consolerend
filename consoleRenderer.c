#include "consoleRenderer.h"

int init_Surface(struct Surface *surface_ptr, struct Rect rect) {
  surface_ptr->width = rect.size.x;
  surface_ptr->height = rect.size.y;

  if (!surface_ptr || rect.size.x <= 0 || rect.size.y <= 0) {
    return 1;
  }

  //dynamicly alocate to the heap
  surface_ptr->buffer = malloc(rect.size.y * sizeof(char**));

  for (int y = 0; y < rect.size.y; y++){
    surface_ptr->buffer[y] = malloc(rect.size.x * sizeof(char*));
    for (int x = 0; x < rect.size.x; x++){
      surface_ptr->buffer[y][x] = malloc(28 * sizeof(char));
    }
  }

  struct Color color = {0, 0, 0};

  fill_Surface(surface_ptr, color);

  return 0;
}
void destroy_Surface(struct Surface *surface_ptr){
  for (int y = 0; y < surface_ptr->height; y++){
    for (int x = 0; x < surface_ptr->width; x++){
      free(surface_ptr->buffer[y][x]);
    }
    free(surface_ptr->buffer[y]);
  }
  free(surface_ptr->buffer);
}
int blit_Surface(struct Surface *src_Surface, struct Surface *dest_Surface, struct Rect rect){
  if (!src_Surface || !dest_Surface){
    fprintf(stderr, "Invalid Surface pointer");
    return 1;
  }
  if (src_Surface->width > dest_Surface->width || src_Surface->height > dest_Surface->height){
    fprintf(stderr, "Invalid surface width");
    return 1;
  }
  
  int sizex = rect.size.x;
  int sizey = rect.size.y; 
  int posx = rect.pos.x;
  int posy = rect.pos.y;

  //cliping to avoid buffer overflow
  if (posx + sizex > dest_Surface->width){
    sizex = dest_Surface->width - posx;
  }
  if (posy + sizey > dest_Surface->height){
    sizey = dest_Surface->height - posy;
  }
  if (posy< 0){
    posy = 0;
    sizey = rect.size.y + rect.pos.y;
  }
  if (posx< 0){
    posx = 0;
    sizex = rect.size.x + rect.pos.x;
  }

  for (int i = 0; i < sizey; i++){
    for (int j = 0; j < sizex; j++){
      strncpy(dest_Surface->buffer[i+posy][j+posx], src_Surface->buffer[i][j], 28);
    }
  }
  return 0;
}
void rgb_to_ansi(int r, int g, int b, int foreground, char *buffer) {
  if (foreground) {
    snprintf(buffer, 28, "\033[38;2;%d;%d;%dm", r, g, b);
  } else {
    snprintf(buffer, 28, "\033[48;2;%d;%d;%dm", r, g, b);
  }
}

void create_triangle_gradient(struct Surface *surface, struct Color c1,
                              struct Vector p1, struct Color c2,
                              struct Vector p2, struct Color c3,
                              struct Vector p3) {
  if (!surface || surface->width <= 0 || surface->height <= 0)
    return;

  char ansi_code[28];
  for (int y = 0; y < surface->height; y++) {
    for (int x = 0; x < surface->width; x++) {
      // Calculate barycentric coordinates
      float det = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
      float w1 =
          ((p2.y - p3.y) * (x - p3.x) + (p3.x - p2.x) * (y - p3.y)) / det;
      float w2 =
          ((p3.y - p1.y) * (x - p3.x) + (p1.x - p3.x) * (y - p3.y)) / det;
      float w3 = 1 - w1 - w2;

      if (w1 >= 0 && w1 <= 1 && w2 >= 0 && w2 <= 1 && w3 >= 0 && w3 <= 1) {
        // Inside triangle - interpolate colors
        unsigned char r = c1.r * w1 + c2.r * w2 + c3.r * w3;
        unsigned char g = c1.g * w1 + c2.g * w2 + c3.g * w3;
        unsigned char b = c1.b * w1 + c2.b * w2 + c3.b * w3;

        rgb_to_ansi(r, g, b, 1, ansi_code);
      } else {
        // Outside triangle - black
        rgb_to_ansi(0, 0, 0, 1, ansi_code);
      }
      strncpy(surface->buffer[y][x], ansi_code, 28);
    }
  }
}

void create_diagonal_gradient(struct Surface *surface, struct Color start,
                              struct Color end) {
  if (!surface || surface->width <= 0 || surface->height <= 0)
    return;

  char ansi_code[28];
  float max_dist = surface->width + surface->height - 2;

  for (int y = 0; y < surface->height; y++) {
    for (int x = 0; x < surface->width; x++) {
      float ratio = (x + y) / max_dist;
      unsigned char r = start.r + (end.r - start.r) * ratio;
      unsigned char g = start.g + (end.g - start.g) * ratio;
      unsigned char b = start.b + (end.b - start.b) * ratio;

      rgb_to_ansi(r, g, b, 1, ansi_code);
      strncpy(surface->buffer[y][x], ansi_code, 28);
    }
  }
}

void fill_gradient_Surface(struct Surface *surface, struct Color color1,
                           struct Color color2) {
  // Validate input
  if (!surface || surface->width <= 0 || surface->height <= 0)
    return;

  // Temporary buffer for ANSI code
  char ansi_code[28];

  for (int x = 0; x < surface->width; x++) {
    // Calculate interpolation ratio (0.0 at left to 1.0 at right)
    float ratio = (float)x / (surface->width - 1);

    // Interpolate RGB values
    unsigned char r = color1.r + (color2.r - color1.r) * ratio;
    unsigned char g = color1.g + (color2.g - color1.g) * ratio;
    unsigned char b = color1.b + (color2.b - color1.b) * ratio;

    // Generate ANSI color code for this column
    rgb_to_ansi(r, g, b, 1, ansi_code); // 1 for foreground color

    // Store this color in every row of the current column
    for (int y = 0; y < surface->height; y++) {
      strncpy(surface->buffer[y][x], ansi_code, 28);
    }
  }
}

void create_rainbow_spectrum(struct Surface *surface) {
  if (!surface || surface->width <= 0 || surface->height <= 0)
    return;

  char ansi_code[28];
  for (int x = 0; x < surface->width; x++) {
    float hue = (float)x / surface->width * 6.0f; // 0-6 range for full spectrum
    struct Color c;

    // HSV to RGB conversion
    if (hue < 1) { // Red to Yellow
      c.r = 255;
      c.g = 255 * hue;
      c.b = 0;
    } else if (hue < 2) { // Yellow to Green
      c.r = 255 * (2 - hue);
      c.g = 255;
      c.b = 0;
    } else if (hue < 3) { // Green to Cyan
      c.r = 0;
      c.g = 255;
      c.b = 255 * (hue - 2);
    } else if (hue < 4) { // Cyan to Blue
      c.r = 0;
      c.g = 255 * (4 - hue);
      c.b = 255;
    } else if (hue < 5) { // Blue to Magenta
      c.r = 255 * (hue - 4);
      c.g = 0;
      c.b = 255;
    } else { // Magenta to Red
      c.r = 255;
      c.g = 0;
      c.b = 255 * (6 - hue);
    }

    rgb_to_ansi(c.r, c.g, c.b, 1, ansi_code);
    for (int y = 0; y < surface->height; y++) {
      strncpy(surface->buffer[y][x], ansi_code, 28);
    }
  }
}

void cleanup(void){
  system("clear");
  printf("\033[?25h");
}
/*void update_Screen(struct Surface *Surface) {
  printf("\033[H");
  printf("\033[?25l\033[H");
  printf("Surface width height: \n%s", Surface->width);
  printf("\n%s", Surface->height);
  printf("Max width and height: \n%s", MAX_WIDTH);
  printf("\n%s", MAX_HEIGHT);
  char output[(MAX_WIDTH * (28 + 2) + 1) * MAX_HEIGHT];
  char *ptr = output;
  for (int i = 0; i < Surface->height; i++) {
    for (int j = 0; j < Surface->width; j++) {
      ptr += sprintf(ptr, "%s%s", Surface->buffer[i][j], ASCII_VALUE);
    }
    ptr += sprintf(ptr, "\n");
  }
  free(ptr);
  printf("%s", output);
  printf("\033[?25h");
  fflush(stdout);
}*/

void update_Screen(struct Surface *Surface) {
  printf("\033[H");
  printf("\033[?25l\033[H");
  char output[(MAX_WIDTH * (28 + 2) + 1) *
                     MAX_HEIGHT];
  char *ptr = output;
  for (int i = 0; i < Surface->height; i++) {
    for (int j = 0; j < Surface->width; j++) {
      ptr += sprintf(ptr, "%s%s", Surface->buffer[i][j], ASCII_VALUE);
    }
    ptr += sprintf(ptr, "\n");
  }
  printf("%s", output);
  printf("\033[?25h");
  fflush(stdout);
}

void fill_Surface(struct Surface *Surface, struct Color color) {
  for (int i = 0; i < Surface->height; i++) {
    for (int j = 0; j < Surface->width; j++) {
      char value[28];
      rgb_to_ansi(color.r, color.g, color.b, 1, value);
      strcpy(Surface->buffer[i][j], value);
    }
  }
}
