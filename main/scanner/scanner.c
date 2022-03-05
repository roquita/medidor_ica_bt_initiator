#include "stdio.h"
#include <string.h>
#include "scanner.h"
#include "main.h"
#include "esp_log.h"

#define SCANNER_TAG "SCANNER_TAG"

uint32_t bt_dev_total = 0;
bt_device_t bt_dev_list[MAX_BT_DEVS] = {0};

bool maccomp(uint8_t *mac1, uint8_t *mac2)
{
    return (mac1[0] == mac2[0] &&
            mac1[1] == mac2[1] &&
            mac1[2] == mac2[2] &&
            mac1[3] == mac2[3] &&
            mac1[4] == mac2[4] &&
            mac1[5] == mac2[5]);
}

void maccpy(uint8_t *dest, uint8_t *src)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = src[3];
    dest[4] = src[4];
    dest[5] = src[5];
}

void erase_bt_dev(int index)
{
    bt_dev_list[index].taked_slot = false;
    bt_dev_list[index].rssi = 0;
    bt_dev_list[index].bt_active = false;
    bt_dev_list[index].paired = false;
    memset(bt_dev_list[index].mac, 0, MAC_STR_LEN);
    memset(bt_dev_list[index].name, 0, BT_DEVICE_NAME_MAX_LEN + 1);
}

void scanner_print_dev_list()
{
    int printed_devs = 0;
    ESP_LOGI(SCANNER_TAG, " ");
    ESP_LOGI(SCANNER_TAG, "Total %d devices", bt_dev_total);
    for (int i = 0; i < MAX_BT_DEVS; i++)
    {
        if (bt_dev_list[i].taked_slot == false)
            continue;

        ESP_LOGI(SCANNER_TAG, "Dev %d:", i);
        ESP_LOGI(SCANNER_TAG, "  Name: %s", bt_dev_list[i].name);
        ESP_LOGI(SCANNER_TAG, "  MAC: %02x:%02x:%02x:%02x:%02x:%02x",
                 bt_dev_list[i].mac[0], bt_dev_list[i].mac[1],
                 bt_dev_list[i].mac[2], bt_dev_list[i].mac[3],
                 bt_dev_list[i].mac[4], bt_dev_list[i].mac[5]);
        ESP_LOGI(SCANNER_TAG, "  RSSI: %d", bt_dev_list[i].rssi);

        printed_devs++;

        if (printed_devs >= bt_dev_total)
            break;
    }
}

bool scanner_add_new_device_mac(int *index, esp_bd_addr_t bda)
{
    *index = -1;
    if (bt_dev_total >= MAX_BT_DEVS)
        return false;

    // char bda_str[18];
    // sprintf(bda_str, "%02x:%02x:%02x:%02x:%02x:%02x",
    //         bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

    for (int i = 0; i < MAX_BT_DEVS; i++)
    {
        if (bt_dev_list[i].taked_slot == false)
            continue;

        // MAC ADDRESS MATCH FOUND
        if (maccomp(bt_dev_list[i].mac, bda) == true)
        {
            // set flags for existing device
            bt_dev_list[i].bt_active = true;
            *index = i;
            return false;
        }
    }

    for (int i = 0; i < MAX_BT_DEVS; i++)
    {
        // EMPTY SLOT FOUND
        if (bt_dev_list[i].taked_slot == false)
        {
            // add 1 to total count
            bt_dev_total++;
            // save new mac
            maccpy(bt_dev_list[i].mac, bda);
            // set flags for new device
            bt_dev_list[i].taked_slot = true;
            bt_dev_list[i].bt_active = true;
            *index = i;
            return true;
        }
    }

    return false;
}

bool scanner_remove_inactive_devs()
{
    bool devs_removed = false;
    int checked_bt_devs = 0;

    for (uint32_t i = 0; i < MAX_BT_DEVS; i++)
    {
        if (bt_dev_list[i].taked_slot == false)
            continue;

        if (bt_dev_list[i].bt_active == false)
        {
            // erase inactive devices
            erase_bt_dev(i);
            bt_dev_total--;
            devs_removed = true;
        }
        else
        {
            // clean flag for active devices
            bt_dev_list[i].bt_active = false;
        }

        // take care only for added devs
        checked_bt_devs++;
        if (checked_bt_devs >= bt_dev_total)
        {
            break;
        }
    }
    return devs_removed;
}

bool scanner_add_new_device_name(int index, void *val)
{
    strncpy(bt_dev_list[index].name, (char *)val, BT_DEVICE_NAME_MAX_LEN);
    return true;
}

bool scanner_add_new_device_rssi(int index, void *val)
{
    bt_dev_list[index].rssi = *(int8_t *)val;
    return true;
}


uint8_t *scanner_get_mac_from_index(int index)
{
    return bt_dev_list[index].mac;
}