#pragma once
#include <stdbool.h>

#define PUSHBUTTON_ID_MAIN 0
#define PUSHBUTTON_ID_COUNT (PUSHBUTTON_ID_MAIN + 1)

#define PUSHBUTTON_EVENT_PRESSED 0
#define PUSHBUTTON_EVENT_RELEASED 1

void pushbuttons_init(void);

bool pushbotton_is_pressed(int id);

typedef void (*pushbutton_event_callback_t)(int button_id, int event);

bool pushbutton_register_callback(int button_id, int event,
  pushbutton_event_callback_t cb);
