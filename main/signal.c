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

extern bool need_new_conn;
bool are_buttons_blocked = false;
bool are_scanner_blocked = false;

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
    if (are_scanner_blocked)
        return;

    bool macs_removed = scanner_remove_inactive_devs();
    if (macs_removed)
    {
        screen_sync_data();
        // scanner_print_dev_list();
    }

    if (!are_buttons_blocked)
        screen_update();
}

void signal_save_dev_name(conc_msg_t *msg)
{
    dev_data_ctx *ctx = (dev_data_ctx *)msg->data;

    if (are_scanner_blocked)
        goto end;

    int index;
    bool new_mac_added = scanner_add_new_device_mac(&index, ctx->bd_addr);
    if (new_mac_added)
    {
        screen_sync_data();
        // scanner_print_dev_list();
    }

    if (index == -1)
        goto end;

    scanner_add_new_device_name(index, ctx->val);
    // screen_update();

end:
    free(ctx->bd_addr);
    free(ctx->val);
    free(ctx);
}

void signal_save_dev_rssi(conc_msg_t *msg)
{
    dev_data_ctx *ctx = (dev_data_ctx *)msg->data;

    if (are_scanner_blocked)
        goto end;

    int index;
    bool new_mac_added = scanner_add_new_device_mac(&index, ctx->bd_addr);
    if (new_mac_added)
    {
        screen_sync_data();
        // scanner_print_dev_list();
    }

    if (index == -1)
        goto end;

    scanner_add_new_device_rssi(index, ctx->val);

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

void signal_set_paired(conc_msg_t *msg)
{
    uint8_t *dev_addr = (uint8_t *)msg->data;
    scanner_set_device_as_paired(dev_addr);
    radio_set_as_paired();

    free(dev_addr);

    // if (!are_buttons_blocked)
    //    screen_update();
}

void signal_set_unpaired(conc_msg_t *msg)
{
    uint8_t *dev_addr = radio_get_dev_mac();
    scanner_set_device_as_unpaired(dev_addr);
    radio_set_as_unpaired();

    uint8_t null_addr[6] = {0};
    radio_set_dev_mac(null_addr);

    // if (!are_buttons_blocked)
    // screen_update();
}

void signal_unblock_buttons(conc_msg_t *msg)
{
    are_buttons_blocked = false;

    screen_update();
}

void signal_unblock_scanner(conc_msg_t *msg)
{
    are_scanner_blocked = false;
}

void signal_set_handle(conc_msg_t *msg)
{
    uint32_t handle = (uint32_t)msg->data;
    radio_set_handle(handle);
}

void signal_scroll_up(conc_msg_t *msg)
{
    if (are_buttons_blocked)
        return;

    screen_sync_with_scroll_up();
    screen_update();

    // scanner_print_dev_list();
}

void signal_set_dev_target(conc_msg_t *msg)
{
    if (are_buttons_blocked)
        return;

    int index = screen_get_selected_index();

    uint8_t *sel_dev_addr = scanner_get_mac_from_index(index);

    if (radio_is_paired())
    {
        uint8_t *paired_dev_addr = radio_get_dev_mac();
        if (maccomp(sel_dev_addr, paired_dev_addr) == true)
        {
            // to disconnect
            radio_start_disconnection();
        }
        else
        {
            // to connect
            need_new_conn = true;            
            // to disconnect
            radio_start_disconnection();
        }
    }
    else
    {
        are_scanner_blocked = true;
        are_buttons_blocked = true;
        screen_clean();
        screen_print_pairing();

        // to connect
        radio_set_dev_mac(sel_dev_addr);
        radio_scan_dev_service();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void signal_scroll_down(conc_msg_t *msg)
{
    if (are_buttons_blocked)
        return;

    screen_sync_with_scroll_down();
    screen_update();

    // scanner_print_dev_list();
}