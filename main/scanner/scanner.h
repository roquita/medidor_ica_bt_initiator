#pragma once

#include "main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

typedef struct
{
    uint8_t mac[MAC_STR_LEN];
    char name[BT_DEVICE_NAME_MAX_LEN + 1];    
    int8_t rssi;
    bool taked_slot;
    bool bt_active;
    bool paired;
} bt_device_t;

extern uint32_t bt_dev_total;
extern bt_device_t bt_dev_list[MAX_BT_DEVS];

void scanner_print_dev_list();
bool scanner_add_new_device_mac(int *index, esp_bd_addr_t bda);
bool scanner_remove_inactive_devs();
bool scanner_add_new_device_name(int index, void *val);
bool scanner_add_new_device_rssi(int index, void *val);
uint8_t *scanner_get_mac_from_index(int index);
