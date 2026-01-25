//**********************************
//  Created by boil on 2026/1/25.
//**********************************

#include "common/ser/json_ser.h"
#include <sstream>

BEGIN_NAMESPACE_COMMON
namespace ser {

Result<ByteBuffer> JsonSerializer::serialize(const ByteBuffer& data) {
    // 尝试验证 JSON 是否有效
    if (!is_valid_json(data)) {
        return Error(ErrorCode::SerializationError, "无效的 JSON 数据");
    }
    // 返回原始数据（已经是 JSON 格式）
    return data;
}

Result<ByteBuffer> JsonSerializer::deserialize(const ByteBuffer& data) {
    // 验证并返回原始 JSON 数据
    if (!is_valid_json(data)) {
        return Error(ErrorCode::DeserializationError, "无效的 JSON 数据");
    }
    return data;
}

Result<ByteBuffer> JsonSerializer::serialize_json(int value) {
    return ByteBuffer(std::to_string(value).begin(), 
                     std::to_string(value).end());
}

Result<ByteBuffer> JsonSerializer::serialize_json(int64_t value) {
    return ByteBuffer(std::to_string(value).begin(), 
                     std::to_string(value).end());
}

Result<ByteBuffer> JsonSerializer::serialize_json(double value) {
    return ByteBuffer(std::to_string(value).begin(), 
                     std::to_string(value).end());
}

Result<ByteBuffer> JsonSerializer::serialize_json(const std::string& value) {
    std::string result = "\"";
    for (char c : value) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    result += "\"";
    return ByteBuffer(result.begin(), result.end());
}

Result<ByteBuffer> JsonSerializer::serialize_json(bool value) {
    std::string str = value ? "true" : "false";
    return ByteBuffer(str.begin(), str.end());
}

Result<ByteBuffer> JsonSerializer::serialize_json_null() {
    static const std::string null_str = "null";
    return ByteBuffer(null_str.begin(), null_str.end());
}

Result<int64_t> JsonSerializer::deserialize_json_int64(const ByteBuffer& data) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);
        
        if (doc.is_int64()) {
            return doc.get_int64().value();
        }
        return Error(ErrorCode::DeserializationError, "不是整数类型");
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, e.what());
    }
}

Result<double> JsonSerializer::deserialize_json_double(const ByteBuffer& data) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);
        
        if (doc.is_double()) {
            return doc.get_double().value();
        }
        return Error(ErrorCode::DeserializationError, "不是浮点数类型");
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, e.what());
    }
}

Result<std::string> JsonSerializer::deserialize_json_string(const ByteBuffer& data) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);
        
        if (doc.is_string()) {
            auto str_view = doc.get_string();
            return std::string(str_view.value());
        }
        return Error(ErrorCode::DeserializationError, "不是字符串类型");
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, e.what());
    }
}

Result<bool> JsonSerializer::deserialize_json_bool(const ByteBuffer& data) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);
        
        if (doc.is_bool()) {
            return doc.get_bool().value();
        }
        return Error(ErrorCode::DeserializationError, "不是布尔类型");
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, e.what());
    }
}

std::string JsonSerializer::get_json_type(const ByteBuffer& data) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);

        auto type_result = doc.type();
        auto type = type_result.value();
        switch (type) {
            case simdjson::dom::element_type::ARRAY: return "array";
            case simdjson::dom::element_type::OBJECT: return "object";
            case simdjson::dom::element_type::INT64: return "int64";
            case simdjson::dom::element_type::UINT64: return "uint64";
            case simdjson::dom::element_type::DOUBLE: return "double";
            case simdjson::dom::element_type::STRING: return "string";
            case simdjson::dom::element_type::BOOL: return "bool";
            case simdjson::dom::element_type::NULL_VALUE: return "null";
            default: return "unknown";
        }
    } catch (...) {
        return "error";
    }
}

Result<std::string> JsonSerializer::get_json_string(const ByteBuffer& data, const std::string& key) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);

        if (!doc.is_object()) {
            return Error(ErrorCode::DeserializationError, "JSON 不是对象类型");
        }

        auto element = doc.at_key(key).value();
        if (!element.is_string()) {
            return Error(ErrorCode::DeserializationError, "键的值不是字符串类型");
        }
        auto str_view = element.get_string();
        return std::string(str_view.value());
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, std::string("键不存在或访问失败: ") + e.what());
    }
}

Result<int64_t> JsonSerializer::get_json_int64(const ByteBuffer& data, const std::string& key) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);

        if (!doc.is_object()) {
            return Error(ErrorCode::DeserializationError, "JSON 不是对象类型");
        }

        auto element = doc.at_key(key).value();
        if (!element.is_int64()) {
            return Error(ErrorCode::DeserializationError, "键的值不是整数类型");
        }
        return element.get_int64().value();
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, std::string("键不存在或访问失败: ") + e.what());
    }
}

Result<double> JsonSerializer::get_json_double(const ByteBuffer& data, const std::string& key) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);

        if (!doc.is_object()) {
            return Error(ErrorCode::DeserializationError, "JSON 不是对象类型");
        }

        auto element = doc.at_key(key).value();
        if (!element.is_double()) {
            return Error(ErrorCode::DeserializationError, "键的值不是浮点数类型");
        }
        return element.get_double().value();
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, std::string("键不存在或访问失败: ") + e.what());
    }
}

Result<bool> JsonSerializer::get_json_bool(const ByteBuffer& data, const std::string& key) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);

        if (!doc.is_object()) {
            return Error(ErrorCode::DeserializationError, "JSON 不是对象类型");
        }

        auto element = doc.at_key(key).value();
        if (!element.is_bool()) {
            return Error(ErrorCode::DeserializationError, "键的值不是布尔类型");
        }
        return element.get_bool().value();
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, std::string("键不存在或访问失败: ") + e.what());
    }
}

Result<std::string> JsonSerializer::get_json_value_type(const ByteBuffer& data, const std::string& key) {
    try {
        std::string str(data.begin(), data.end());
        simdjson::dom::parser parser;
        auto doc = parser.parse(str);

        if (!doc.is_object()) {
            return Error(ErrorCode::DeserializationError, "JSON 不是对象类型");
        }

        auto element = doc.at_key(key).value();
        switch (element.type()) {
            case simdjson::dom::element_type::ARRAY: return "array";
            case simdjson::dom::element_type::OBJECT: return "object";
            case simdjson::dom::element_type::INT64: return "int64";
            case simdjson::dom::element_type::UINT64: return "uint64";
            case simdjson::dom::element_type::DOUBLE: return "double";
            case simdjson::dom::element_type::STRING: return "string";
            case simdjson::dom::element_type::BOOL: return "bool";
            case simdjson::dom::element_type::NULL_VALUE: return "null";
            default: return "unknown";
        }
    } catch (const simdjson::simdjson_error& e) {
        return Error(ErrorCode::DeserializationError, std::string("键不存在或访问失败: ") + e.what());
    }
}

} // namespace ser
END_NAMESPACE_COMMON
