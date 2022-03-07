#include "signal.c"
#include "async.c"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "radio.h"
#include "scanner.h"
#include "button.h"

QueueHandle_t conc_queue = NULL;

// static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
// static const esp_spp_role_t role_master = ESP_SPP_ROLE_MASTER;

// esp_bd_addr_t peer_bd_addr = {0};
// static uint8_t peer_bdname_len;
// static char peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
// static const char remote_device_name[] = "ESP_SPP_ACCEPTOR";
// static const uint8_t inq_num_rsps = 0;

void app_main(void)
{
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
