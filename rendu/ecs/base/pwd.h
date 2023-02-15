/*
* Created by boil on 2023/2/16.
*/

#ifndef RENDU_ECS_BASE_PWD_H_
#define RENDU_ECS_BASE_PWD_H_

/*! @brief Default entity identifier. */
enum class entity : id_type {};

template<typename, typename = entity>
struct component_traits;


#endif //RENDU_ECS_BASE_PWD_H_
