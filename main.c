#include "consoleRenderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "input.h"

#define SCREEN_WIDTH 75
#define SCREEN_HEIGHT 100

struct Bird {
  double score;
  bool alive;
  struct Surface surface;
  double gravity;
  double vertical_velocity;
  struct Rect rect;
};

struct Pipe {
  struct Surface surface;
  struct Rect rect;
};

struct PipePair {
  struct Pipe top;
  struct Pipe bottom;
};

void draw_pipe_pair(struct PipePair *pipe_pair, struct Surface *s);
void destroy_pipe_pair(struct PipePair *pipe_pair);
void init_pipe_pair(struct PipePair *pipe_pair, struct Rect rect, int gap);
void update_pipe_pair(struct PipePair *pipe_pair);
void draw_bg(struct Surface *s);
void init_bird(struct Bird *bird, struct Rect rect);
void draw_bird(struct Bird *bird, struct Surface *s);
void update_bird(struct Bird *bird);
void destroy_bird(struct Bird *bird);
bool collide(struct Rect rect1, struct Rect rect2);
int generate_random_number(int min, int max);

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

  int pipe_width = SCREEN_HEIGHT / 10;
  int pipe_gap = 30;
  struct PipePair pipe_pair;
  init_pipe_pair(
      &pipe_pair,
      (struct Rect){{SCREEN_WIDTH, generate_random_number(
                                       pipe_gap, SCREEN_HEIGHT - pipe_gap)},
                    {pipe_width, SCREEN_HEIGHT}},
      pipe_gap);

  int num_birds = 10;
  struct Bird birds[num_birds];
  for (int i = 0; i < num_birds; i++){
    init_bird(&birds[i], (struct Rect){{10*i + 5, SCREEN_HEIGHT / 2}, {2, 2}});
  }
  
  bool running = true;
  while (running) {
    update_key_states(&kb_state);
      
    draw_bg(&Screen);

    if (pipe_pair.top.rect.pos.x < -pipe_width) {
      init_pipe_pair(
          &pipe_pair,
          (struct Rect){{SCREEN_WIDTH, generate_random_number(
                                           pipe_gap, SCREEN_HEIGHT - pipe_gap)},
                        {pipe_width, SCREEN_HEIGHT}},
          pipe_gap);
    }

    running = false;
    for (int i = 0; i < num_birds; i++){
      if (birds[i].rect.pos.y + birds[i].rect.size.y > SCREEN_HEIGHT - SCREEN_HEIGHT / 10){
        fill_Surface(&birds[i].surface, (struct Color){255, 0, 0});
        birds[i].alive = false;
        birds[i].vertical_velocity = 1;
      }
      if (collide(birds[i].rect, pipe_pair.top.rect)){
        fill_Surface(&birds[i].surface, (struct Color){255, 0, 0});
        birds[i].alive = false;
        birds[i].vertical_velocity = 1;
      }
      if (collide(birds[i].rect, pipe_pair.bottom.rect)){
        fill_Surface(&birds[i].surface, (struct Color){255, 0, 0});
        birds[i].alive = false;
        birds[i].vertical_velocity = 1;
      }
      update_bird(&birds[i]);
      draw_bird(&birds[i], &Screen);
      if (birds[i].alive){
        if (key_pressed(&kb_state, 'w')){
          if (birds[i].alive){
            birds[i].vertical_velocity = -1.5;
          }
        }
        running = true;
      }
    }

    if (key_pressed(&kb_state, 'q'))
      running = false;

    update_pipe_pair(&pipe_pair);
    draw_pipe_pair(&pipe_pair, &Screen);
    update_Screen(&Screen);
    usleep(32000);
  }
  
  for (int i = 0; i < 5; i++){
    set_alpha(&Screen, i);
    update_Screen(&Screen);
    usleep(160000);
  }
  
  cleanup();
  cleanup_input();
  destroy_Surface(&Screen);
  destroy_pipe_pair(&pipe_pair);
  for (int i = 0; i < num_birds; i++){
    destroy_bird(&birds[i]);
    printf("%f\n", birds[i].score);
  }
  return 0;
}

bool collide(struct Rect a, struct Rect b) {
    bool x_overlap = (a.pos.x < b.pos.x + b.size.x) && (a.pos.x + a.size.x > b.pos.x);
    bool y_overlap = (a.pos.y < b.pos.y + b.size.y) && (a.pos.y + a.size.y > b.pos.y);
    return x_overlap && y_overlap;
}

int generate_random_number(int min, int max) {
  // Seed the random number generator with current time
  // Note: This should only be done once in your program
  static int seeded = 0;
  if (!seeded) {
    srand(time(NULL));
    seeded = 1;
  }

  // Generate random number in the range [min, max]
  return min + rand() % (max - min + 1);
}

void init_bird(struct Bird *bird, struct Rect rect) {
  bird->rect = rect;
  bird->alive = true;
  if (init_Surface(&bird->surface, rect)) {
    printf("Error initializing bird syrface\n");
  }
  fill_Surface(&bird->surface, (struct Color){255, 255, 0});
  bird->gravity = 0.1;
}

void destroy_bird(struct Bird *bird){
  destroy_Surface(&bird->surface);
}

void draw_bird(struct Bird *bird, struct Surface *s) {
  blit_Surface(&bird->surface, s, bird->rect);
}

void update_bird(struct Bird *bird) {
  bird->vertical_velocity += bird->gravity;
  if (bird->vertical_velocity > 2) {
    bird->vertical_velocity = 2;
  }

  if (bird->alive){
    bird->score += 0.01;
  }

  bird->rect.pos.y += bird->vertical_velocity;
}

void draw_pipe_pair(struct PipePair *pipe_pair, struct Surface *s) {
  blit_Surface(&pipe_pair->top.surface, s, pipe_pair->top.rect);
  blit_Surface(&pipe_pair->bottom.surface, s, pipe_pair->bottom.rect);
}

void destroy_pipe_pair(struct PipePair *pipe_pair) {
  destroy_Surface(&pipe_pair->top.surface);
  destroy_Surface(&pipe_pair->bottom.surface);
}

void init_pipe_pair(struct PipePair *pipe_pair, struct Rect rect, int gap) {
  struct Rect top_r = {{rect.pos.x, rect.pos.y - SCREEN_HEIGHT - gap / 2},
                       {rect.size.x, rect.size.y}};
  struct Rect bottom_r = {{rect.pos.x, rect.pos.y + gap / 2},
                          {rect.size.x, rect.size.y}};

  struct Surface top_s;
  if (init_Surface(&top_s, top_r)) {
    printf("Pipe pair top surface initialization error\n");
    return;
  }
  fill_Surface(&top_s, (struct Color){50, 200, 50});
  struct Surface bottom_s;
  if (init_Surface(&bottom_s, bottom_r)) {
    printf("Pipe pair top surface initialization error\n");
    return;
  }
  fill_Surface(&bottom_s, (struct Color){50, 200, 50});

  pipe_pair->top.surface = top_s;
  pipe_pair->bottom.surface = bottom_s;
  pipe_pair->top.rect = top_r;
  pipe_pair->bottom.rect = bottom_r;
}

void update_pipe_pair(struct PipePair *pipe_pair) {
  pipe_pair->top.rect.pos.x -= 1;
  pipe_pair->bottom.rect.pos.x -= 1;
}

void draw_bg(struct Surface *s) {
  fill_Surface(s, (struct Color){150, 200, 255});
  draw_rect(s,
            (struct Rect){{0, SCREEN_HEIGHT - SCREEN_HEIGHT / 10},
                          {SCREEN_WIDTH, SCREEN_HEIGHT / 10}},
            (struct Color){200, 255, 200, 0});
}