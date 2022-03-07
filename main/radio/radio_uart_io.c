#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "stdbool.h"
#include "string.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "radio.h"

#define RADIO_UART_IO_TAG "RADIO_UART_IO_TAG"

#define CONF_PIN (GPIO_NUM_21)
#define TXD_PIN (GPIO_NUM_1) // (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_3) // (GPIO_NUM_5)
#define RX_BUF_SIZE 2048

RingbufHandle_t buf_handle;
bool write_flag_enabled = true;
bool is_cong_needed = false;
bool read_flag_enabled = false;

extern uint32_t handle;

void uart_tx_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);

    buf_handle = xRingbufferCreate(1024 * 5, RINGBUF_TYPE_BYTEBUF);

    while (1)
    {
        if (read_flag_enabled)
        {
            vTaskDelay(pdMS_TO_TICKS(300)); // vTaskDelay(pdMS_TO_TICKS(200));

            size_t size = 0;
            void *data = xRingbufferReceive(buf_handle, &size, pdMS_TO_TICKS(1000));

            if (data)
            {
                uart_write_bytes(UART_NUM_0, data, size);
                uart_wait_tx_done(UART_NUM_0, portMAX_DELAY);

                vRingbufferReturnItem(buf_handle, data);
            }
            read_flag_enabled = false;
        }
        vTaskDelay(1);
    }
}

void uart_rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);
    int rxBytes = 0;
    while (1)
    {
        rxBytes += uart_read_bytes(UART_NUM_0, data + rxBytes, RX_BUF_SIZE - rxBytes, pdMS_TO_TICKS(20)); // pdMS_TO_TICKS(1)

        if (handle == 0)
        {
            memset(data, 0, rxBytes);
            rxBytes = 0;
            continue;
        }
        if (rxBytes > 0 && write_flag_enabled)
        {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes'", rxBytes);
            esp_spp_write(handle, rxBytes, data);
            rxBytes = 0;
            write_flag_enabled = false;
        }
    }
    free(data);
}

void radio_uart_ioc_init()
{
    const gpio_config_t gpio_conf = {
        .pin_bit_mask = 1UL << CONF_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_conf);

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = gpio_get_level(CONF_PIN) == 0 ? UART_DATA_7_BITS : UART_DATA_8_BITS,
        .parity = gpio_get_level(CONF_PIN) == 0 ? UART_PARITY_EVEN : UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    char parity = uart_config.parity == UART_PARITY_DISABLE ? 'N' : (uart_config.parity == UART_PARITY_EVEN ? 'E' : 'O');
    uint8_t data_bits = uart_config.data_bits + 5;
    ESP_LOGI(RADIO_UART_IO_TAG, "uart data config: %d%c1\n\r", data_bits, parity);

    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    char texto[100] = {0};
    sprintf(texto, "uart data config: %d%c1\n\r", data_bits, parity);
    uart_tx_chars(UART_NUM_0, texto, strlen(texto));

    xTaskCreatePinnedToCore(uart_rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL, 0);
    xTaskCreatePinnedToCore(uart_tx_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL, 0);
}