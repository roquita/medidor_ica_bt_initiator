#include "stdio.h"
#include "stdint.h"
#include "esp_log.h"
#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "button.h"

on_button_arrow_up_pressed_t on_button_arrow_up_pressed_cb = NULL;
on_button_enter_pressed_t on_button_enter_pressed_cb = NULL;
on_button_arrow_down_pressed_t on_button_arrow_down_pressed_cb = NULL;

TimerHandle_t timer1 = NULL;
TimerHandle_t timer2 = NULL;
TimerHandle_t timer3 = NULL;

void timer_callback(TimerHandle_t xTimer)
{
    uint32_t gpio = (uint32_t)pvTimerGetTimerID(xTimer);
    gpio_intr_enable(gpio);
}

void timer_init()
{
    timer1 = xTimerCreate("arrow_up_pressed",
                          pdMS_TO_TICKS(300),
                          pdFALSE, (void *)15,
                          timer_callback);

    timer2 = xTimerCreate("enter_pressed",
                          pdMS_TO_TICKS(1000),
                          pdFALSE, (void *)16,
                          timer_callback);

    timer3 = xTimerCreate("arrow_down_pressed",
                          pdMS_TO_TICKS(300),
                          pdFALSE, (void *)4,
                          timer_callback);
}

void on_arrow_up_pressed(void *args)
{
    gpio_intr_disable(15);
    on_button_arrow_up_pressed_cb();
    xTimerStartFromISR(timer1, NULL);
}

void on_enter_pressed(void *args)
{
    gpio_intr_disable(16);
    on_button_enter_pressed_cb();

    xTimerStartFromISR(timer2, NULL);
}

void on_arrow_down_pressed(void *args)
{
    gpio_intr_disable(4);
    on_button_arrow_down_pressed_cb();

    xTimerStartFromISR(timer3, NULL);
}