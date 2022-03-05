#include "radio_callbacks.h"

void radio_init(on_profile_init_t,
                on_discovery_stops_t,
                on_new_dev_name_t,
                on_new_dev_rssi_t,
                on_scan_dev_service_t,
                on_cl_init_t,
                on_open_t,
                on_pin_req_t);

void radio_scan_dev_service(uint8_t *mac);
void radio_start_connection();
void radio_send_pin(uint8_t *pin_code, uint8_t pin_size);
void radio_start_discovery();
void radio_stop_discovery();
