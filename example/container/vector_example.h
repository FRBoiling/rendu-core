/*
* Created by boil on 2023/5/3.
*/

#ifndef RENDU_VECTOR_EXAMPLE_H
#define RENDU_VECTOR_EXAMPLE_H

#include <vector>
#include "define.h"

void vector_example()
{
    std::vector<TestSortClass> temp_vec;
    int key = 10;
    int value = 1;
    while (key > 0) {
        TestSortClass item;
        item.m_value = value;
        item.m_key = key;
        temp_vec.push_back(item) ;
        key--;
        value++;
    }
    std::sort(temp_vec.begin(), temp_vec.end(),
              [](const TestSortClass &l, const TestSortClass &r) {
                  return l.m_value < r.m_value;
              });

    std::vector<int> vec(0);
    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }
    for (int i = 2; i < 5; i++) {
        vec.push_back(i);
    }

    removeRepeat(vec);


    std::vector<TestSortClass> vec1;
    for (int i = 1; i < 10; i++) {
        TestSortClass item;
        item.m_value = i;
        item.m_key = i;
        vec1.push_back(item);
    }
    for (int i = 2; i < 5; i++) {
        TestSortClass item;
        item.m_value = i;
        item.m_key = i;
        vec1.push_back(item);
    }
    std::sort(vec1.begin(), vec1.end(),
              [](const TestSortClass &l, const TestSortClass &r) {
                  return l.m_value < r.m_value;
              });
    vec1.erase(std::unique(vec1.begin(), vec1.end()), vec1.end());
};

#endif //RENDU_VECTOR_EXAMPLE_H
