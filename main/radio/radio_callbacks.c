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
#include "main.h"
#include "radio_callbacks.h"

#define RADIO_CALLBACKS_TAG "RADIO_CALLBACKS_TAG"

on_profile_init_t on_profile_init_cb = NULL;
on_discovery_stops_t on_discovery_stops_cb = NULL;
on_new_dev_name_t on_new_dev_name_cb = NULL;
on_new_dev_rssi_t on_new_dev_rssi_cb = NULL;

void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)
    {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_INIT_EVT");
        on_profile_init_cb();

        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_DISCOVERY_COMP_EVT status=%d scn_num=%d", param->disc_comp.status, param->disc_comp.scn_num);
        /*
                if (param->disc_comp.status == ESP_SPP_SUCCESS)
                {
                    esp_spp_connect(sec_mask, role_master, param->disc_comp.scn[0], peer_bd_addr);
                }
        */
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_OPEN_EVT");

        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_DATA_IND_EVT");
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_CONG_EVT cong=%d", param->cong.cong);

        if (param->cong.cong == 0)
        {
        }
        break;
    case ESP_SPP_WRITE_EVT:

        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_WRITE_EVT len=%d cong=%d", param->write.len, param->write.cong);

        if (param->write.cong == 0)
        {
        }
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_SRV_OPEN_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_BT_GAP_DISC_RES_EVT:

        for (int i = 0; i < param->disc_res.num_prop; i++)
        {
            switch (param->disc_res.prop[i].type)
            {
            case ESP_BT_GAP_DEV_PROP_BDNAME /* NAME */:
                on_new_dev_name_cb(param->disc_res.bda, param->disc_res.prop[i].val);
                // strncpy(bt_dev_list[index].name, (char *)param->disc_res.prop[i].val, BT_DEVICE_NAME_MAX_LEN);
                break;
            case ESP_BT_GAP_DEV_PROP_RSSI /* RSSI */:
                on_new_dev_rssi_cb(param->disc_res.bda, param->disc_res.prop[i].val);
                // bt_dev_list[index].rssi = *(int8_t *)param->disc_res.prop[i].val;
                break;
            default:
                break;
            }
        }

        break;
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_BT_GAP_DISC_STATE_CHANGED_EVT: state=%d", param->disc_st_chg.state);

        if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED)
        {
            on_discovery_stops_cb();
        }
        else
        {
        }

        break;
    case ESP_BT_GAP_RMT_SRVCS_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_BT_GAP_RMT_SRVCS_EVT");
        break;
    case ESP_BT_GAP_RMT_SRVC_REC_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_BT_GAP_RMT_SRVC_REC_EVT");
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT:
    {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGI(RADIO_CALLBACKS_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(RADIO_CALLBACKS_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        }
        else
        {
            ESP_LOGE(RADIO_CALLBACKS_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:
    {
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit)
        {
            ESP_LOGI(RADIO_CALLBACKS_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        }
        else
        {
            ESP_LOGI(RADIO_CALLBACKS_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(RADIO_CALLBACKS_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d", param->mode_chg.mode);
        break;

    default:
        break;
    }
}