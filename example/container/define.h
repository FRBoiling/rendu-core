/*
* Created by boil on 2023/5/3.
*/

#ifndef RENDU_DEFINE_H
#define RENDU_DEFINE_H


class TestSortClass {
public:
    TestSortClass() : m_key(0), m_value(0) {}

public:
    int m_key;
    int m_value;
public:
    bool operator==(const TestSortClass &ps) const {
        return this->m_value == ps.m_value;
    }

    bool operator<(const TestSortClass &ps) const {
        return this->m_value < ps.m_value;
    }
};


template<typename T>
void removeRepeat(std::vector<T> &vec) {
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
};



#endif //RENDU_DEFINE_H
