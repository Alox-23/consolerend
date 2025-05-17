#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
  
typedef struct {
    bool keys[128];        // Current key states
    bool prev_keys[128];   // Previous key states
    bool key_pressed[128]; // Key pressed this frame
    bool key_released[128];// Key released this frame
} KeyboardState;

  
// Initializes the input system
int init_input();

// Cleans up the input system
void cleanup_input();

// Updates keyboard statevoid update_key_states(KeyboardState* kb_state);
void update_key_states(KeyboardState* kb_state);

// Checks if a key is currently held
bool key_held(const KeyboardState* kb_state, char key);

// Checks if a key was pressed this frame
bool key_pressed(const KeyboardState* kb_state, char key);

// Checks if a key was released this frame
bool key_released(const KeyboardState* kb_state, char key);

#endif
