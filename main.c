#include "consoleRenderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

// Enable non-blocking input
void enable_raw_mode() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Restore normal terminal mode
void disable_raw_mode() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Check if key pressed (non-blocking)
int kbhit() {
  struct timeval tv = {0, 0};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

// Get pressed key (if any)
char getch() {
  char buf = 0;
  read(STDIN_FILENO, &buf, 1);
  return buf;
}

int main(void) {
  enable_raw_mode();
  struct Surface Screen;
  struct Rect screenRect = {{0, 0}, {350, 250}};
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

  if (init_Surface(&Screen, screenRect)){
    printf("Error initializing surface\n");
    cleanup();
    disable_raw_mode();
    return 1;
  }

  struct Surface player_Surface;
  struct Rect player_Rect = {{5, 5}, {5, 5}};
  if (init_Surface(&player_Surface, player_Rect))
  {
    cleanup();
    disable_raw_mode();
    return 1;
  }
  fill_Surface(&player_Surface, green);
  
  //fill_gradient_Surface(&Screen, bg_color, bg_color2);
  //create_rainbow_spectrum(&Screen);
  //create_triangle_gradient(&Screen, red, p1, green, p2, blue, p3);

  bool running = true;
  while (running) {
    if (kbhit()) {
      char key = getch();
      switch (key) {
        case 'q': running = false; break;
        case 'w': player_Rect.pos.y --; break;
        case 'a': player_Rect.pos.x --; break;
        case 's': player_Rect.pos.y ++; break;
        case 'd': player_Rect.pos.x ++; break;
      }
    }
    fill_Surface(&Screen, red);
    blit_Surface(&player_Surface, &Screen, player_Rect);
    // draw code here
    update_Screen(&Screen);
    usleep(16000);
  }

  cleanup();
  destroy_Surface(&player_Surface);
  destroy_Surface(&Screen);
  disable_raw_mode();

  return 0;
}
