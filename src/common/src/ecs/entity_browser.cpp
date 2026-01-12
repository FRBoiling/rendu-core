//
// Entity Browser 实现
//

#include "common/ecs/entity_browser.h"
#include "common/ecs/registry_optimized.h"
#include "common/ecs/relationships.h"
#include "common/ecs/archetype.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <typeinfo>
#include <cxxabi.h>
#include <memory>

BEGIN_NAMESPACE_ECS

EntityBrowser::EntityBrowser()
    : m_refreshed(false)
    , m_registry(nullptr)
    , m_relationships(nullptr)
{
    m_statistics = {};
}

EntityBrowser::~EntityBrowser() = default;

void EntityBrowser::refresh(const RegistryOptimized& registry) {
    m_registry = &registry;
    m_relationships = nullptr;
    m_entityCache.clear();

    // 遍历所有实体并收集信息
    size_t entityCount = 0;
    registry.each([this, &entityCount, &registry](Entity entity) {
        EntityInfo info;
        info.entity = entity;
        info.valid = true;

        // 没有关系管理器时，父实体为空
        info.parent = Entity{};
        info.children.clear();

        // 获取组件类型信息
        if (const auto* componentTypes = registry.getEntityComponentTypes(entity.index())) {
            for (const auto& typeIndex : *componentTypes) {
                ComponentInfo compInfo;

                // 使用 demangle 函数获取可读的类型名称
                const char* typeName = typeIndex.name();
                #ifdef __GNUC__
                    int status = 0;
                    char* demangled = abi::__cxa_demangle(typeName, nullptr, nullptr, &status);
                    if (status == 0 && demangled) {
                        compInfo.typeName = demangled;
                        std::free(demangled);
                    } else {
                        compInfo.typeName = typeName;
                    }
                #else
                    compInfo.typeName = typeName;
                #endif

                compInfo.size = 0;
                compInfo.offset = 0;

                std::ostringstream oss;
                oss << "Component@" << entity.value();
                compInfo.value = oss.str();

                info.components.push_back(compInfo);
            }
        }

        m_entityCache[entity.value()] = info;
        entityCount++;
    });

    m_refreshed = true;
    updateStatistics();
}

void EntityBrowser::refresh(const RegistryOptimized& registry, const RelationshipManager& relationships) {
    m_registry = &registry;
    m_relationships = &relationships;
    m_entityCache.clear();

    // 遍历所有实体并收集信息
    size_t entityCount = 0;
    registry.each([this, &entityCount, &registry, &relationships](Entity entity) {
        EntityInfo info;
        info.entity = entity;
        info.valid = true;

        // 获取父实体
        info.parent = relationships.getParent(entity);

        // 获取子实体
        info.children.clear();
        info.children = relationships.getChildren(entity);

        // 获取组件类型信息
        if (const auto* componentTypes = registry.getEntityComponentTypes(entity.index())) {
            for (const auto& typeIndex : *componentTypes) {
                ComponentInfo compInfo;

                // 使用 demangle 函数获取可读的类型名称
                const char* typeName = typeIndex.name();
                #ifdef __GNUC__
                    int status = 0;
                    char* demangled = abi::__cxa_demangle(typeName, nullptr, nullptr, &status);
                    if (status == 0 && demangled) {
                        compInfo.typeName = demangled;
                        std::free(demangled);
                    } else {
                        compInfo.typeName = typeName;
                    }
                #else
                    compInfo.typeName = typeName;
                #endif

                compInfo.size = 0;
                compInfo.value = "";
                info.components.push_back(compInfo);
            }
        }

        m_entityCache[entity.value()] = info;
        entityCount++;
    });

    m_refreshed = true;
    updateStatistics();
}



std::vector<EntityBrowser::EntityInfo> EntityBrowser::getEntities() const {
    std::vector<EntityInfo> entities;
    entities.reserve(m_entityCache.size());

    for (const auto& pair : m_entityCache) {
        entities.push_back(pair.second);
    }

    return entities;
}

std::vector<EntityBrowser::EntityInfo> EntityBrowser::getValidEntities() const {
    std::vector<EntityInfo> validEntities;

    for (const auto& pair : m_entityCache) {
        if (pair.second.valid) {
            validEntities.push_back(pair.second);
        }
    }

    return validEntities;
}

EntityBrowser::EntityInfo EntityBrowser::getEntityInfo(Entity entity) const {
    auto it = m_entityCache.find(entity.value());
    if (it != m_entityCache.end()) {
        return it->second;
    }

    EntityInfo info;
    info.entity = entity;
    info.valid = false;
    return info;
}

std::vector<EntityBrowser::ComponentInfo> EntityBrowser::getComponents(Entity entity) const {
    auto it = m_entityCache.find(entity.value());
    if (it != m_entityCache.end()) {
        return it->second.components;
    }
    return {};
}

std::vector<EntityBrowser::EntityInfo> EntityBrowser::searchByComponent(const std::string& componentType) const {
    std::vector<EntityInfo> results;

    for (const auto& pair : m_entityCache) {
        const auto& info = pair.second;
        for (const auto& comp : info.components) {
            if (comp.typeName == componentType) {
                results.push_back(info);
                break;
            }
        }
    }

    return results;
}

std::vector<EntityBrowser::EntityInfo> EntityBrowser::searchByValue(const std::string& componentType,
                                                                     const std::string& valuePattern) const {
    std::vector<EntityInfo> results;

    for (const auto& pair : m_entityCache) {
        const auto& info = pair.second;
        for (const auto& comp : info.components) {
            if (comp.typeName == componentType && comp.value.find(valuePattern) != std::string::npos) {
                results.push_back(info);
                break;
            }
        }
    }

    return results;
}

std::vector<EntityBrowser::EntityInfo> EntityBrowser::advancedSearch(const std::function<bool(const EntityInfo&)>& filter) const {
    std::vector<EntityInfo> results;

    for (const auto& pair : m_entityCache) {
        if (filter(pair.second)) {
            results.push_back(pair.second);
        }
    }

    return results;
}

EntityBrowser::Statistics EntityBrowser::getStatistics() const {
    return m_statistics;
}

void EntityBrowser::printEntityInfo(const EntityInfo& info) {
    std::cout << "Entity " << info.entity.value() << " (valid: " << (info.valid ? "yes" : "no") << ")\n";

    if (info.components.empty()) {
        std::cout << "  No components\n";
    } else {
        std::cout << "  Components:\n";
        for (const auto& comp : info.components) {
            std::cout << "    - " << comp.typeName << " (size: " << comp.size << " bytes)\n";
            std::cout << "      Value: " << comp.value << "\n";
        }
    }

    if (info.parent.value() != 0) {
        std::cout << "  Parent: Entity " << info.parent.value() << "\n";
    }

    if (!info.children.empty()) {
        std::cout << "  Children: ";
        for (size_t i = 0; i < info.children.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << info.children[i].value();
        }
        std::cout << "\n";
    }
}

void EntityBrowser::printAllEntities() const {
    std::cout << "\n=== All Entities ===\n";
    std::cout << "Total: " << m_entityCache.size() << " entities\n\n";

    for (const auto& pair : m_entityCache) {
        printEntityInfo(pair.second);
        std::cout << "\n";
    }
}

void EntityBrowser::printStatistics() const {
    std::cout << "\n=== Entity Browser Statistics ===\n";
    std::cout << "Total entities: " << m_statistics.totalEntities << "\n";
    std::cout << "Valid entities: " << m_statistics.validEntities << "\n";
    std::cout << "Total components: " << m_statistics.totalComponents << "\n";
    std::cout << "Unique component types: " << m_statistics.uniqueComponentTypes << "\n";

    if (!m_statistics.componentCounts.empty()) {
        std::cout << "\nComponent breakdown:\n";
        for (const auto& pair : m_statistics.componentCounts) {
            std::cout << "  " << pair.first << ": " << pair.second << "\n";
        }
    }

    std::cout << "==============================\n";
}

std::string EntityBrowser::exportEntityToJson(Entity entity) const {
    auto info = getEntityInfo(entity);

    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"id\": " << entity.value() << ",\n";
    oss << "  \"valid\": " << (info.valid ? "true" : "false") << ",\n";

    if (!info.components.empty()) {
        oss << "  \"components\": [\n";
        for (size_t i = 0; i < info.components.size(); ++i) {
            const auto& comp = info.components[i];
            oss << "    {\n";
            oss << "      \"type\": \"" << comp.typeName << "\",\n";
            oss << "      \"size\": " << comp.size << ",\n";
            oss << "      \"value\": \"" << comp.value << "\"\n";
            oss << "    }";
            if (i < info.components.size() - 1) {
                oss << ",";
            }
            oss << "\n";
        }
        oss << "  ],\n";
    }

    if (info.parent.value() != 0) {
        oss << "  \"parent\": " << info.parent.value() << ",\n";
    }

    if (!info.children.empty()) {
        oss << "  \"children\": [";
        for (size_t i = 0; i < info.children.size(); ++i) {
            oss << info.children[i].value();
            if (i < info.children.size() - 1) {
                oss << ", ";
            }
        }
        oss << "]\n";
    } else {
        oss << "  \"children\": []\n";
    }

    oss << "}";
    return oss.str();
}

std::string EntityBrowser::exportAllToJson() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"entities\": [\n";

    const auto entities = getEntities();
    for (size_t i = 0; i < entities.size(); ++i) {
        const auto& info = entities[i];
        oss << "    {\n";
        oss << "      \"id\": " << info.entity.value() << ",\n";
        oss << "      \"valid\": " << (info.valid ? "true" : "false") << ",\n";

        if (!info.components.empty()) {
            oss << "      \"components\": [";
            for (size_t j = 0; j < info.components.size(); ++j) {
                const auto& comp = info.components[j];
                oss << "\"" << comp.typeName << "\"";
                if (j < info.components.size() - 1) {
                    oss << ", ";
                }
            }
            oss << "]\n";
        }

        oss << "    }";
        if (i < entities.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "  ]\n";
    oss << "}";
    return oss.str();
}

std::string EntityBrowser::exportStatisticsToJson() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"totalEntities\": " << m_statistics.totalEntities << ",\n";
    oss << "  \"validEntities\": " << m_statistics.validEntities << ",\n";
    oss << "  \"totalComponents\": " << m_statistics.totalComponents << ",\n";
    oss << "  \"uniqueComponentTypes\": " << m_statistics.uniqueComponentTypes << ",\n";

    oss << "  \"componentCounts\": {\n";
    bool first = true;
    for (const auto& pair : m_statistics.componentCounts) {
        if (!first) oss << ",\n";
        oss << "    \"" << pair.first << "\": " << pair.second;
        first = false;
    }
    oss << "\n  }\n";
    oss << "}";

    return oss.str();
}

void EntityBrowser::updateStatistics() {
    m_statistics = {};
    m_statistics.totalEntities = m_entityCache.size();

    for (const auto& pair : m_entityCache) {
        const auto& info = pair.second;

        if (info.valid) {
            m_statistics.validEntities++;
        }

        m_statistics.totalComponents += info.components.size();

        for (const auto& comp : info.components) {
            m_statistics.componentCounts[comp.typeName]++;
        }
    }

    m_statistics.uniqueComponentTypes = m_statistics.componentCounts.size();
}

END_NAMESPACE_ECS
