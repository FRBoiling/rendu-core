//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_TYPES_H
#define RENDU_TYPES_H

#include "common/define.h"
#include <type_traits>

BEGIN_NAMESPACE_COMMON

    // ============================================================================
    // find_type_if 类型查找工具
    // ============================================================================
    
    /**
     * @brief 类型查找的结束标记
     * 
     * 当在类型列表中找不到匹配的类型时，返回此标记作为查找结果。
     * 用于区分"找到类型"和"未找到类型"两种情况。
     */
    struct find_type_end;

    /**
     * @brief 主模板声明 - 在类型列表中查找满足条件的第一个类型
     * 
     * @tparam Check 类型检查谓词，必须提供静态bool值 ::value
     * @tparam Ts 要搜索的类型列表
     * 
     * 这是一个编译时类型查找工具，用于在模板参数包中查找第一个满足特定条件的类型。
     */
    template <template<typename...> typename Check, typename... Ts>
    struct find_type_if;

    /**
     * @brief 基本情况 - 类型列表为空时的特化
     * 
     * 当没有类型可供检查时，返回结束标记表示查找失败。
     */
    template <template<typename...> typename Check>
    struct find_type_if<Check>
    {
        using type = find_type_end;  ///< 返回结束标记，表示未找到匹配类型
    };

    /**
     * @brief 递归情况 - 检查当前类型并继续查找
     * 
     * 递归地在类型列表中查找第一个满足条件的类型：
     * 1. 检查当前类型T1是否满足Check<T1>::value == true
     * 2. 如果满足，返回T1
     * 3. 如果不满足，继续在剩余类型Ts...中递归查找
     * 
     * @note 使用std::conditional_t进行编译时条件判断，避免运行时开销
     */
    template <template<typename...> typename Check, typename T1, typename... Ts>
    struct find_type_if<Check, T1, Ts...> : 
        std::conditional_t<Check<T1>::value, std::type_identity<T1>, find_type_if<Check, Ts...>>
    {
    };

    /**
     * @brief 类型查找工具的便捷别名
     * 
     * 使用示例：
     * @code
     * // 定义检查是否为tuple的谓词
     * template<typename T>
     * struct is_tuple : std::false_type {};
     * 
     * template<typename... Ts>
     * struct is_tuple<std::tuple<Ts...>> : std::true_type {};
     * 
     * // 在类型列表中查找tuple类型
     * using FoundType = find_type_if_t<is_tuple, int, std::string, std::tuple<int, int>, char>;
     * // FoundType 将是 std::tuple<int, int>
     * 
     * // 检查是否找到类型
     * bool found = !std::is_same_v<FoundType, find_type_end>;
     * @endcode
     * 
     * 应用场景：
     * - 配置解析：在参数列表中查找特定类型的配置项
     * - 事件处理：在参数包中查找事件处理器类型
     * - 序列化：在类型列表中查找需要特殊处理的类型
     */
    template <template<typename...> typename Check, typename... Ts>
    using find_type_if_t = typename find_type_if<Check, Ts...>::type;

    // ============================================================================
    // dependant_false 依赖false惯用法
    // ============================================================================
    
    /**
     * @brief 依赖false模板结构体
     * 
     * 这是一个模板元编程技巧，用于在模板特化中提供编译时错误信息。
     * 关键特性：只在模板被实例化时才触发static_assert错误。
     * 
     * 普通static_assert(false, "Error")会在编译时立即报错，而这个技巧允许：
     * - 模板定义存在但不被使用时不会报错
     * - 只有在实际使用不支持的模板特化时才报错
     */
    template <typename T>
    struct dependant_false
    {
        static constexpr bool value = false;  ///< 总是返回false，但依赖模板参数T
    };

    /**
     * @brief dependant_false的便捷变量模板
     * 
     * 使用示例：
     * @code
     * template<typename T>
     * struct UnsupportedTypeHandler
     * {
     *     static_assert(dependant_false_v<T>, "This type is not supported");
     *     // 其他特化版本可以正常编译
     * };
     * 
     * template<>
     * struct UnsupportedTypeHandler<int>
     * {
     *     // int类型的特化版本，正常编译
     * };
     * @endcode
     */
    template <typename T>
    constexpr bool dependant_false_v = dependant_false<T>::value;

END_NAMESPACE_COMMON

#endif //RENDU_TYPES_H