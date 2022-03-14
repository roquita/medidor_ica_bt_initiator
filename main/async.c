#include <string.h>
#include "main.h"

bool need_new_conn = false;
bool is_pared = false;
extern bool are_scanner_blocked;

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

    if (!is_pared)
    {
        SIGNAL_START_DISCOVERY();
    }
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

void on_scan_dev_service(bool success)
{
    if (success)
    {
        conc_msg_t msg;
        msg.signal = CONC_SIG_START_CONNECTION;
        xQueueSendFromISR(conc_queue, &msg, NULL);
    }
    else
    {
        CONC_SIG_UNBLOCK_SCANNER();
        CONC_SIG_UNBLOCK_BUTTONS();
    }
}

void on_cl_init(bool success, uint32_t handle)
{
    if (success)
    {
        CONC_SIG_SET_HANDLE(handle);
    }
    else
    {
        CONC_SIG_UNBLOCK_SCANNER();
        CONC_SIG_UNBLOCK_BUTTONS();
    }
}

void on_open(bool success, uint8_t *dev_addr)
{
    if (success)
    {
        is_pared = true;

        uint8_t *dev_addr_copy = (uint8_t *)malloc(6);
        memcpy(dev_addr_copy, dev_addr, 6);

        CONC_SIG_SET_PAIRED(dev_addr_copy);
        CONC_SIG_STOP_DISCOVERY();
    }
    else
    {
        CONC_SIG_UNBLOCK_SCANNER();
    }

    CONC_SIG_UNBLOCK_BUTTONS();
}

void on_pin_req(void)
{
    SIGNAL_SEND_PIN();
}

void on_close()
{
    is_pared = false;

    CONC_SIG_SET_UNPAIRED();
    CONC_SIG_SET_HANDLE(0);
    CONC_SIG_UNBLOCK_BUTTONS();

    if (need_new_conn)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));
        need_new_conn = false;
        CONC_SIG_SET_DEV_TARGET();
    }
    else
    {
        
        CONC_SIG_UNBLOCK_SCANNER();
        SIGNAL_START_DISCOVERY();
    }
}

void on_button_arrow_up_pressed()
{
    conc_msg_t msg;
    msg.signal = CONC_SIG_SCROLL_UP;

    xQueueSendFromISR(conc_queue, &msg, NULL);
}

void on_button_enter_pressed()
{
    CONC_SIG_SET_DEV_TARGET();
}

void on_button_arrow_down_pressed()
{
    conc_msg_t msg;
    msg.signal = CONC_SIG_SCROLL_DOWN;

    xQueueSendFromISR(conc_queue, &msg, NULL);
}