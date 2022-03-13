#include "screen_table.c"
#include "screen_window.c"
#include "esp_log.h"
#include "ssd1306.h"
#include "font8x8_basic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

#define TAG "SCREEN_TAG"

SSD1306_t dev;

void screen_init()
{
    spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&dev, 128, 64);

    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xff);
}

void screen_sync_data()
{
    screen_table_sync_with_btdevs();
    screen_window_sync_with_table();

    ESP_LOGI(TAG, "cursor_relative_pos:%i || min_cursor_pos:%i "
                  "|| max_cursor_pos:%i || min_table_pos:%i || max_table_pos:%i\n",
             cursor_relative_pos, min_cursor_pos, max_cursor_pos, min_table_pos, max_table_pos);
}

void screen_sync_with_scroll_up()
{
    screen_window_arrow_up_pressed();
    ESP_LOGI(TAG, "cursor_relative_pos:%i || min_cursor_pos:%i "
                  "|| max_cursor_pos:%i || min_table_pos:%i || max_table_pos:%i\n",
             cursor_relative_pos, min_cursor_pos, max_cursor_pos, min_table_pos, max_table_pos);
}

void screen_sync_with_scroll_down()
{
    screen_window_arrow_down_pressed();
    ESP_LOGI(TAG, "cursor_relative_pos:%i || min_cursor_pos:%i "
                  "|| max_cursor_pos:%i || min_table_pos:%i || max_table_pos:%i\n",
             cursor_relative_pos, min_cursor_pos, max_cursor_pos, min_table_pos, max_table_pos);
}

// const char symb_connected = 128;
const char symb_connected = 'C';
const char symb_arrow = 62;
const char symb_space = 32;

void screen_update()
{
    ssd1306_clear_screen(&dev, false);

    char bufer[17] = {0};
    char rssi_level;

    int window_length = max_cursor_pos - min_cursor_pos + 1;

    if (screen_dev_index_total == 0)
    {
        ssd1306_display_text(&dev, 0, " --No devices-- ", 16, false);
        return;
    }

    for (int i = 0; i < window_length; i++)
    {

        int index = screen_dev_index_list[min_cursor_pos + i];
        bt_device_t bt_dev = bt_dev_list[index];
        uint8_t *mac = bt_dev.mac;
        char *name = bt_dev.name;
        int8_t rssi = bt_dev.rssi;
        bool paired = bt_dev.paired;
        /*
                sprintf(bufer, "%c%c%02x%02x:%02x%02x:%02x%02x",
                        i + 1 == cursor_relative_pos ? symb_arrow : symb_space,
                        paired ? symb_connected : symb_space,
                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                ssd1306_display_text(&dev, i * 2, bufer, strlen(bufer), false);
                rssi_level = rssi > -65 ? 132 : (rssi > -85 ? 131 : (rssi > -100 ? 130 : (rssi > -110 ? 129 : 133)));
                sprintf(bufer, "  %11.11s %c", name, rssi_level);
                // bufer[15] = rssi_level;
                ssd1306_display_text(&dev, i * 2 + 1, bufer, strlen(bufer), false);
                */
        /*
sprintf(bufer, "%c%c%02x%02x:%02x%02x:%02x%02x",
      i + 1 == cursor_relative_pos ? symb_arrow : symb_space,
      paired ? symb_connected : symb_space,
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
ssd1306_display_text(&dev, i * 2, bufer, strlen(bufer), false);
*/
        rssi_level = rssi > -65 ? 132 : (rssi > -85 ? 131 : (rssi > -100 ? 130 : (rssi > -110 ? 129 : 133)));
        sprintf(bufer, "%c %-9.9s %c %c", i + 1 == cursor_relative_pos ? symb_arrow : symb_space,
                name, paired ? symb_connected : symb_space, rssi_level);
        // bufer[15] = rssi_level;
        ssd1306_display_text(&dev, i * 2, bufer, strlen(bufer), false);
    }
}

void screen_print_pairing()
{
    ssd1306_display_text(&dev, 2, "-- Pairing... -- ", 16, true);
}

void screen_print_starting()
{
    ssd1306_display_text(&dev, 2, "-- Starting.. -- ", 16, true);
}

void screen_clean()
{
    ssd1306_clear_screen(&dev, true);
}

int screen_get_selected_index()
{
    int cursor_pos = screen_window_get_cursor_pos();
    int index = screen_table_get_index(cursor_pos);

    return index;
}
