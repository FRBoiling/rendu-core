//
// Created by boil on 2026/1/15.
// ECS 序列化系统
//

#ifndef RENDU_ECS_ARCHIVE_H
#define RENDU_ECS_ARCHIVE_H

#include "common/ecs/entity.h"
#include "common/define.h"
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <optional>
#include <iostream>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // 序列化版本
    // ============================================================================

    constexpr uint32 ARCHIVE_VERSION = 1;
    constexpr uint32 ARCHIVE_MAGIC = 0x52454443; // "REDC" in hex

    // ============================================================================
    // 序列化数据结构
    // ============================================================================

    /**
     * @brief 组件数据（类型擦除）
     */
    struct SerializedComponent
    {
        std::string typeName;
        std::vector<uint8_t> data;
    };

    /**
     * @brief 实体数据
     */
    struct SerializedEntity
    {
        uint32 id;
        uint32 version;
        std::vector<SerializedComponent> components;
    };

    /**
     * @brief 序列化的注册表快照
     */
    struct SerializedRegistry
    {
        uint32 version;
        uint32 magic;
        uint32 nextEntityId;
        std::vector<SerializedEntity> entities;
    };

    // ============================================================================
    // 输出存档器
    // ============================================================================

    /**
     * @brief 将注册表序列化到输出流
     */
    class RC_COMMON_API OutputArchive
    {
    public:
        explicit OutputArchive(std::ostream& os);
        virtual ~OutputArchive() = default;

    /**
     * @brief 序列化注册表
     */
    void save(const class RegistryOptimized& registry);

    /**
     * @brief 保存到文件
     */
    static bool saveToFile(
        const class RegistryOptimized& registry,
        const std::string& filename);

    protected:
        /**
         * @brief 写入头信息
         */
        virtual void writeHeader(uint32 nextEntityId, size_t entityCount);

        /**
         * @brief 写入实体
         */
        virtual void writeEntity(const SerializedEntity& entity);

        /**
         * @brief 写入组件
         */
        virtual void writeComponent(const SerializedComponent& component);

        std::ostream& m_stream;
    };

    // ============================================================================
    // 文本输出存档器 (JSON 格式)
    // ============================================================================

    /**
     * @brief JSON 格式的序列化输出
     */
    class RC_COMMON_API JsonOutputArchive : public OutputArchive
    {
    public:
        explicit JsonOutputArchive(std::ostream& os);

    protected:
        void writeHeader(uint32 nextEntityId, size_t entityCount) override;
        void writeEntity(const SerializedEntity& entity) override;
        void writeComponent(const SerializedComponent& component) override;

    private:
        void writeIndent();
        void writeString(const std::string& str);
        void writeData(const std::vector<uint8_t>& data);

        int m_indentLevel = 0;
    };

    // ============================================================================
    // 输入存档器
    // ============================================================================

    /**
     * @brief 从输入流反序列化注册表
     */
    class RC_COMMON_API InputArchive
    {
    public:
        explicit InputArchive(std::istream& is);
        virtual ~InputArchive() = default;

    /**
     * @brief 反序列化到注册表
     */
    void load(class RegistryOptimized& registry);

    /**
     * @brief 从文件加载
     */
    static bool loadFromFile(
        class RegistryOptimized& registry,
        const std::string& filename);

    protected:
        /**
         * @brief 读取头信息
         */
        virtual bool readHeader(uint32& nextEntityId, size_t& entityCount);

        /**
         * @brief 读取实体
         */
        virtual bool readEntity(SerializedEntity& entity);

        /**
         * @brief 读取组件
         */
        virtual bool readComponent(SerializedComponent& component);

        std::istream& m_stream;
    };

    // ============================================================================
    // 文本输入存档器 (JSON 格式)
    // ============================================================================

    /**
     * @brief JSON 格式的反序列化输入
     */
    class RC_COMMON_API JsonInputArchive : public InputArchive
    {
    public:
        explicit JsonInputArchive(std::istream& is);

    protected:
        bool readHeader(uint32& nextEntityId, size_t& entityCount) override;
        bool readEntity(SerializedEntity& entity) override;
        bool readComponent(SerializedComponent& component) override;

    private:
        std::string readToken();
        std::string readString();
        std::string readData();
        void skipWhitespace();
    };

    // ============================================================================
    // 存档辅助函数
    // ============================================================================

    /**
     * @brief 创建注册表的序列化快照
     */
    RC_COMMON_API SerializedRegistry createSnapshot(const class RegistryOptimized& registry);

    /**
     * @brief 从快照恢复注册表
     */
    RC_COMMON_API void restoreFromSnapshot(
        class RegistryOptimized& registry,
        const SerializedRegistry& snapshot);

END_NAMESPACE_ECS

#endif // RENDU_ECS_ARCHIVE_H
