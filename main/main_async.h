#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

void on_profile_init(void);
void on_discovery_stops();
void on_new_dev_name(uint8_t *bd_addr, void *val);
void on_new_dev_rssi(uint8_t *bd_addr, void *val);
