#include "stdio.h"
#include "button.h"
#include "scanner.h"
#include "screen.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include "radio_callbacks.h"
#include "string.h"
#include "radio.h"

void signal_start_discovery(conc_msg_t *msg)
{
    // update screen

    radio_start_discovery();
}

void signal_stop_discovery(conc_msg_t *msg)
{
    radio_stop_discovery();
}

void signal_clean_lists(conc_msg_t *msg)
{
    bool macs_removed = scanner_remove_inactive_devs();
    if (macs_removed)
    {
        screen_sync_data();
        scanner_print_dev_list();
    }
}

void signal_save_dev_name(conc_msg_t *msg)
{
    dev_data_ctx *ctx = (dev_data_ctx *)msg->data;

    int index;
    bool new_mac_added = scanner_add_new_device_mac(&index, ctx->bd_addr);
    if (new_mac_added)
    {
        screen_sync_data();
        scanner_print_dev_list();
    }

    if (index == -1)
        goto end;

    scanner_add_new_device_name(index, ctx->val);
    screen_update();

end:
    free(ctx->bd_addr);
    free(ctx->val);
    free(ctx);
}

void signal_save_dev_rssi(conc_msg_t *msg)
{
    dev_data_ctx *ctx = (dev_data_ctx *)msg->data;

    int index;
    bool new_mac_added = scanner_add_new_device_mac(&index, ctx->bd_addr);
    if (new_mac_added)
    {
        screen_sync_data();
        scanner_print_dev_list();
    }

    if (index == -1)
        goto end;

    scanner_add_new_device_rssi(index, ctx->val);
    screen_update();

end:
    free(ctx->bd_addr);
    free(ctx->val);
    free(ctx);
}

void signal_start_connection(conc_msg_t *msg)
{
    radio_start_connection();
}

void signal_send_pin(conc_msg_t *msg)
{
    uint8_t pin_code[5];

    pin_code[0] = '5';
    pin_code[1] = '5';
    pin_code[2] = '5';
    pin_code[3] = '5';
    pin_code[4] = '5';

    radio_send_pin(pin_code, 5);
}

void signal_scroll_up(conc_msg_t *msg)
{
    screen_sync_with_scroll_up();
    screen_update();

    scanner_print_dev_list();
}
void signal_scan_dev_service(conc_msg_t *msg)
{
    int index = screen_get_selected_index();
    uint8_t *mac = scanner_get_mac_from_index(index);

    radio_scan_dev_service(mac);

    printf("Button enter pressed\n");
}
void signal_scroll_down(conc_msg_t *msg)
{
    screen_sync_with_scroll_down();
    screen_update();

    scanner_print_dev_list();
}