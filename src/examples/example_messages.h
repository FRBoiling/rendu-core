#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>

#include "core/actor/message.h"

BEGIN_NAMESPACE_CORE

/// 文本消息示例
class TextMessage : public Message {
public:
    TextMessage() = default;
    explicit TextMessage(const std::string& text) : text_(text) {}

    const char* get_type() const override { return "TextMessage"; }

    std::string serialize() const override {
        return "TextMessage|" + std::to_string(request_id_) + "|" + text_;
    }

    void deserialize_data(const std::string& data) override {
        text_ = data;
    }

    const std::string& text() const { return text_; }
    void set_text(const std::string& text) { text_ = text; }

private:
    std::string text_;
};

/// 计数消息示例
class CounterMessage : public Message {
public:
    CounterMessage() = default;
    explicit CounterMessage(int32_t count) : count_(count) {}

    const char* get_type() const override { return "CounterMessage"; }

    std::string serialize() const override {
        return "CounterMessage|" + std::to_string(request_id_) + "|" + std::to_string(count_);
    }

    void deserialize_data(const std::string& data) override {
        try {
            count_ = std::stoi(data);
        } catch (...) {
            count_ = 0;
        }
    }

    int32_t count() const { return count_; }
    void set_count(int32_t count) { count_ = count; }

private:
    int32_t count_{0};
};

/// 数据消息示例（可以承载任意数据）
class DataMessage : public Message {
public:
    DataMessage() = default;
    explicit DataMessage(const std::vector<uint8_t>& data) : data_(data) {}

    const char* get_type() const override { return "DataMessage"; }

    std::string serialize() const override {
        std::string result = "DataMessage|" + std::to_string(request_id_) + "|";
        result.append(reinterpret_cast<const char*>(data_.data()), data_.size());
        return result;
    }

    void deserialize_data(const std::string& data) override {
        data_.assign(data.begin(), data.end());
    }

    const std::vector<uint8_t>& data() const { return data_; }
    void set_data(const std::vector<uint8_t>& data) { data_ = data; }

private:
    std::vector<uint8_t> data_;
};

/// 停止消息（用于优雅关闭 Actor）
class StopMessage : public Message {
public:
    StopMessage() = default;

    const char* get_type() const override { return "StopMessage"; }

    std::string serialize() const override {
        return "StopMessage|" + std::to_string(request_id_) + "|";
    }

    void deserialize_data(const std::string& data) override {
        // 无数据
    }
};

END_NAMESPACE_CORE
