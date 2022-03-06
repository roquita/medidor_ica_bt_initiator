#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#include "radio_callbacks.h"
#include "main.h"
#include "radio_uart_io.c"

#define RADIO_INIT_TAG "RADIO_INIT_TAG"

bool is_paired = false;
uint8_t dev_mac[6] = {0};
uint32_t handle = 0;

void radio_init(on_profile_init_t on_profile_init,
                on_discovery_stops_t on_discovery_stops,
                on_new_dev_name_t on_new_dev_name,
                on_new_dev_rssi_t on_new_dev_rssi,
                on_scan_dev_service_t on_scan_dev_service,
                on_cl_init_t on_cl_init,
                on_open_t on_open,
                on_pin_req_t on_pin_req,
                on_close_t on_close)
{
    on_profile_init_cb = on_profile_init;
    on_discovery_stops_cb = on_discovery_stops;
    on_new_dev_name_cb = on_new_dev_name;
    on_new_dev_rssi_cb = on_new_dev_rssi;
    on_scan_dev_service_cb = on_scan_dev_service;
    on_cl_init_cb = on_cl_init;
    on_open_cb = on_open;
    on_pin_req_cb = on_pin_req;
    on_close_cb = on_close;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK)
    {
        ESP_LOGE(RADIO_INIT_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK)
    {
        ESP_LOGE(RADIO_INIT_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK)
    {
        ESP_LOGE(RADIO_INIT_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK)
    {
        ESP_LOGE(RADIO_INIT_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK)
    {
        ESP_LOGE(RADIO_INIT_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK)
    {
        ESP_LOGE(RADIO_INIT_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(ESP_SPP_MODE_CB)) != ESP_OK)
    {
        ESP_LOGE(RADIO_INIT_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    esp_bt_dev_set_device_name(DEVICE_NAME);
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);

    radio_uart_ioc_init();
}

void radio_scan_dev_service()
{
    ESP_LOGI(RADIO_INIT_TAG, "Scanning dev service for: %02x:%02x:%02x:%02x:%02x:%02x\n",
             dev_mac[0], dev_mac[1], dev_mac[2], dev_mac[3], dev_mac[4], dev_mac[5]);
    // esp_spp_connect(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_MASTER, param->disc_comp.scn[0], mac);

    esp_spp_start_discovery(dev_mac);
}

void radio_start_connection()
{
    esp_spp_connect(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_MASTER, serv_channel, dev_mac);
}

void radio_start_disconnection()
{
    esp_spp_disconnect(handle);
}

void radio_send_pin(uint8_t *pin_code, uint8_t pin_size)
{
    esp_bt_gap_pin_reply(dev_mac, true, pin_size, pin_code);
}

void radio_start_discovery()
{
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 5, 0);
}

void radio_stop_discovery()
{
    esp_bt_gap_cancel_discovery();
}

void radio_set_as_paired()
{
    is_paired = true;
}

void radio_set_as_unpaired()
{
    is_paired = false;
}

bool radio_is_paired()
{
    return is_paired;
}
void radio_set_dev_mac(uint8_t *new_dev_mac)
{
    dev_mac[0] = new_dev_mac[0];
    dev_mac[1] = new_dev_mac[1];
    dev_mac[2] = new_dev_mac[2];
    dev_mac[3] = new_dev_mac[3];
    dev_mac[4] = new_dev_mac[4];
    dev_mac[5] = new_dev_mac[5];
}
uint8_t *radio_get_dev_mac()
{
    return dev_mac;
}

void radio_set_handle(uint32_t new_handle)
{
    handle = new_handle;
}

uint32_t radio_get_handle()
{
    return handle;
}