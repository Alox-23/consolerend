#include "consoleRenderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "input.h"

int main(void) {
  struct Surface Screen;
  struct Rect screenRect = {{0, 0}, {15, 15}};
  struct Color bg_color = {0, 0, 255};
  struct Color bg_color2 = {255, 0, 0};

  short triangle_offset = 1;

  struct Vector p1 = {screenRect.size.x / 2 + triangle_offset,
                      screenRect.size.y / 2 + triangle_offset}; // Top-left
  struct Vector p2 = {triangle_offset,
                      screenRect.size.y / 2 + triangle_offset}; // bottom-left
  struct Vector p3 = {screenRect.size.x / 2 + triangle_offset,
                      triangle_offset}; // Bottom-center

  struct Color red = {255, 0, 0};
  struct Color green = {0, 255, 0};
  struct Color blue = {0, 0, 255};
  
  KeyboardState kb_state = {0};
  if (init_input()){
    printf("Error initalizing KeyboardState\n");
    cleanup();
    return 1;
  }

  if (init_Surface(&Screen, screenRect)){
    printf("Error initializing surface\n");
    cleanup();
    return 1;
  }

  struct Surface player_Surface;
  struct Rect player_Rect = {{5, 5}, {1, 1}};
  if (init_Surface(&player_Surface, player_Rect))
  {
    cleanup();
    return 1;
  }
  fill_Surface(&player_Surface, green);
  
  //fill_gradient_Surface(&Screen, bg_color, bg_color2);
  //create_rainbow_spectrum(&Screen);
  //create_triangle_gradient(&Screen, red, p1, green, p2, blue, p3);

  bool running = true;
  while (running) {
    update_key_states(&kb_state);

    if (key_pressed(&kb_state, 'q')) running = false;
    if (key_held(&kb_state, 'w')) player_Rect.pos.y --; 
    if (key_held(&kb_state, 'a')) player_Rect.pos.x --; 
    if (key_held(&kb_state, 's')) player_Rect.pos.y ++; 
    if (key_held(&kb_state, 'd')) player_Rect.pos.x ++; 

    fill_Surface(&Screen, red);
    blit_Surface(&player_Surface, &Screen, player_Rect);
    // draw code here
    update_Screen(&Screen);
    usleep(16000);
  }

  cleanup();
  cleanup_input();
  destroy_Surface(&player_Surface);
  destroy_Surface(&Screen);

  return 0;
}
