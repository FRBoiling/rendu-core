/*
* Created by boil on 2023/5/3.
*/

#ifndef RENDU_MAP_EXAMPLE_H
#define RENDU_MAP_EXAMPLE_H

#include "define.h"
#include <map>

void map_example()
{
    std::map<int, TestSortClass> temp_back_groups;
    int key = 10;
    int value = 1;
    while (key > 0) {
        TestSortClass item;
        item.m_value = value;
        item.m_key = key;
        temp_back_groups[item.m_key] = item;
        key--;
        value++;
    }
    temp_back_groups.erase(8);
    temp_back_groups.erase(8);

}

#endif //RENDU_MAP_EXAMPLE_H
