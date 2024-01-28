/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_INSTANCE_IMP_H
#define RENDU_INSTANCE_IMP_H

#include "define.h"

RD_NAMESPACE_BEGIN

#define INSTANCE_IMP(class_name, ...) \
class_name &class_name::Instance() { \
    static std::shared_ptr<class_name> s_instance(new class_name(__VA_ARGS__)); \
    static class_name &s_instance_ref = *s_instance; \
    return s_instance_ref; \
}

RD_NAMESPACE_END

#endif //RENDU_INSTANCE_IMP_H
