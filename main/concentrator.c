#include "signal.c"
#include "async.c"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "radio.h"
#include "scanner.h"
#include "button.h"
#include "driver/gpio.h"

QueueHandle_t conc_queue = NULL;

void task_led(void *arg)
{
#define LED_PIN GPIO_NUM_2

    const gpio_config_t led = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1UL << LED_PIN,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&led);

    while (1)
    {
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(task_led, "task_led", 1024 * 2, NULL, 1, NULL, 1);

    conc_queue = xQueueCreate(20, sizeof(conc_msg_t));

    button_init(on_button_arrow_up_pressed,
                on_button_enter_pressed,
                on_button_arrow_down_pressed);

    radio_init(on_profile_init,
               on_discovery_stops,
               on_new_dev_name,
               on_new_dev_rssi,
               on_scan_dev_service,
               on_cl_init,
               on_open,
               on_pin_req,
               on_close);

    screen_init();
    screen_clean();
    screen_print_starting();
    vTaskDelay(pdMS_TO_TICKS(1000));
    screen_update();

    while (1)
    {
        conc_msg_t msg;
        xQueueReceive(conc_queue, &msg, portMAX_DELAY);

        switch (msg.signal)
        {
        case CONC_SIG_START_DISCOVERY:
            signal_start_discovery(&msg);
            break;
        case CONC_SIG_STOP_DISCOVERY:
            signal_stop_discovery(&msg);
            break;
        case CONC_SIG_CLEAN_LISTS:
            signal_clean_lists(&msg);
            break;
        case CONC_SIG_SAVE_DEV_NAME:
            signal_save_dev_name(&msg);
            break;
        case CONC_SIG_SAVE_DEV_RSSI:
            signal_save_dev_rssi(&msg);
            break;
        case CONC_SIG_START_CONNECTION:
            signal_start_connection(&msg);
            break;
        case CONC_SIG_SEND_PIN:
            signal_send_pin(&msg);
            break;
        case CONC_SIG_SET_PAIRED:
            signal_set_paired(&msg);
            break;
        case CONC_SIG_SET_UNPAIRED:
            signal_set_unpaired(&msg);
            break;
        case CONC_SIG_UNBLOCK_BUTTONS:
            signal_unblock_buttons(&msg);
            break;
        case CONC_SIG_UNBLOCK_SCANNER:
            signal_unblock_scanner(&msg);
            break;
        case CONC_SIG_SET_HANDLE:
            signal_set_handle(&msg);
            break;
        case CONC_SIG_SCROLL_UP:
            signal_scroll_up(&msg);
            break;
        case CONC_SIG_SET_DEV_TARGET:
            signal_set_dev_target(&msg);
            break;
        case CONC_SIG_SCROLL_DOWN:
            signal_scroll_down(&msg);
            break;

        default:
            break;
        }
    }
}
