//
// Relationship Visualizer - 关系树可视化
// 用于可视化和导出实体间的关系结构
//

#pragma once

#include "common/define.h"
#include "entity.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

BEGIN_NAMESPACE_ECS

// 前向声明
class RelationshipManager;
class RegistryOptimized;

/// @brief 关系可视化 - 提供实体关系树的可视化功能
class RelationshipVisualizer {
public:
    /// @brief 关系树节点
    struct Node {
        Entity entity;                    ///< 实体 ID
        std::string label;                ///< 节点标签
        std::vector<Entity> children;     ///< 子实体列表
        Entity parent;                    ///< 父实体
        size_t depth;                      ///< 深度（从根节点开始）
        size_t siblingIndex;              ///< 在兄弟节点中的索引
    };

    /// @brief 关系树
    struct Tree {
        std::unordered_map<uint32, Node> nodes;  ///< 所有节点
        std::vector<Entity> roots;               ///< 根节点列表（没有父节点的实体）
        size_t totalNodes;                       ///< 总节点数
        size_t maxDepth;                         ///< 最大深度
    };

    /// @brief 树统计信息
    struct TreeStats {
        size_t totalNodes;                     ///< 总节点数
        size_t maxDepth;                       ///< 最大深度
        size_t rootCount;                      ///< 根节点数量
        float avgChildrenPerNode;              ///< 平均每个节点的子节点数
        std::vector<size_t> nodesPerLevel;     ///< 每层的节点数量
    };

    /// @brief DOT 导出选项
    struct DotExportOptions {
        bool showEntityIds;       ///< 是否显示实体 ID
        bool showLabels;          ///< 是否显示标签
        bool colorize;           ///< 是否使用颜色
        bool showDepth;         ///< 是否显示深度
        std::string fontName; ///< 字体名称
        int fontSize;              ///< 字体大小

        /// @brief 构造默认选项
        DotExportOptions()
            : showEntityIds(true), showLabels(true), colorize(true),
              showDepth(false), fontName("Arial"), fontSize(12) {}
    };

    /// @brief 构造函数
    RelationshipVisualizer() = default;

    /// @brief 析构函数
    ~RelationshipVisualizer() = default;

    /// @brief 构建关系树
    /// @param relationships 关系管理器
    /// @return 关系树
    Tree buildTree(const RelationshipManager& relationships);

    /// @brief 构建关系树（带注册表，获取实体标签）
    /// @param relationships 关系管理器
    /// @param registry 注册表（用于获取实体信息）
    /// @return 关系树
    Tree buildTree(const RelationshipManager& relationships, const RegistryOptimized& registry);

    /// @brief 获取关系树
    /// @param relationships 关系管理器
    /// @return 关系树
    Tree getRelationshipTree(const RelationshipManager& relationships);

    /// @brief 获取关系树（带标签）
    /// @param relationships 关系管理器
    /// @param registry 注册表
    /// @return 关系树
    Tree getRelationshipTree(const RelationshipManager& relationships,
                           const RegistryOptimized& registry);

    /// @brief 获取子节点树（递归获取所有后代）
    /// @param relationships 关系管理器
    /// @param root 根实体
    /// @return 子节点树
    Tree getSubTree(const RelationshipManager& relationships, Entity root) const;

    /// @brief 搜索节点
    /// @param tree 关系树
    /// @param filter 过滤函数，返回 true 表示匹配
    /// @return 匹配的节点列表
    std::vector<Node> searchNodes(const Tree& tree, const std::function<bool(const Node&)>& filter);

    /// @brief 打印关系树（文本格式）
    /// @param tree 关系树
    void printTree(const Tree& tree) const;

    /// @brief 打印子节点树
    /// @param relationships 关系管理器
    /// @param root 根实体
    void printSubTree(const RelationshipManager& relationships, Entity root) const;

    /// @brief 导出为 DOT 格式（Graphviz）
    /// @param tree 关系树
    /// @param options 导出选项
    /// @return DOT 格式字符串
    std::string exportToDOT(const Tree& tree, const DotExportOptions& options = DotExportOptions()) const;

    /// @brief 导出为 DOT 格式（简化）
    /// @param relationships 关系管理器
    /// @param options 导出选项
    /// @return DOT 格式字符串
    std::string exportToDOT(const RelationshipManager& relationships,
                           const DotExportOptions& options = DotExportOptions()) const;

    /// @brief 导出子节点树为 DOT 格式
    /// @param relationships 关系管理器
    /// @param root 根实体
    /// @param options 导出选项
    /// @return DOT 格式字符串
    std::string exportSubTreeToDOT(const RelationshipManager& relationships,
                                   Entity root,
                                   const DotExportOptions& options = DotExportOptions()) const;

    /// @brief 导出为图片（需要 Graphviz 的 dot 工具）
    /// @param tree 关系树
    /// @param filename 输出文件名（支持 .png, .svg, .pdf 等格式）
    /// @param options 导出选项
    /// @return 是否成功
    bool exportToImage(const Tree& tree,
                      const std::string& filename,
                      const DotExportOptions& options = DotExportOptions()) const;

    /// @brief 导出为 JSON 格式
    /// @param tree 关系树
    /// @return JSON 字符串
    std::string exportToJson(const Tree& tree) const;

    /// @brief 获取树的统计信息
    /// @param tree 关系树
    /// @return 统计信息
    TreeStats getTreeStats(const Tree& tree);

    /// @brief 获取树的统计信息（字符串格式）
    /// @param tree 关系树
    /// @return 统计信息字符串
    std::string getTreeStatistics(const Tree& tree) const;

private:
    /// @brief 递归计算节点深度
    /// @param tree 关系树
    /// @param entityId 实体 ID
    /// @param depth 当前深度
    void calculateDepth(Tree& tree, uint32 entityId, size_t depth);

    /// @brief 递归打印子树
    /// @param tree 关系树
    /// @param entityId 实体 ID
    /// @param indent 缩进级别
    void printSubTreeImpl(const Tree& tree, uint32 entityId, int indent) const;
};

END_NAMESPACE_ECS
