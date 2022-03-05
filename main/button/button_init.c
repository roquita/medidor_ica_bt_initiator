
#include "button_callbacks.c"
#include "driver/gpio.h"
#include "button.h"

extern on_button_arrow_up_pressed_t on_button_arrow_up_pressed_cb;
extern on_button_enter_pressed_t on_button_enter_pressed_cb;
extern on_button_arrow_down_pressed_t on_button_arrow_down_pressed_cb;

void button_init(on_button_arrow_up_pressed_t on_button_arrow_up_pressed,
                 on_button_enter_pressed_t on_button_enter_pressed,
                 on_button_arrow_down_pressed_t on_button_arrow_down_pressed)
{
    on_button_arrow_up_pressed_cb = on_button_arrow_up_pressed;
    on_button_enter_pressed_cb = on_button_enter_pressed;
    on_button_arrow_down_pressed_cb = on_button_arrow_down_pressed;

    const gpio_config_t arrow_up_gpio = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << 15),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&arrow_up_gpio);

    const gpio_config_t enter_gpio = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << 16),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&enter_gpio);

    const gpio_config_t arrow_down_gpio = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << 4),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&arrow_down_gpio);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(15, on_arrow_up_pressed, NULL);
    gpio_isr_handler_add(16, on_enter_pressed, NULL);
    gpio_isr_handler_add(4, on_arrow_down_pressed, NULL);

    timer_init();
}