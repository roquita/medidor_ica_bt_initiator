#pragma once
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

typedef void (*on_profile_init_t)(void);
typedef void (*on_discovery_stops_t)(void);
typedef void (*on_new_dev_name_t)(esp_bd_addr_t bd_addr, void *val);
typedef void (*on_new_dev_rssi_t)(esp_bd_addr_t bd_addr, void *val);
typedef void (*on_scan_dev_service_t)(bool success);
typedef void (*on_cl_init_t)(bool success, uint32_t handle);
typedef void (*on_open_t)(bool success, esp_bd_addr_t bd_addr);
typedef void (*on_pin_req_t)(void);
typedef void (*on_close_t)(void );

extern on_profile_init_t on_profile_init_cb;
extern on_discovery_stops_t on_discovery_stops_cb;
extern on_new_dev_name_t on_new_dev_name_cb;
extern on_new_dev_rssi_t on_new_dev_rssi_cb;
extern on_scan_dev_service_t on_scan_dev_service_cb;
extern on_cl_init_t on_cl_init_cb;
extern on_open_t on_open_cb;
extern on_pin_req_t on_pin_req_cb;
extern on_close_t on_close_cb ;

extern uint8_t serv_channel;

void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);