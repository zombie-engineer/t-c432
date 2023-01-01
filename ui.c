#include "ui.h"
#include "font.h"
#include "list.h"
#include <string.h>
#include "ssd1306.h"
#include "tim.h"
#include "pushbuttons.h"
#include "ui/widget.h"
#include "ui/main_widget.h"

#define UI_ID_MAIN 0
#define UI_ID_TEST1 1
#define UI_ID_TEST2 2
#define UI_ID_COUNT 3

struct widget main_widget;

struct widget *focus_widget = NULL;
struct widget *current_window = NULL;

void ui_update(void)
{
  // frame_counter++;
  dbuf_clear();
  focus_widget->draw(focus_widget);
  dbuf_flush();
}

void ui_callback_button_event_pressed(int button_id)
{
  focus_widget->handle_event(focus_widget, UI_EVENT_BUTTON_PRESSED,
    button_id);
}

void ui_callback_button_event_released(int button_id)
{
  focus_widget->handle_event(focus_widget, UI_EVENT_BUTTON_RELEASED,
    button_id);
}

void ui_init(void)
{
  main_widget_init(&main_widget);
  focus_widget = &main_widget;
}
