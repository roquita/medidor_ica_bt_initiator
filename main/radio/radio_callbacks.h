
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

typedef void (*on_profile_init_t)(void);
typedef void (*on_discovery_stops_t)(void);
typedef void (*on_new_dev_name_t)(esp_bd_addr_t bd_addr, void *val);
typedef void (*on_new_dev_rssi_t)(esp_bd_addr_t bd_addr, void *val);

extern on_profile_init_t on_profile_init_cb;
extern on_discovery_stops_t on_discovery_stops_cb;
extern on_new_dev_name_t on_new_dev_name_cb;
extern on_new_dev_rssi_t on_new_dev_rssi_cb;

void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);