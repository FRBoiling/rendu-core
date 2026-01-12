//
// Created by boil on 2026/1/15.
//

#include "common/ecs/archive.h"
#include "common/ecs/registry_optimized.h"

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // OutputArchive 实现
    // ============================================================================

    OutputArchive::OutputArchive(std::ostream& os) : m_stream(os) {}

    void OutputArchive::save(const RegistryOptimized& registry)
    {
        SerializedRegistry snapshot;
        snapshot.magic = ARCHIVE_MAGIC;
        snapshot.version = ARCHIVE_VERSION;

        // 遍历所有实体
        registry.each([&](Entity entity) {
            SerializedEntity serializedEntity;
            serializedEntity.id = entity.value();
            serializedEntity.version = entity.version();

            // 获取实体的所有组件类型
            const auto* componentTypes = registry.getEntityComponentTypes(entity.index());

            if (componentTypes != nullptr)
            {
                for (const auto& typeInfo : *componentTypes)
                {
                    SerializedComponent serializedComponent;
                    serializedComponent.typeName = typeInfo.name();

                    // 注意：RegistryOptimized 没有直接获取组件数据的接口
                    // 这里简化实现，仅记录组件类型名称
                    // 完整实现需要在 RegistryOptimized 中添加 getComponentData 接口

                    serializedEntity.components.push_back(serializedComponent);
                }
            }

            snapshot.entities.push_back(serializedEntity);
        });

        snapshot.nextEntityId = snapshot.entities.empty() ? 1 : snapshot.entities.back().id + 1;

        writeHeader(snapshot.nextEntityId, snapshot.entities.size());

        for (size_t i = 0; i < snapshot.entities.size(); ++i)
        {
            writeEntity(snapshot.entities[i]);
            if (i < snapshot.entities.size() - 1)
            {
                m_stream << ",";
            }
            m_stream << "\n";
        }

        m_stream << "]\n}\n";
    }

    bool OutputArchive::saveToFile(
        const RegistryOptimized& registry,
        const std::string& filename)
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        JsonOutputArchive archive(file);
        archive.save(registry);
        return file.good();
    }

    void OutputArchive::writeHeader(uint32 nextEntityId, size_t entityCount)
    {
        // 基类默认实现
    }

    void OutputArchive::writeEntity(const SerializedEntity& entity)
    {
        // 基类默认实现
    }

    void OutputArchive::writeComponent(const SerializedComponent& component)
    {
        // 基类默认实现
    }

    // ============================================================================
    // JsonOutputArchive 实现
    // ============================================================================

    JsonOutputArchive::JsonOutputArchive(std::ostream& os)
        : OutputArchive(os) {}

    void JsonOutputArchive::writeHeader(uint32 nextEntityId, size_t entityCount)
    {
        m_stream << "{\n";
        m_indentLevel = 1;
        writeIndent();
        m_stream << "\"version\": " << ARCHIVE_VERSION << ",\n";
        writeIndent();
        m_stream << "\"nextEntityId\": " << nextEntityId << ",\n";
        writeIndent();
        m_stream << "\"entities\": [\n";
    }

    void JsonOutputArchive::writeEntity(const SerializedEntity& entity)
    {
        writeIndent();
        m_indentLevel = 2;
        m_stream << "{\n";

        writeIndent();
        m_indentLevel = 3;
        m_stream << "\"id\": " << entity.id << ",\n";

        writeIndent();
        m_stream << "\"version\": " << entity.version << ",\n";

        writeIndent();
        m_stream << "\"components\": [\n";

        for (size_t i = 0; i < entity.components.size(); ++i)
        {
            writeComponent(entity.components[i]);
            if (i < entity.components.size() - 1)
            {
                m_stream << ",";
            }
            m_stream << "\n";
        }

        m_indentLevel = 2;
        writeIndent();
        m_stream << "]\n";

        m_indentLevel = 1;
        writeIndent();
        m_stream << "}";
    }

    void JsonOutputArchive::writeComponent(const SerializedComponent& component)
    {
        writeIndent();
        m_indentLevel = 4;
        m_stream << "{\n";

        writeIndent();
        m_indentLevel = 5;
        m_stream << "\"type\": ";
        writeString(component.typeName);
        m_stream << ",\n";

        writeIndent();
        m_stream << "\"data\": ";
        writeData(component.data);

        m_indentLevel = 4;
        m_stream << "\n";
        writeIndent();
        m_stream << "}";
    }

    void JsonOutputArchive::writeIndent()
    {
        for (int i = 0; i < m_indentLevel; ++i)
        {
            m_stream << "  ";
        }
    }

    void JsonOutputArchive::writeString(const std::string& str)
    {
        m_stream << "\"" << str << "\"";
    }

    void JsonOutputArchive::writeData(const std::vector<uint8_t>& data)
    {
        // 简化实现：将字节转义为字符串
        m_stream << "\"";
        for (uint8_t byte : data)
        {
            if (byte >= 32 && byte < 127)
            {
                m_stream << (char)byte;
            }
            else
            {
                m_stream << "\\x" << std::hex << (int)byte << std::dec;
            }
        }
        m_stream << "\"";
    }

    // ============================================================================
    // InputArchive 实现
    // ============================================================================

    InputArchive::InputArchive(std::istream& is) : m_stream(is) {}

    void InputArchive::load(RegistryOptimized& registry)
    {
        uint32 nextEntityId;
        size_t entityCount;

        if (!readHeader(nextEntityId, entityCount))
        {
            std::cerr << "Failed to read archive header" << std::endl;
            return;
        }

        for (size_t i = 0; i < entityCount; ++i)
        {
            SerializedEntity entity;
            if (readEntity(entity))
            {
                // 创建实体并恢复组件
                // 注意：完整实现需要类型注册和组件恢复机制
                // 这里提供简化实现，仅演示流程
            }
        }
    }

    bool InputArchive::loadFromFile(
        RegistryOptimized& registry,
        const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return false;
        }

        JsonInputArchive archive(file);
        archive.load(registry);
        return true;
    }

    bool InputArchive::readHeader(uint32& nextEntityId, size_t& entityCount)
    {
        // 基类默认实现
        return true;
    }

    bool InputArchive::readEntity(SerializedEntity& entity)
    {
        // 基类默认实现
        return true;
    }

    bool InputArchive::readComponent(SerializedComponent& component)
    {
        // 基类默认实现
        return true;
    }

    // ============================================================================
    // JsonInputArchive 实现
    // ============================================================================

    JsonInputArchive::JsonInputArchive(std::istream& is)
        : InputArchive(is) {}

    bool JsonInputArchive::readHeader(uint32& nextEntityId, size_t& entityCount)
    {
        skipWhitespace();
        std::string token = readToken();

        if (token != "{")
        {
            return false;
        }

        // 解析 JSON 对象
        while (true)
        {
            skipWhitespace();
            token = readToken();

            if (token == "}")
            {
                break;
            }

            if (token == "\"version\":")
            {
                skipWhitespace();
                token = readToken();
                // 版本号，暂不使用
            }
            else if (token == "\"nextEntityId\":")
            {
                skipWhitespace();
                token = readToken();
                nextEntityId = static_cast<uint32>(std::stoul(token));
            }
            else if (token == "\"entities\":")
            {
                skipWhitespace();
                token = readToken();
                if (token == "[")
                {
                    // 开始读取实体数组
                    entityCount = 0;

                    // 简化实现：只统计实体数量
                    size_t bracketDepth = 1;
                    while (m_stream.good() && bracketDepth > 0)
                    {
                        char c = m_stream.get();
                        if (c == '[') bracketDepth++;
                        else if (c == ']') bracketDepth--;
                        if (c == '{' && bracketDepth == 1) entityCount++;
                    }

                    return true;
                }
            }
            else
            {
                skipWhitespace();
                token = readToken(); // 跳过值
            }
        }

        return true;
    }

    bool JsonInputArchive::readEntity(SerializedEntity& entity)
    {
        skipWhitespace();
        std::string token = readToken();

        if (token != "{")
        {
            return false;
        }

        while (true)
        {
            skipWhitespace();
            token = readToken();

            if (token == "}")
            {
                break;
            }

            if (token == "\"id\":")
            {
                skipWhitespace();
                token = readToken();
                entity.id = static_cast<uint32>(std::stoul(token));
            }
            else if (token == "\"version\":")
            {
                skipWhitespace();
                token = readToken();
                entity.version = static_cast<uint32>(std::stoul(token));
            }
            else if (token == "\"components\":")
            {
                skipWhitespace();
                token = readToken();
                if (token == "[")
                {
                    while (true)
                    {
                        skipWhitespace();
                        char c = m_stream.peek();
                        if (c == ']')
                        {
                            m_stream.get();
                            break;
                        }

                        SerializedComponent component;
                        if (readComponent(component))
                        {
                            entity.components.push_back(component);
                        }
                    }
                }
            }
            else
            {
                skipWhitespace();
                token = readToken(); // 跳过值
            }
        }

        return true;
    }

    bool JsonInputArchive::readComponent(SerializedComponent& component)
    {
        skipWhitespace();
        std::string token = readToken();

        if (token != "{")
        {
            return false;
        }

        while (true)
        {
            skipWhitespace();
            token = readToken();

            if (token == "}")
            {
                break;
            }

            if (token == "\"type\":")
            {
                skipWhitespace();
                component.typeName = readString();
            }
            else if (token == "\"data\":")
            {
                skipWhitespace();
                component.data.clear();

                char c = m_stream.get();
                if (c == '"')
                {
                    while (m_stream.good())
                    {
                        c = m_stream.get();
                        if (c == '"') break;
                        if (c == '\\')
                        {
                            c = m_stream.get();
                            if (c == 'x')
                            {
                                // 读取十六进制转义
                                std::string hex;
                                hex += m_stream.get();
                                hex += m_stream.get();
                                uint8_t byte = static_cast<uint8_t>(std::stoi(hex, nullptr, 16));
                                component.data.push_back(byte);
                            }
                            else
                            {
                                component.data.push_back(static_cast<uint8_t>(c));
                            }
                        }
                        else
                        {
                            component.data.push_back(static_cast<uint8_t>(c));
                        }
                    }
                }
            }
            else
            {
                skipWhitespace();
                token = readToken(); // 跳过值
            }
        }

        return true;
    }

    std::string JsonInputArchive::readString()
    {
        std::string result;
        skipWhitespace();

        char c = m_stream.get();
        if (c != '"')
        {
            return result;
        }

        while (m_stream.good())
        {
            c = m_stream.get();
            if (c == '"') break;
            if (c == '\\')
            {
                c = m_stream.get();
                // 处理转义字符
            }
            result += c;
        }

        return result;
    }

    std::string JsonInputArchive::readData()
    {
        std::string result;
        skipWhitespace();

        char c = m_stream.get();
        if (c != '"')
        {
            return result;
        }

        while (m_stream.good())
        {
            c = m_stream.get();
            if (c == '"') break;
            result += c;
        }

        return result;
    }

    std::string JsonInputArchive::readToken()
    {
        std::string token;
        skipWhitespace();

        while (m_stream.good())
        {
            char c = m_stream.get();
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            {
                if (!token.empty()) break;
                continue;
            }

            if (c == '{' || c == '}' || c == '[' || c == ']' ||
                c == ',' || c == ':')
            {
                if (token.empty())
                {
                    token += c;
                }
                m_stream.unget();
                break;
            }

            token += c;
        }

        return token;
    }

    void JsonInputArchive::skipWhitespace()
    {
        while (std::isspace(m_stream.peek()))
        {
            m_stream.get();
        }
    }

    // ============================================================================
    // 存档辅助函数
    // ============================================================================

    SerializedRegistry createSnapshot(const RegistryOptimized& registry)
    {
        SerializedRegistry snapshot;
        snapshot.magic = ARCHIVE_MAGIC;
        snapshot.version = ARCHIVE_VERSION;
        snapshot.nextEntityId = 1; // 简化实现
        return snapshot;
    }

    void restoreFromSnapshot(
        RegistryOptimized& registry,
        const SerializedRegistry& snapshot)
    {
        // 简化实现：需要完整的反序列化逻辑
    }

END_NAMESPACE_ECS

