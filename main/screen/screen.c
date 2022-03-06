#include "screen_table.c"
#include "screen_window.c"
#include "esp_log.h"

#define TAG "SCREEN_TAG"

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

void screen_update()
{
}
int screen_get_selected_index()
{
    int cursor_pos = screen_window_get_cursor_pos();
    int index = screen_table_get_index(cursor_pos);

    return index;
}
