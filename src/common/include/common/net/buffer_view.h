#pragma once

#include "common/define.h"
#include <cstddef>
#include <vector>
#include <string_view>

BEGIN_NAMESPACE_COMMON

/**
 * @brief 字节类型定义
 * 注意: 不使用 std::byte,因为需要与现有 ByteBuffer (std::vector<byte>) 兼容
 */
using byte = unsigned char;

/**
 * @brief 字节缓冲区类型
 */
using ByteBuffer = std::vector<byte>;

/**
 * @brief 缓冲区视图 - 零拷贝引用
 *
 * 提供对已有缓冲区的非拥有引用,避免拷贝
 * 注意: 不管理内存生命周期,调用者需确保原始缓冲区有效
 */
class BufferView {
public:
    /**
     * @brief 默认构造函数
     */
    BufferView() noexcept
        : data_(nullptr), size_(0) {}

    /**
     * @brief 从指针和大小构造
     * @param data 数据指针
     * @param size 数据大小
     */
    BufferView(const void* data, size_t size) noexcept
        : data_(static_cast<const byte*>(data)), size_(size) {}

    /**
     * @brief 从 ByteBuffer 构造
     * @param buffer 字节缓冲区
     */
    explicit BufferView(const ByteBuffer& buffer) noexcept
        : data_(buffer.data()), size_(buffer.size()) {}

    /**
     * @brief 从指针范围构造
     * @param begin 起始指针
     * @param end 结束指针
     */
    BufferView(const byte* begin, const byte* end) noexcept
        : data_(begin), size_(static_cast<size_t>(end - begin)) {}

    /**
     * @brief 从 std::string_view 构造
     * @param sv 字符串视图
     */
    explicit BufferView(std::string_view sv) noexcept
        : data_(reinterpret_cast<const byte*>(sv.data())), size_(sv.size()) {}

    /**
     * @brief 获取数据指针
     * @return const byte*
     */
    const byte* data() const noexcept { return data_; }

    /**
     * @brief 获取数据大小
     * @return size_t
     */
    size_t size() const noexcept { return size_; }

    /**
     * @brief 检查是否为空
     * @return bool
     */
    bool empty() const noexcept { return size_ == 0; }

    /**
     * @brief 转换为 ByteBuffer
     * @warning 这会产生拷贝,不是零拷贝
     * @return ByteBuffer
     */
    ByteBuffer to_buffer() const {
        return ByteBuffer(data_, data_ + size_);
    }

    /**
     * @brief 转换为字符串视图
     * @return std::string_view
     */
    std::string_view to_string_view() const noexcept {
        return std::string_view(reinterpret_cast<const char*>(data_), size_);
    }

    /**
     * @brief 创建子视图
     * @param offset 偏移量
     * @param length 长度
     * @return BufferView
     */
    BufferView subview(size_t offset, size_t length) const noexcept {
        if (offset >= size_) {
            return BufferView();
        }
        return BufferView(data_ + offset, std::min(length, size_ - offset));
    }

    /**
     * @brief 访问元素
     * @param index 索引
     * @return const byte&
     */
    const byte& operator[](size_t index) const noexcept {
        return data_[index];
    }

    /**
     * @brief 前向迭代器
     * @return const byte*
     */
    const byte* begin() const noexcept { return data_; }

    /**
     * @brief 后向迭代器
     * @return const byte*
     */
    const byte* end() const noexcept { return data_ + size_; }

private:
    const byte* data_;
    size_t size_;
};

/**
 * @brief 可变缓冲区视图
 *
 * 类似 BufferView,但允许修改数据
 */
class MutableBufferView {
public:
    /**
     * @brief 默认构造函数
     */
    MutableBufferView() noexcept
        : data_(nullptr), size_(0) {}

    /**
     * @brief 从指针和大小构造
     * @param data 数据指针
     * @param size 数据大小
     */
    MutableBufferView(void* data, size_t size) noexcept
        : data_(static_cast<byte*>(data)), size_(size) {}

    /**
     * @brief 从 ByteBuffer 构造
     * @param buffer 字节缓冲区
     */
    explicit MutableBufferView(ByteBuffer& buffer) noexcept
        : data_(buffer.data()), size_(buffer.size()) {}

    /**
     * @brief 获取数据指针
     * @return byte*
     */
    byte* data() noexcept { return data_; }

    /**
     * @brief 获取数据指针 (const)
     * @return const byte*
     */
    const byte* data() const noexcept { return data_; }

    /**
     * @brief 获取数据大小
     * @return size_t
     */
    size_t size() const noexcept { return size_; }

    /**
     * @brief 检查是否为空
     * @return bool
     */
    bool empty() const noexcept { return size_ == 0; }

    /**
     * @brief 转换为 BufferView
     * @return BufferView
     */
    BufferView to_view() const noexcept {
        return BufferView(data_, size_);
    }

    /**
     * @brief 创建子视图
     * @param offset 偏移量
     * @param length 长度
     * @return MutableBufferView
     */
    MutableBufferView subview(size_t offset, size_t length) noexcept {
        if (offset >= size_) {
            return MutableBufferView();
        }
        return MutableBufferView(data_ + offset, std::min(length, size_ - offset));
    }

    /**
     * @brief 访问元素
     * @param index 索引
     * @return byte&
     */
    byte& operator[](size_t index) noexcept {
        return data_[index];
    }

    /**
     * @brief 访问元素 (const)
     * @param index 索引
     * @return const byte&
     */
    const byte& operator[](size_t index) const noexcept {
        return data_[index];
    }

    /**
     * @brief 前向迭代器
     * @return byte*
     */
    byte* begin() noexcept { return data_; }

    /**
     * @brief 后向迭代器
     * @return byte*
     */
    byte* end() noexcept { return data_ + size_; }

private:
    byte* data_;
    size_t size_;
};

END_NAMESPACE_COMMON
