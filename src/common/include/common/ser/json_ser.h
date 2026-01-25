#pragma once
#include "common/define.h"
#include "common/ser/serializer.h"
#include "common/net/codec.h"
#include "common/util/error.h"
#include <simdjson.h>
#include <string>
#include <vector>
#include <map>

BEGIN_NAMESPACE_COMMON
namespace ser {

using net::ByteBuffer;

/**
 * @brief JSON 序列化器
 * 
 * 封装 simdjson 的序列化/反序列化操作
 * 注意：simdjson 主要用于高性能解析，序列化使用字符串拼接
 */
class JsonSerializer : public Serializer {
public:
    JsonSerializer() = default;
    ~JsonSerializer() override = default;

    // 禁止拷贝和移动
    JsonSerializer(const JsonSerializer&) = delete;
    JsonSerializer& operator=(const JsonSerializer&) = delete;

    /**
     * @brief 序列化 ByteBuffer 到 JSON 格式
     * @param data 输入数据
     * @return 序列化后的 ByteBuffer
     */
    Result<ByteBuffer> serialize(const ByteBuffer& data) override;

    /**
     * @brief 反序列化 JSON 格式到 ByteBuffer
     * @param data 输入数据
     * @return 反序列化后的 ByteBuffer
     */
    Result<ByteBuffer> deserialize(const ByteBuffer& data) override;

    /**
     * @brief 序列化整数到 JSON
     * @param value 要序列化的值
     * @return 序列化后的 ByteBuffer
     */
    static Result<ByteBuffer> serialize_json(int value);
    static Result<ByteBuffer> serialize_json(int64_t value);
    static Result<ByteBuffer> serialize_json(double value);

    /**
     * @brief 序列化字符串到 JSON
     * @param value 要序列化的值
     * @return 序列化后的 ByteBuffer
     */
    static Result<ByteBuffer> serialize_json(const std::string& value);

    /**
     * @brief 序列化布尔值到 JSON
     * @param value 要序列化的值
     * @return 序列化后的 ByteBuffer
     */
    static Result<ByteBuffer> serialize_json(bool value);

    /**
     * @brief 序列化空值到 JSON
     * @return 序列化后的 ByteBuffer
     */
    static Result<ByteBuffer> serialize_json_null();

    /**
     * @brief 序列化数组到 JSON
     * @param values 要序列化的数组
     * @return 序列化后的 ByteBuffer
     */
    template<typename T>
    static Result<ByteBuffer> serialize_json_array(const std::vector<T>& values) {
        try {
            std::string result = "[";
            for (size_t i = 0; i < values.size(); ++i) {
                if (i > 0) result += ",";
                if constexpr (std::is_same_v<T, std::string>) {
                    result += "\"" + values[i] + "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    result += values[i] ? "true" : "false";
                } else {
                    result += std::to_string(values[i]);
                }
            }
            result += "]";
            return ByteBuffer(result.begin(), result.end());
        } catch (const std::exception& e) {
            return Error(ErrorCode::SerializationError, e.what());
        }
    }

    /**
     * @brief 反序列化 JSON 到整数
     * @param data JSON 数据
     * @return 反序列化后的值
     */
    static Result<int64_t> deserialize_json_int64(const ByteBuffer& data);
    static Result<double> deserialize_json_double(const ByteBuffer& data);

    /**
     * @brief 反序列化 JSON 到字符串
     * @param data JSON 数据
     * @return 反序列化后的值
     */
    static Result<std::string> deserialize_json_string(const ByteBuffer& data);

    /**
     * @brief 反序列化 JSON 到布尔值
     * @param data JSON 数据
     * @return 反序列化后的值
     */
    static Result<bool> deserialize_json_bool(const ByteBuffer& data);

    /**
     * @brief 反序列化 JSON 到数组
     * @tparam T 数组元素类型
     * @param data JSON 数据
     * @return 反序列化后的数组
     */
    template<typename T>
    static Result<std::vector<T>> deserialize_json_array(const ByteBuffer& data) {
        try {
            std::string str(data.begin(), data.end());
            simdjson::dom::parser parser;
            auto doc = parser.parse(str);
            
            if (!doc.is_array()) {

                return Error(ErrorCode::DeserializationError, "JSON 不是数组类型");
            }

            std::vector<T> result;
            for (auto element : doc.get_array()) {
                if constexpr (std::is_same_v<T, int64_t>) {
                    if (element.is_int64()) {
                        result.push_back(element.get_int64().value());
                    }
                } else if constexpr (std::is_same_v<T, double>) {
                    if (element.is_double()) {
                        result.push_back(element.get_double().value());
                    }
                } else if constexpr (std::is_same_v<T, std::string>) {
                    if (element.is_string()) {
                        auto str_view_result = element.get_string();
                        result.push_back(std::string(str_view_result.value()));
                    }
                } else if constexpr (std::is_same_v<T, bool>) {
                    if (element.is_bool()) {
                        result.push_back(element.get_bool().value());
                    }
                }
            }
            return result;
        } catch (const simdjson::simdjson_error& e) {
            return Error(ErrorCode::DeserializationError, e.what());
        }
    }

    /**
     * @brief 检查是否为有效的 JSON 数据
     * @param data 输入数据
     * @return 是否有效
     */
    static bool is_valid_json(const ByteBuffer& data) {
        try {
            std::string str(data.begin(), data.end());
            if (str.empty()) {
                return false;
            }
            simdjson::dom::parser parser;
            auto doc = parser.parse(str);
            return true;
        } catch (...) {
            return false;
        }
    }

    /**
     * @brief 获取 JSON 类型
     * @param data JSON 数据
     * @return JSON 类型字符串
     */
    static std::string get_json_type(const ByteBuffer& data);

    /**
     * @brief 获取 JSON 对象的字符串值
     * @param data JSON 数据
     * @param key 键名
     * @return 值或错误
     */
    static Result<std::string> get_json_string(const ByteBuffer& data, const std::string& key);

    /**
     * @brief 获取 JSON 对象的整数值
     * @param data JSON 数据
     * @param key 键名
     * @return 值或错误
     */
    static Result<int64_t> get_json_int64(const ByteBuffer& data, const std::string& key);

    /**
     * @brief 获取 JSON 对象的浮点数值
     * @param data JSON 数据
     * @param key 键名
     * @return 值或错误
     */
    static Result<double> get_json_double(const ByteBuffer& data, const std::string& key);

    /**
     * @brief 获取 JSON 对象的布尔值
     * @param data JSON 数据
     * @param key 键名
     * @return 值或错误
     */
    static Result<bool> get_json_bool(const ByteBuffer& data, const std::string& key);

    /**
     * @brief 获取 JSON 对象的值（原始类型，用于类型检查）
     * @param data JSON 数据
     * @param key 键名
     * @return 类型名称或错误
     */
    static Result<std::string> get_json_value_type(const ByteBuffer& data, const std::string& key);
};

} // namespace ser
END_NAMESPACE_COMMON
