/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_DEFINE_TYPE_H
#define RENDU_DEFINE_TYPE_H


#define DEFINE_TYPE(name, type) \
    typedef struct { \
        type value; \
    } name##_t;

DEFINE_TYPE(Int, int);
DEFINE_TYPE(Double, double);

#define FUNC(name, type) name##_##type
void FUNC(add, int)() {}//add_int()
void FUNC(add, float)() {}//add_float()

#endif //RENDU_DEFINE_TYPE_H
