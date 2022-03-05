
#include "screen.h"
#include "scanner.h"

extern int screen_dev_index_total;
int cursor_relative_pos = 1; // from 1 to 4
int min_cursor_pos = 0;
int max_cursor_pos = 0;
int min_table_pos = 0;
int max_table_pos = 0;

void screen_window_sync_with_table()
{
    min_table_pos = 0;
    max_table_pos = screen_dev_index_total == 0 ? 0 : screen_dev_index_total - 1;

    if (max_cursor_pos > max_table_pos)
    {
        max_cursor_pos = max_table_pos;
        if (max_table_pos < 3)
            min_cursor_pos = 0;
        else
            min_cursor_pos = max_table_pos - 3;
    }
    else if (max_cursor_pos < max_table_pos)
    {
        if (max_table_pos <= 3)
        {
            max_cursor_pos = max_table_pos;
            min_cursor_pos = 0;
        }
        else
        {
            if (max_cursor_pos - min_cursor_pos + 1 < 4)
            {
                max_cursor_pos = 3;
                min_cursor_pos = 0;
            }
        }
    }
}

void screen_window_arrow_up_pressed()
{
    if (cursor_relative_pos == 1)
    {
        if (min_cursor_pos > min_table_pos)
        {
            min_cursor_pos--;
            max_cursor_pos--;
        }
    }
    else
    {
        cursor_relative_pos--;
    }
}

void screen_window_arrow_down_pressed()
{
    if (cursor_relative_pos == 4)
    {
        if (max_cursor_pos < max_table_pos)
        {
            min_cursor_pos++;
            max_cursor_pos++;
        }
    }
    else
    {
        cursor_relative_pos++;
    }
}

int screen_window_get_cursor_pos()
{
    return min_cursor_pos + cursor_relative_pos - 1;
}
