#pragma once
#include "radio_callbacks.h"

void radio_init(on_profile_init_t,
                on_discovery_stops_t,
                on_new_dev_name_t,
                on_new_dev_rssi_t,
                on_scan_dev_service_t,
                on_cl_init_t,
                on_open_t,
                on_pin_req_t,
                on_close_t);

void radio_scan_dev_service();
void radio_start_connection();
void radio_start_disconnection();
void radio_send_pin(uint8_t *pin_code, uint8_t pin_size);
void radio_start_discovery();
void radio_stop_discovery();
void radio_set_as_paired();
void radio_set_as_unpaired();
bool radio_is_paired();
void radio_set_dev_mac(uint8_t *new_dev_mac);
uint8_t *radio_get_dev_mac();
void radio_set_handle(uint32_t new_handle);
uint32_t radio_get_handle();
