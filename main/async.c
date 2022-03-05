#include <string.h>
#include "main.h"

void on_profile_init(void)
{
    conc_msg_t msg;
    msg.signal = CONC_SIG_START_DISCOVERY;

    xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000));
}

void on_discovery_stops()
{
    SIGNAL_CLEAN_LISTS();

    // screen_update();
    SIGNAL_START_DISCOVERY();
}

void on_new_dev_name(uint8_t *bd_addr, void *val)
{
    dev_data_ctx *ctx = (dev_data_ctx *)malloc(sizeof(dev_data_ctx));

    uint8_t *bd_addr_copy = (uint8_t *)malloc(6);
    memcpy(bd_addr_copy, bd_addr, 6);

    char *name = (char *)malloc(strlen((char *)val) + 1);
    memcpy(name, val, strlen((char *)val) + 1);

    ctx->bd_addr = bd_addr_copy;
    ctx->val = name;

    conc_msg_t msg;
    msg.signal = CONC_SIG_SAVE_DEV_NAME;
    msg.data = ctx;

    xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000));
}

void on_new_dev_rssi(uint8_t *bd_addr, void *val)
{
    dev_data_ctx *ctx = (dev_data_ctx *)malloc(sizeof(dev_data_ctx));

    uint8_t *bd_addr_copy = (uint8_t *)malloc(6);
    memcpy(bd_addr_copy, bd_addr, 6);

    int8_t *rssi = (int8_t *)malloc(sizeof(int8_t));
    *rssi = *(int8_t *)val;

    ctx->bd_addr = bd_addr_copy;
    ctx->val = rssi;

    conc_msg_t msg;
    msg.signal = CONC_SIG_SAVE_DEV_RSSI;
    msg.data = ctx;

    xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000));
}

void on_scan_dev_service(bool success, void *dev_mac)
{
    if (success)
    {
        conc_msg_t msg;
        msg.signal = CONC_SIG_START_CONNECTION;
        msg.data = dev_mac;

        xQueueSendFromISR(conc_queue, &msg, NULL);
    }
    else
    {
    }
}

void on_cl_init(bool success)
{
    if (success)
    {
    }
    else
    {
    }
}

void on_open(bool success)
{
    if (success)
    {
        /*
        conc_msg_t msg;
        msg.signal = CONC_SIG_STOP_DISCOVERY;

        xQueueSendFromISR(conc_queue, &msg, NULL);
        */
    }
    else
    {
    }
}

void on_pin_req(void)
{
    SIGNAL_SEND_PIN();
}

void on_button_arrow_up_pressed()
{
    conc_msg_t msg;
    msg.signal = CONC_SIG_SCROLL_UP;

    xQueueSendFromISR(conc_queue, &msg, NULL);
}

void on_button_enter_pressed()
{
    conc_msg_t msg;
    msg.signal = CONC_SIG_SCAN_DEV_SERVICE;

    xQueueSendFromISR(conc_queue, &msg, NULL);
}

void on_button_arrow_down_pressed()
{
    conc_msg_t msg;
    msg.signal = CONC_SIG_SCROLL_DOWN;

    xQueueSendFromISR(conc_queue, &msg, NULL);
}