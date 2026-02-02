#include "common/config/loader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

#include "common/config/json_loader.h"

BEGIN_NAMESPACE_COMMON
    namespace config {

// 简化的 JSON 解析器
class SimpleJsonParser {
public:
    explicit SimpleJsonParser(const std::string& content) : content_(content), pos_(0), error_({}) {}

    bool parse(Config& config) {
        skip_whitespace();
        if (pos_ >= content_.size()) {
            error_ = Error(ErrorCode::InvalidArgument, "Empty JSON");
            return false;
        }

        if (content_[pos_] != '{') {
            error_ = Error(ErrorCode::InvalidArgument, "JSON must start with '{'");
            return false;
        }

        auto obj_result = parse_object();
        if (std::holds_alternative<Error>(obj_result)) {
            error_ = std::get<Error>(obj_result);
            return false;
        }

        config.data() = std::get<std::unordered_map<std::string, ConfigValue>>(obj_result);
        return true;
    }

    const Error& error() const { return error_; }

private:
    Result<std::unordered_map<std::string, ConfigValue>> parse_object() {
        std::unordered_map<std::string, ConfigValue> obj;

        if (content_[pos_] != '{') {
            return Error(ErrorCode::InvalidArgument, "Expected '{'");
        }
        pos_++;
        skip_whitespace();

        while (pos_ < content_.size() && content_[pos_] != '}') {
            // 解析键
            auto key_result = parse_string();
            if (std::holds_alternative<Error>(key_result)) {
                return std::get<Error>(key_result);
            }
            std::string key = std::get<std::string>(key_result);

            skip_whitespace();

            // 解析冒号
            if (pos_ >= content_.size() || content_[pos_] != ':') {
                return Error(ErrorCode::InvalidArgument, "Expected ':'");
            }
            pos_++;
            skip_whitespace();

            // 解析值
            auto value_result = parse_value();
            if (std::holds_alternative<Error>(value_result)) {
                return std::get<Error>(value_result);
            }
            obj[key] = std::get<ConfigValue>(value_result);

            skip_whitespace();

            // 解析逗号或结束
            if (pos_ < content_.size() && content_[pos_] == ',') {
                pos_++;
                skip_whitespace();
            }
        }

        if (pos_ >= content_.size() || content_[pos_] != '}') {
            return Error(ErrorCode::InvalidArgument, "Expected '}'");
        }
        pos_++;

        return obj;
    }

    Result<ConfigValue> parse_value() {
        skip_whitespace();

        if (pos_ >= content_.size()) {
            return Error(ErrorCode::InvalidArgument, "Unexpected end of JSON");
        }

        char c = content_[pos_];

        if (c == '"') {
            auto str_result = parse_string();
            if (std::holds_alternative<Error>(str_result)) {
                return std::get<Error>(str_result);
            }
            return std::get<std::string>(str_result);
        } else if (c == '{') {
            auto obj_result = parse_nested_object();
            if (std::holds_alternative<Error>(obj_result)) {
                return std::get<Error>(obj_result);
            }
            return std::get<ConfigValue>(obj_result);
        } else if (c == '[') {
            auto arr_result = parse_array();
            if (std::holds_alternative<Error>(arr_result)) {
                return std::get<Error>(arr_result);
            }
            return std::get<ConfigValue>(arr_result);
        } else if (c == 't' || c == 'f') {
            return parse_boolean();
        } else if (c == 'n') {
            return parse_null();
        } else if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
            return parse_number();
        }

        return Error(ErrorCode::InvalidArgument, std::string("Unexpected character: ") + c);
    }

    Result<ConfigValue> parse_nested_object() {
        auto obj_result = parse_object();
        if (std::holds_alternative<Error>(obj_result)) {
            return std::get<Error>(obj_result);
        }

        // 创建嵌套 Config 对象
        const auto& raw_obj = std::get<std::unordered_map<std::string, ConfigValue>>(obj_result);
        auto nested_config = std::make_shared<Config>();

        for (const auto& [k, v] : raw_obj) {
            nested_config->data()[k] = v;
        }

        return ConfigValue(nested_config);
    }

    Result<ConfigValue> parse_array() {
        std::vector<int64_t> int_arr;
        std::vector<double> double_arr;
        std::vector<std::string> string_arr;
        std::vector<bool> bool_arr;

        if (content_[pos_] != '[') {
            return Error(ErrorCode::InvalidArgument, "Expected '['");
        }
        pos_++;
        skip_whitespace();

        bool has_double = false;
        bool has_string = false;
        bool has_bool = false;

        while (pos_ < content_.size() && content_[pos_] != ']') {
            auto value_result = parse_value();
            if (std::holds_alternative<Error>(value_result)) {
                return value_result;
            }
            auto value = std::get<ConfigValue>(value_result);

            if (std::holds_alternative<std::string>(value)) {
                string_arr.push_back(std::get<std::string>(value));
                has_string = true;
            } else if (std::holds_alternative<double>(value)) {
                double_arr.push_back(std::get<double>(value));
                has_double = true;
            } else if (std::holds_alternative<int64_t>(value)) {
                int_arr.push_back(std::get<int64_t>(value));
            } else if (std::holds_alternative<bool>(value)) {
                bool_arr.push_back(std::get<bool>(value));
                has_bool = true;
            }

            skip_whitespace();

            if (pos_ < content_.size() && content_[pos_] == ',') {
                pos_++;
                skip_whitespace();
            }
        }

        if (pos_ >= content_.size() || content_[pos_] != ']') {
            return Error(ErrorCode::InvalidArgument, "Expected ']'");
        }
        pos_++;

        // 返回第一个非空类型的数组
        if (has_string) return string_arr;
        if (has_double) return double_arr;
        if (has_bool) return bool_arr;
        return int_arr;
    }

    Result<std::string> parse_string() {
        if (pos_ >= content_.size() || content_[pos_] != '"') {
            return Error(ErrorCode::InvalidArgument, "Expected '\"'");
        }
        pos_++;

        std::string result;
        while (pos_ < content_.size() && content_[pos_] != '"') {
            if (content_[pos_] == '\\') {
                pos_++;
                if (pos_ < content_.size()) {
                    // 处理转义字符
                    char escaped = content_[pos_];
                    switch (escaped) {
                        case 'n': result += '\n'; break;
                        case 't': result += '\t'; break;
                        case 'r': result += '\r'; break;
                        case '\\': result += '\\'; break;
                        case '"': result += '"'; break;
                        default: result += escaped; break;
                    }
                }
            } else {
                result += content_[pos_];
            }
            pos_++;
        }

        if (pos_ >= content_.size() || content_[pos_] != '"') {
            return Error(ErrorCode::InvalidArgument, "Unterminated string");
        }
        pos_++;

        return result;
    }

    Result<ConfigValue> parse_number() {
        size_t start = pos_;

        if (content_[pos_] == '-') {
            pos_++;
        }

        while (pos_ < content_.size() && std::isdigit(static_cast<unsigned char>(content_[pos_]))) {
            pos_++;
        }

        bool is_float = false;
        if (pos_ < content_.size() && content_[pos_] == '.') {
            is_float = true;
            pos_++;
            while (pos_ < content_.size() && std::isdigit(static_cast<unsigned char>(content_[pos_]))) {
                pos_++;
            }
        }

        std::string num_str = content_.substr(start, pos_ - start);

        try {
            if (is_float) {
                return std::stod(num_str);
            } else {
                return static_cast<int64_t>(std::stoll(num_str));
            }
        } catch (const std::exception&) {
            return Error(ErrorCode::InvalidArgument, "Invalid number: " + num_str);
        }
    }

    Result<ConfigValue> parse_boolean() {
        if (pos_ + 4 <= content_.size() && content_.substr(pos_, 4) == "true") {
            pos_ += 4;
            return true;
        }
        if (pos_ + 5 <= content_.size() && content_.substr(pos_, 5) == "false") {
            pos_ += 5;
            return false;
        }
        return Error(ErrorCode::InvalidArgument, "Invalid boolean value");
    }

    Result<ConfigValue> parse_null() {
        if (pos_ + 4 <= content_.size() && content_.substr(pos_, 4) == "null") {
            pos_ += 4;
            return std::string(""); // null 映射为空字符串
        }
        return Error(ErrorCode::InvalidArgument, "Invalid null value");
    }

    void skip_whitespace() {
        while (pos_ < content_.size() && std::isspace(static_cast<unsigned char>(content_[pos_]))) {
            pos_++;
        }
    }

    std::string content_;
    size_t pos_;
    Error error_;
};

Result<Config> JsonLoader::load(const std::string& filepath) {
    // 读取文件内容
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return Error(ErrorCode::NotFound,
            "Failed to open config file: " + filepath);
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    if (content.empty()) {
        return Error(ErrorCode::InvalidArgument,
            "Config file is empty: " + filepath);
    }

    // 解析 JSON
    SimpleJsonParser parser(content);
    Config config;
    if (!parser.parse(config)) {
        return parser.error();
    }
    return config;
}

} // namespace config
END_NAMESPACE_COMMON
