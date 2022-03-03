#include "main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

typedef struct
{
    char mac[MAC_STR_LEN];
    char name[BT_DEVICE_NAME_MAX_LEN + 1];
    int8_t rssi;
    bool taked_slot;
    bool bt_active;
} bt_device_t;

extern uint32_t bt_dev_total;
extern bt_device_t bt_dev_list[MAX_BT_DEVS];

void scanner_print_dev_list();
void scanner_remove_inactive_devs();
void scanner_add_new_device_name(esp_bd_addr_t bd_addr, void *val);
void scanner_add_new_device_rssi(esp_bd_addr_t bd_addr, void *val);
void scanner_start();
