#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "radio.h"
#include "main_async.h"
#include "scanner.h"

SemaphoreHandle_t printSemph = NULL;

// static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
// static const esp_spp_role_t role_master = ESP_SPP_ROLE_MASTER;

// esp_bd_addr_t peer_bd_addr = {0};
// static uint8_t peer_bdname_len;
// static char peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
// static const char remote_device_name[] = "ESP_SPP_ACCEPTOR";
// static const uint8_t inq_num_rsps = 0;

void app_main(void)
{
    radio_init(on_profile_init, on_discovery_stops, on_new_dev_name, on_new_dev_rssi);

    printSemph = xSemaphoreCreateBinary();

    while (1)
    {
        xSemaphoreTake(printSemph, portMAX_DELAY);
        scanner_print_dev_list();
    }
}
