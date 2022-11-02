#pragma once
#include <stdbool.h>

#define PUSHBUTTON_A 8
#define PUSHBUTTON_B 9

void pushbuttons_init(void);

bool pushbotton_a_is_pressed(void);
bool pushbotton_b_is_pressed(void);
bool pushbotton_c_is_pressed(void);
