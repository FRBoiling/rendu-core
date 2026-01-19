//
// Relationship Visualizer 实现
//

#include "common/ecs/relationship_visualizer.h"
#include "common/ecs/relationships.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unordered_set>

#include "common/ecs/registry_optimized.h"

BEGIN_NAMESPACE_ECS
    // 注意：由于 RelationshipManager 的 API 与设计不同，
// 这里提供一个简化的实现版本

RelationshipVisualizer::Tree RelationshipVisualizer::buildTree(const RelationshipManager& relationships) {
    Tree tree;
    tree.nodes.clear();
    tree.roots.clear();
    tree.totalNodes = 0;
    tree.maxDepth = 0;

    // 获取所有参与关系的实体索引
    std::vector<uint32> allEntityIndices = relationships.getAllEntityIndices();

    // 递归收集所有节点
    std::function<void(Entity, size_t)> collectNode = [&](Entity entity, size_t depth) {
        uint32 entityId = entity.value();

        // 跳过已访问的节点
        if (tree.nodes.find(entityId) != tree.nodes.end()) {
            return;
        }

        // 创建节点
        Node node;
        node.entity = entity;
        node.label = "Entity_" + std::to_string(entity.value());
        node.depth = depth;
        node.parent = Entity();
        node.siblingIndex = 0;
        node.children = relationships.getChildren(entity);

        tree.nodes[entityId] = node;
        tree.totalNodes++;
        tree.maxDepth = std::max(tree.maxDepth, depth);

        // 递归处理子节点（先收集，再设置关系）
        for (size_t i = 0; i < node.children.size(); ++i) {
            collectNode(node.children[i], depth + 1);
            // 设置父节点和兄弟索引
            if (tree.nodes.find(node.children[i].value()) != tree.nodes.end()) {
                tree.nodes[node.children[i].value()].parent = entity;
                tree.nodes[node.children[i].value()].siblingIndex = i;
            }
        }
    };

    // 遍历所有实体，找到根节点并收集
    for (uint32 entityId : allEntityIndices) {
        Entity entity(entityId);
        Entity parent = relationships.getParent(entity);

        // 如果没有父节点，就是根节点
        if (!parent.valid()) {
            tree.roots.push_back(entity);
            collectNode(entity, 0);
        }
    }

    return tree;
}

RelationshipVisualizer::Tree RelationshipVisualizer::buildTree(const RelationshipManager& relationships,
                                                             const RegistryOptimized& registry) {
    Tree tree;
    tree.nodes.clear();
    tree.roots.clear();
    tree.totalNodes = 0;
    tree.maxDepth = 0;

    // 收集所有在 RelationshipManager 中的实体
    std::unordered_set<uint32> allEntities;

    // 先遍历 registry 中的实体，收集它们和它们的子节点
    registry.each([&](Entity entity) {
        uint32 entityId = entity.value();
        allEntities.insert(entityId);

        // 获取该实体的子节点
        const auto& children = relationships.getChildren(entity);
        for (const auto& child : children) {
            allEntities.insert(child.value());
        }
    });

    // 递归收集所有节点
    std::function<void(Entity, size_t)> collectNode = [&](Entity entity, size_t depth) {
        uint32 entityId = entity.value();

        // 跳过已访问的节点
        if (tree.nodes.find(entityId) != tree.nodes.end()) {
            return;
        }

        // 创建节点
        Node node;
        node.entity = entity;
        node.label = "Entity_" + std::to_string(entity.value());
        node.depth = depth;
        node.parent = Entity();
        node.siblingIndex = 0;
        node.children = relationships.getChildren(entity);

        tree.nodes[entityId] = node;
        tree.totalNodes++;
        tree.maxDepth = std::max(tree.maxDepth, depth);

        // 递归处理子节点（先收集，再设置关系）
        for (size_t i = 0; i < node.children.size(); ++i) {
            collectNode(node.children[i], depth + 1);
            // 设置父节点和兄弟索引
            if (tree.nodes.find(node.children[i].value()) != tree.nodes.end()) {
                tree.nodes[node.children[i].value()].parent = entity;
                tree.nodes[node.children[i].value()].siblingIndex = i;
            }
        }
    };

    // 遍历所有实体，找到根节点并收集
    for (uint32 entityId : allEntities) {
        Entity entity(entityId);
        Entity parent = relationships.getParent(entity);

        // 如果没有父节点，就是根节点
        if (!parent.valid()) {
            tree.roots.push_back(entity);
            collectNode(entity, 0);
        }
    }

    return tree;
}

RelationshipVisualizer::Tree RelationshipVisualizer::getRelationshipTree(const RelationshipManager& relationships) {
    return buildTree(relationships);
}

RelationshipVisualizer::Tree RelationshipVisualizer::getRelationshipTree(const RelationshipManager& relationships,
                                                                       const RegistryOptimized& registry) {
    return buildTree(relationships, registry);
}

RelationshipVisualizer::Tree RelationshipVisualizer::getSubTree(const RelationshipManager& relationships, Entity root) const {
    Tree tree;
    tree.nodes.clear();
    tree.roots.clear();
    tree.totalNodes = 0;
    tree.maxDepth = 0;

    // 递归收集子节点
    std::function<void(Entity, size_t)> collect = [&](Entity entity, size_t depth) {
        Node node;
        node.entity = entity;
        node.label = "Entity_" + std::to_string(entity.value());
        node.depth = depth;
        node.siblingIndex = 0;

        tree.nodes[entity.value()] = node;
        tree.totalNodes++;
        tree.maxDepth = std::max(tree.maxDepth, depth);

        const auto& children = relationships.getChildren(entity);
        for (size_t i = 0; i < children.size(); ++i) {
            collect(children[i], depth + 1);
            tree.nodes[children[i].value()].parent = entity;
            tree.nodes[children[i].value()].siblingIndex = i;
        }
    };

    collect(root, 0);
    tree.roots.push_back(root);

    return tree;
}

std::vector<RelationshipVisualizer::Node> RelationshipVisualizer::searchNodes(
    const Tree& tree, const std::function<bool(const Node&)>& filter) {

    std::vector<Node> results;
    for (const auto& pair : tree.nodes) {
        if (filter(pair.second)) {
            results.push_back(pair.second);
        }
    }
    return results;
}

void RelationshipVisualizer::printTree(const Tree& tree) const {
    for (const auto& root : tree.roots) {
        printSubTreeImpl(tree, root.value(), 0);
    }
}

void RelationshipVisualizer::printSubTree(const RelationshipManager& relationships, Entity root) const {
    Tree subtree = getSubTree(relationships, root);
    printTree(subtree);
}

void RelationshipVisualizer::printSubTreeImpl(const Tree& tree, uint32 entityId, int indent) const {
    auto it = tree.nodes.find(entityId);
    if (it == tree.nodes.end()) {
        return;
    }

    const Node& node = it->second;

    // 打印缩进
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }

    // 打印节点
    std::cout << node.label << " (ID: " << node.entity.value() << ", Depth: " << node.depth << ")" << std::endl;

    // 递归打印子节点
    for (const auto& child : node.children) {
        printSubTreeImpl(tree, child.value(), indent + 1);
    }
}

std::string RelationshipVisualizer::exportToDOT(const Tree& tree, const DotExportOptions& options) const {
    std::ostringstream oss;
    oss << "digraph EntityRelationships {\n";
    oss << "  node [fontname=\"" << options.fontName << "\", fontsize=" << options.fontSize << "];\n";
    oss << "  edge [fontname=\"" << options.fontName << "\", fontsize=" << options.fontSize << "];\n\n";

    for (const auto& pair : tree.nodes) {
        const Node& node = pair.second;
        oss << "  \"" << node.entity.value() << "\" [";

        if (options.showLabels) {
            oss << "label=\"" << node.label << "\"";
        }

        if (options.showEntityIds && options.showLabels) {
            oss << "\\n";
        }

        if (options.showEntityIds) {
            oss << node.entity.value();
        }

        if (options.showDepth) {
            oss << "\\nDepth: " << node.depth;
        }

        oss << "];\n";

        for (const auto& child : node.children) {
            oss << "  \"" << node.entity.value() << "\" -> \"" << child.value() << "\";\n";
        }
    }

    oss << "}";
    return oss.str();
}

std::string RelationshipVisualizer::exportToDOT(const RelationshipManager& relationships,
                                               const DotExportOptions& options) const {
    Tree tree;
    // 由于无法直接访问 RelationshipManager 的内部，返回简化实现
    // tree = buildTree(const_cast<RelationshipManager&>(relationships));
    return exportToDOT(tree, options);
}

std::string RelationshipVisualizer::exportSubTreeToDOT(const RelationshipManager& relationships,
                                                        Entity root,
                                                        const DotExportOptions& options) const {
    Tree subtree = getSubTree(relationships, root);
    return exportToDOT(subtree, options);
}

bool RelationshipVisualizer::exportToImage(const Tree& tree, const std::string& filename,
                                          const DotExportOptions& options) const {
    std::string dotContent = exportToDOT(tree, options);

    // 创建临时 DOT 文件
    std::string tempDot = filename + ".dot";
    std::ofstream dotFile(tempDot);
    if (!dotFile.is_open()) {
        return false;
    }
    dotFile << dotContent;
    dotFile.close();

    // 调用 Graphviz 的 dot 工具生成图片
    std::string command = "dot -Tpng " + tempDot + " -o " + filename;
    int result = std::system(command.c_str());

    // 清理临时文件
    std::remove(tempDot.c_str());

    return result == 0;
}

std::string RelationshipVisualizer::exportToJson(const Tree& tree) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"totalNodes\": " << tree.totalNodes << ",\n";
    oss << "  \"maxDepth\": " << tree.maxDepth << ",\n";
    oss << "  \"roots\": [";

    for (size_t i = 0; i < tree.roots.size(); ++i) {
        oss << tree.roots[i].value();
        if (i < tree.roots.size() - 1) {
            oss << ", ";
        }
    }

    oss << "],\n";
    oss << "  \"nodes\": {\n";

    size_t nodeCount = 0;
    for (const auto& pair : tree.nodes) {
        const Node& node = pair.second;
        oss << "    \"" << node.entity.value() << "\": {\n";
        oss << "      \"label\": \"" << node.label << "\",\n";
        oss << "      \"depth\": " << node.depth << ",\n";
        oss << "      \"siblingIndex\": " << node.siblingIndex << ",\n";
        oss << "      \"parent\": " << (node.parent.value() ? std::to_string(node.parent.value()) : "null") << ",\n";
        oss << "      \"children\": [";

        for (size_t i = 0; i < node.children.size(); ++i) {
            oss << node.children[i].value();
            if (i < node.children.size() - 1) {
                oss << ", ";
            }
        }

        oss << "]\n";
        oss << "    }";

        if (++nodeCount < tree.nodes.size()) {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "  }\n";
    oss << "}";
    return oss.str();
}

RelationshipVisualizer::TreeStats RelationshipVisualizer::getTreeStats(const Tree& tree) {
    TreeStats stats;
    stats.totalNodes = tree.totalNodes;
    stats.maxDepth = tree.maxDepth;
    stats.rootCount = tree.roots.size();

    // 计算每层节点数
    for (const auto& pair : tree.nodes) {
        size_t depth = pair.second.depth;
        if (depth >= stats.nodesPerLevel.size()) {
            stats.nodesPerLevel.resize(depth + 1, 0);
        }
        stats.nodesPerLevel[depth]++;
    }

    // 计算平均子节点数
    size_t totalChildren = 0;
    size_t parentCount = 0;
    for (const auto& pair : tree.nodes) {
        size_t childCount = pair.second.children.size();
        if (childCount > 0) {
            totalChildren += childCount;
            parentCount++;
        }
    }
    stats.avgChildrenPerNode = parentCount > 0 ? static_cast<float>(totalChildren) / parentCount : 0.0f;

    return stats;
}

std::string RelationshipVisualizer::getTreeStatistics(const Tree& tree) const {
    auto stats = const_cast<RelationshipVisualizer*>(this)->getTreeStats(tree);

    std::ostringstream oss;
    oss << "=== Relationship Tree Statistics ===\n";
    oss << "Total nodes: " << stats.totalNodes << "\n";
    oss << "Max depth: " << stats.maxDepth << "\n";
    oss << "Root count: " << stats.rootCount << "\n";
    oss << "Average children per node: " << stats.avgChildrenPerNode << "\n";

    if (!stats.nodesPerLevel.empty()) {
        oss << "Nodes per level:\n";
        for (size_t i = 0; i < stats.nodesPerLevel.size(); ++i) {
            oss << "  Level " << i << ": " << stats.nodesPerLevel[i] << " nodes\n";
        }
    }

    oss << "================================\n";
    return oss.str();
}

void RelationshipVisualizer::calculateDepth(Tree& tree, uint32 entityId, size_t depth) {
    auto it = tree.nodes.find(entityId);
    if (it == tree.nodes.end()) {
        return;
    }

    it->second.depth = depth;

    for (const auto& child : it->second.children) {
        calculateDepth(tree, child.value(), depth + 1);
    }
}

END_NAMESPACE_ECS
