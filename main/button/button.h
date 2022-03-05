#pragma once

typedef void (*on_button_arrow_up_pressed_t)(void);
typedef void (*on_button_enter_pressed_t)(void);
typedef void (*on_button_arrow_down_pressed_t)(void);

void button_init(on_button_arrow_up_pressed_t,
                 on_button_enter_pressed_t,
                 on_button_arrow_down_pressed_t);