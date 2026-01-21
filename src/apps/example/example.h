//
// Created by boil on 2026/1/13.
//

#ifndef RENDU_ECS_EXAMPLE_H
#define RENDU_ECS_EXAMPLE_H

#include "../../common/include/common/ecs/entity.h"
#include "../../common/include/common/ecs/registry_base.h"
#include <string>

// ============================================================================
// 示例: 定义组件 (在用户命名空间中)
// ============================================================================

/**
 * @brief 位置组件
 *
 * 所有组件都继承自 Component 基类(空类,仅用于类型标记)
 */
struct Position : public Rendu::Component
{
    float x;
    float y;

    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

/**
 * @brief 速度组件
 */
struct Velocity : public Rendu::Component
{
    float vx;
    float vy;

    Velocity(float vx = 0.0f, float vy = 0.0f) : vx(vx), vy(vy) {}
};

/**
 * @brief 名称组件
 */
struct Name : public Rendu::Component
{
    std::string value;

    explicit Name(const std::string& name) : value(name) {}
};

#endif //RENDU_ECS_EXAMPLE_H
