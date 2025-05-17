#include "input.h"
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <stdio.h>

static struct termios original_termios;

static int kbhit() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

static char getch() {
    char buf = 0;
    if (read(STDIN_FILENO, &buf, 1) < 1) {
        return 0;
    }
    return buf;
}

int init_input() {
    // Save original terminal settings and set raw mode
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        perror("tcgetattr");
        return 1;
    }

    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        perror("tcsetattr");
        return 1;
    }

    return 0;
}

void cleanup_input() {
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void update_key_states(KeyboardState* kb_state) {
    if (!kb_state) return;

    // Save current states to previous
    memcpy(kb_state->prev_keys, kb_state->keys, sizeof(kb_state->keys));
    
    // Clear current key states
    memset(kb_state->keys, false, sizeof(kb_state->keys));
    memset(kb_state->key_pressed, false, sizeof(kb_state->key_pressed));
    memset(kb_state->key_released, false, sizeof(kb_state->key_released));
    
    // Get new key presses
    while (kbhit()) {
        char key = getch();
        if (key > 0 && key < 128) {
            kb_state->keys[(unsigned char)key] = true;
        }
    }
    
    // Calculate pressed/released states
    for (int i = 0; i < 128; i++) {
        kb_state->key_pressed[i] = kb_state->keys[i] && !kb_state->prev_keys[i];
        kb_state->key_released[i] = !kb_state->keys[i] && kb_state->prev_keys[i];
    }
}

bool key_held(const KeyboardState* kb_state, char key) {
    return kb_state && key > 0 && key < 128 && kb_state->keys[(unsigned char)key];
}

bool key_pressed(const KeyboardState* kb_state, char key) {
    return kb_state && key > 0 && key < 128 && kb_state->key_pressed[(unsigned char)key];
}

bool key_released(const KeyboardState* kb_state, char key) {
    return kb_state && key > 0 && key < 128 && kb_state->key_released[(unsigned char)key];
}
