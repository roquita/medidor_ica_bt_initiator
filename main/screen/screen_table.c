
#include "main.h"
#include "stdint.h"
#include "scanner.h"

int screen_dev_index_total = 0;
int screen_dev_index_list[MAX_BT_DEVS] = {0};

void screen_table_sync_with_btdevs()
{
    screen_dev_index_total = 0;
    int checked_slots = 0;
    for (int i = 0; i < MAX_BT_DEVS; i++)
    {
        if (!bt_dev_list[i].taked_slot)
            continue;

        screen_dev_index_list[screen_dev_index_total] = i;
        screen_dev_index_total++;
        
        checked_slots++;
        if (checked_slots >= bt_dev_total)
            break;
    }
}

int screen_table_get_index(int pos)
{
    return screen_dev_index_list[pos];
}