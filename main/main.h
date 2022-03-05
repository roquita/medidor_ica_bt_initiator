#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define DEVICE_NAME "concentrator"
#define MAX_BT_DEVS 100
#define BT_DEVICE_NAME_MAX_LEN 19
#define MAC_STR_LEN 18

typedef enum
{
    CONC_SIG_START_DISCOVERY,
    CONC_SIG_STOP_DISCOVERY,
    CONC_SIG_CLEAN_LISTS,
    CONC_SIG_SAVE_DEV_NAME,
    CONC_SIG_SAVE_DEV_RSSI,
    CONC_SIG_START_CONNECTION,
    CONC_SIG_SEND_PIN,
    CONC_SIG_SET_PAIRED,
    CONC_SIG_SET_UNPAIRED,
    CONC_SIG_SET_HANDLE,
    CONC_SIG_SCROLL_UP,
    CONC_SIG_SET_DEV_TARGET,
    CONC_SIG_SCROLL_DOWN,

} conc_signal_t;
typedef struct
{
    void *data;
    conc_signal_t signal;
} conc_msg_t;

typedef struct
{
    uint8_t *bd_addr;
    void *val;
} dev_data_ctx;

extern QueueHandle_t conc_queue;

#define SIGNAL_START_DISCOVERY()                           \
    do                                                     \
    {                                                      \
        conc_msg_t msg;                                    \
        msg.signal = CONC_SIG_START_DISCOVERY;             \
        xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000)); \
    } while (0)

#define SIGNAL_CLEAN_LISTS()                               \
    do                                                     \
    {                                                      \
        conc_msg_t msg;                                    \
        msg.signal = CONC_SIG_CLEAN_LISTS;                 \
        xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000)); \
    } while (0)

#define SIGNAL_SEND_PIN()                                  \
    do                                                     \
    {                                                      \
        conc_msg_t msg;                                    \
        msg.signal = CONC_SIG_SEND_PIN;                    \
        xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000)); \
    } while (0)

#define CONC_SIG_SET_PAIRED(dev_addr)                      \
    do                                                     \
    {                                                      \
        conc_msg_t msg;                                    \
        msg.signal = CONC_SIG_SET_PAIRED;                  \
        msg.data = dev_addr;                               \
        xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000)); \
    } while (0)

#define CONC_SIG_SET_UNPAIRED()                            \
    do                                                     \
    {                                                      \
        conc_msg_t msg;                                    \
        msg.signal = CONC_SIG_SET_UNPAIRED;                \
        xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000)); \
    } while (0)

#define CONC_SIG_SET_HANDLE(handle)                        \
    do                                                     \
    {                                                      \
        conc_msg_t msg;                                    \
        msg.signal = CONC_SIG_SET_HANDLE;                  \
        msg.data = (void *)handle;                         \
        xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000)); \
    } while (0)

#define CONC_SIG_SET_DEV_TARGET()                          \
    do                                                     \
    {                                                      \
        conc_msg_t msg;                                    \
        msg.signal = CONC_SIG_SET_DEV_TARGET;              \
        xQueueSend(conc_queue, &msg, pdMS_TO_TICKS(1000)); \
    } while (0)
