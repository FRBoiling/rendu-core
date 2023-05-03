/*
* Created by boil on 2023/5/3.
*/

#ifndef RENDU_UNORDERED_MAP_H
#define RENDU_UNORDERED_MAP_H

#include <unordered_map>
#include <iostream>

void unordered_map_run() {
    // 构造对象
    std::unordered_map<std::string, double> um({{"apple", 1.0},
                                                {"lemon", 2.0}});

    // 调用make_pair函数模板插入新元素
    um.insert(std::make_pair("milk", 2.0));
    um.insert(std::make_pair("eggs", 6.5));
    um.insert(std::make_pair("sugar", 0.8));

    // 遍历
    for (auto e: um) {
        std::cout << e.first << ": " << e.second << std::endl;
    }


    // 构造对象
    std::unordered_map<std::string, double> um1;
    // 利用[]运算符重载函数进行插入
    um["apple"] = 1.5;
    um["lemon"] = 2.0;
    um["sugar"] = 0.8;

    // 利用[]运算符重载函数修改value
    um["apple"] = 8.88;

    // 遍历
    for (auto e: um) {
        std::cout << e.first << ": " << e.second << std::endl;
    }


}

void unordered_multimap_run() {

    std::unordered_multimap<int, int> myUnorderedMultiMap = {{2, 10},
                                                             {1, 20},
                                                             {3, 30},
                                                             {3, 300}};

    //正向迭代器测试
    std::cout << "正向迭代器遍历容器：";
    for (auto it = myUnorderedMultiMap.begin(); it != myUnorderedMultiMap.end(); ++it) {
        std::cout << it->first << " -> " << it->second << ", ";
    }
    std::cout << std::endl;
    //正向常迭代器测试
    std::cout << "正向常迭代器遍历容器：";
    for (auto cit = myUnorderedMultiMap.cbegin(); cit != myUnorderedMultiMap.cend(); ++cit) {
        std::cout << cit->first << " -> " << cit->second << ", ";
    }
    std::cout << std::endl;

    std::cout << "myUnorderedMultiMap.empty() = " << myUnorderedMultiMap.empty() << std::endl;
    std::cout << "myUnorderedMultiMap.size() = " << myUnorderedMultiMap.size() << std::endl;
    std::cout << "myUnorderedMultiMap.max_size() = " << myUnorderedMultiMap.max_size() << std::endl;

    std::unordered_multimap< std::string,  std::string> umm = {
            {"orange","FL"},
            {"strawberry","LA"},
            {"strawberry","OK"},
            {"pumpkin","NH"}
    };

    // 统计下面三个单词在容器中出现的次数
    for (auto& x : { "orange","lemon","strawberry" })
    {
        std::cout << x << ": " << umm.count(x) << " 次" <<  std::endl;
    }

    std::cout << std::endl;
}

void unordered_map_example() {
    unordered_map_run();
    unordered_multimap_run();
}

#endif //RENDU_UNORDERED_MAP_H
