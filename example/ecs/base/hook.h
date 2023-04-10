/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_HOOK_H
#define RENDU_HOOK_H

#include <functional>

template<typename Type>
class hook;

template<typename Ret, typename ...Args>
class hook<Ret(Args...)> {
private:
    using callback_type = std::function<Ret(Args...)>;

    std::vector<callback_type> m_callbacks;

public:
    void connect(callback_type callback, bool front = false) {
        if (front) {
            m_callbacks.insert(m_callbacks.begin(), callback);
        } else {
            m_callbacks.push_back(callback);
        }
    }

    void publish(Args... args) {
        for (auto callback: m_callbacks) {
            callback(args...);
        }
    }
};

#endif //RENDU_HOOK_H
