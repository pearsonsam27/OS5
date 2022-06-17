/**
 * This file contains implementations for methods in the PageTable class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "page_table/page_table.h"

using namespace std;

size_t PageTable::get_present_page_count() const
{
    // TODO: implement me
    int count = 0;
    for (int i = 0; i < rows.size(); i++)
    {
        if (rows[i].present)
        {
            count++;
        }
    }
    return count;
}

size_t PageTable::get_oldest_page() const
{
    int oldIndex = 0;
    int time = 1000000000;
    for (int i = 0; i < rows.size(); i++)
    {
        if (rows[i].loaded_at < time && rows[i].present)
        {
            time = rows[i].loaded_at;
            oldIndex = i;
        }
    }
    return oldIndex;
}

size_t PageTable::get_least_recently_used_page() const
{
    int lruPage = 1000000000;
    int index = 0;
    for (int i = 0; i < rows.size(); i++)
    {
        if (rows[i].last_accessed_at < lruPage && rows[i].present)
        {
            lruPage = rows[i].last_accessed_at;
            index = i;
        }
    }
    return index;
}
