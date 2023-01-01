#pragma once
#include <stdbool.h>

#define PUSHBUTTON_ID_LEFT 0
#define PUSHBUTTON_ID_RIGHT 1
#define PUSHBUTTON_ID_MID 2
#define PUSHBUTTON_ID_COUNT (PUSHBUTTON_ID_MID + 1)

void pushbuttons_init(void);

bool pushbotton_is_pressed(int id);
