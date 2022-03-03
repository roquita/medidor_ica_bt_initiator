#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include "radio_callbacks.h"
#include "main.h"
#include "scanner.h"

#define MAIN_ASYNC_TAG "MAIN_ASYNC_TAG"

extern SemaphoreHandle_t printSemph;

void on_profile_init(void)
{
    scanner_start();
}

void on_discovery_stops()
{
    scanner_remove_inactive_devs();
    scanner_start();
    xSemaphoreGive(printSemph);
}

void on_new_dev_name(uint8_t *bd_addr, void *val)
{
    scanner_add_new_device_name(bd_addr, val);
    xSemaphoreGive(printSemph);
}

void on_new_dev_rssi(uint8_t *bd_addr, void *val)
{
    scanner_add_new_device_rssi(bd_addr, val);
    xSemaphoreGive(printSemph);
}