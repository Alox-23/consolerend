#include "consoleRenderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "input.h"

#define SCREEN_WIDTH 30
#define SCREEN_HEIGHT 30

struct Bird{
  struct Surface surface;
  struct Rect rect;
};

struct Pipe{
  struct Surface surface;
  struct Rect rect;
};

struct PipePair{
  struct Pipe top;
  struct Pipe bottom;
};

void draw_pipe_pair(struct PipePair *pipe_pair, struct Surface *s){
  blit_Surface(&pipe_pair->top.surface, s, pipe_pair->top.rect);
  blit_Surface(&pipe_pair->bottom.surface, s, pipe_pair->bottom.rect);
}

void init_pipe_pair(struct PipePair *pipe_pair, struct Rect rect, int gap){
  struct Rect top_r = {{rect.pos.x,rect.pos.y - SCREEN_HEIGHT - gap/2},{rect.size.x,rect.size.y}};
  struct Rect bottom_r = {{rect.pos.x,rect.pos.y + gap/2},{rect.size.x,rect.size.y}};

  struct Surface top_s;
  if (init_Surface(&top_s, top_r)){
    printf("Pipe pair top surface initialization error\n");
    return;
  }
  fill_Surface(&top_s, (struct Color){50, 200, 50});
  struct Surface bottom_s;
  if (init_Surface(&bottom_s, bottom_r)){
    printf("Pipe pair top surface initialization error\n");
    return;
  }
  fill_Surface(&bottom_s, (struct Color){50, 200, 50});

  pipe_pair->top.surface = top_s;
  pipe_pair->bottom.surface = bottom_s;
  pipe_pair->top.rect = top_r;
  pipe_pair->bottom.rect = bottom_r;
}

void update_pipe_pair(struct PipePair *pipe_pair){
  pipe_pair->top.rect.pos.x -= 1;
  pipe_pair->bottom.rect.pos.x -= 1;
}

void draw_bg(struct Surface *s){
  fill_Surface(s, (struct Color){150, 200, 255});
  draw_rect(s, (struct Rect){{0,SCREEN_HEIGHT-2}, {SCREEN_WIDTH, 2}}, (struct Color){200, 255, 200, 0});
}

int main(void) {
  struct Color red = {255, 0, 0};
  struct Color green = {0, 255, 0, 0};
  struct Color blue = {0, 0, 255, 0};
  struct Color grey = {50, 50, 50, 0};

  KeyboardState kb_state = {0};
  if (init_input()) {
    printf("Error initalizing KeyboardState\n");
    cleanup();
    return 1;
  }

  struct Surface Screen;
  struct Rect screenRect = {{0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT}};
  if (init_Surface(&Screen, screenRect)) {
    printf("Error initializing surface\n");
    cleanup();
    return 1;
  }

  struct PipePair pipe_pair;
  init_pipe_pair(&pipe_pair, (struct Rect){{SCREEN_WIDTH, SCREEN_HEIGHT/2}, {2, SCREEN_HEIGHT}}, 10);

  bool running = true;
  while (running) {
    update_key_states(&kb_state);

    if (key_pressed(&kb_state, 'q'))
      running = false;
    
    update_pipe_pair(&pipe_pair);
    draw_bg(&Screen);
    draw_pipe_pair(&pipe_pair, &Screen);
    update_Screen(&Screen);
    usleep(160000);
  }

  cleanup();
  cleanup_input();
  destroy_Surface(&Screen);

  return 0;
}
