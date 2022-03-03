#include "stdio.h"
#include <string.h>
#include "scanner.h"
#include "main.h"
#include "esp_log.h"

#define SCANNER_TAG "SCANNER_TAG"

uint32_t bt_dev_total = 0;
bt_device_t bt_dev_list[MAX_BT_DEVS] = {0};

void erase_bt_dev(int index)
{
    bt_dev_list[index].taked_slot = false;
    bt_dev_list[index].rssi = 0;
    bt_dev_list[index].bt_active = false;
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
        ESP_LOGI(SCANNER_TAG, "  MAC: %s", bt_dev_list[i].mac);
        ESP_LOGI(SCANNER_TAG, "  RSSI: %d", bt_dev_list[i].rssi);

        printed_devs++;

        if (printed_devs >= bt_dev_total)
            break;
    }
}

int set_index_for_new_bt_dev(esp_bd_addr_t bda)
{
    if (bt_dev_total >= MAX_BT_DEVS)
        return -1;

    char bda_str[18];
    sprintf(bda_str, "%02x:%02x:%02x:%02x:%02x:%02x",
            bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

    for (int i = 0; i < MAX_BT_DEVS; i++)
    {
        // EMPTY SLOT FOUND
        if (bt_dev_list[i].taked_slot == false)
        {
            // add 1 to total count
            bt_dev_total++;
            // save new mac
            strcpy(bt_dev_list[i].mac, bda_str);
            // set flags for new device
            bt_dev_list[i].taked_slot = true;
            bt_dev_list[i].bt_active = true;
            return i;
        }

        // MAC ADDRESS MATCH FOUND
        if (strcmp(bt_dev_list[i].mac, bda_str) == 0)
        {
            // set flags for existing device
            bt_dev_list[i].bt_active = true;
            return i;
        }
    }

    return -1;
}

void scanner_remove_inactive_devs()
{
    int checked_bt_devs = 0;

    for (uint32_t i = 0; i < MAX_BT_DEVS; i++)
    {
        if (bt_dev_list[i].taked_slot == true && bt_dev_list[i].bt_active == false)
        {
            // erase inactive devices
            erase_bt_dev(i);
            bt_dev_total--;
        }
        else
        {
            // clean flag for active devices
            bt_dev_list[i].bt_active = false;
            checked_bt_devs++;
        }

        // take care only for added devs
        if (checked_bt_devs >= bt_dev_total)
        {
            break;
        }
    }
}

void scanner_add_new_device_name(esp_bd_addr_t bd_addr, void *val)
{
    int index = set_index_for_new_bt_dev(bd_addr);
    if (index == -1)
    {
        ESP_LOGE(SCANNER_TAG, "Too many devices");
        return;
    }

    strncpy(bt_dev_list[index].name, (char *)val, BT_DEVICE_NAME_MAX_LEN);
}

void scanner_add_new_device_rssi(esp_bd_addr_t bd_addr, void *val)
{
    int index = set_index_for_new_bt_dev(bd_addr);
    if (index == -1)
    {
        ESP_LOGE(SCANNER_TAG, "Too many devices");
        return;
    }

    bt_dev_list[index].rssi = *(int8_t *)val;
}

void scanner_start()
{
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 5, 0);
}

/*

void scanner_add_device_chrs_to_list(esp_bt_gap_cb_param_t *param)
{

    for (int i = 0; i < param->disc_res.num_prop; i++)
    {
        switch (param->disc_res.prop[i].type)
        {
        case ESP_BT_GAP_DEV_PROP_BDNAME :
            // ESP_LOGI(SCANNER_TAG, " --- name %s ", (char *)param->disc_res.prop[i].val);
            strncpy(bt_dev_list[index].name, (char *)param->disc_res.prop[i].val, BT_DEVICE_NAME_MAX_LEN);
            break;
        case ESP_BT_GAP_DEV_PROP_RSSI :
            // ESP_LOGI(SCANNER_TAG, " --- rssi %d ", *(int8_t *)param->disc_res.prop[i].val);
            bt_dev_list[index].rssi = *(int8_t *)param->disc_res.prop[i].val;
            break;
        default:
            break;
        }
    }
}
*/