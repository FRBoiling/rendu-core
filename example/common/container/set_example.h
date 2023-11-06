/*
* Created by boil on 2023/5/3.
*/

#ifndef RENDU_SET_H
#define RENDU_SET_H
#include "define.h"
#include <set>
#include <map>

void MergeContainer(std::map<int, std::set<int>> &container1, std::map<int, std::set<int>> &container3) {
    for (auto it = container1.begin(); it != container1.end(); ++it) {
        container3[it->first].insert(it->second.begin(), it->second.end());
    }
}

void set_example(){

    std::map<int, std::set<int> > container1;
    std::map<int, std::set<int> > container2;
    std::map<int, std::set<int> > container3;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            container1[i].insert(j);
            container2[i].insert(j + 3);
        }
    }
    MergeContainer(container1, container3);
    MergeContainer(container2, container3);

    std::set<TestSortClass> set1;
    for (int i = 1; i < 10; i++) {
        TestSortClass item;
        item.m_value = i;
        item.m_key = i;
        set1.insert(item);
    }
    for (int i = 2; i < 5; i++) {
        TestSortClass item;
        item.m_value = i;
        item.m_key = i;
        set1.insert(item);
    }

}


#endif //RENDU_SET_H
