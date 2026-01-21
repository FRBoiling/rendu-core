//
// Created by boil on 2026/1/20.
//

// ============================================================================
// result.h - 数据库查询结果
// ============================================================================

#ifndef RENDU_DATABASE_RESULT_H
#define RENDU_DATABASE_RESULT_H

#include "common/database/types.h"
#include <string>
#include <vector>
#include <map>

BEGIN_NAMESPACE_COMMON

namespace Database
{

// ============================================================================
// QueryResult - 查询结果类
// ============================================================================

/**
 * @brief 数据库查询结果
 */
class RC_COMMON_API QueryResult
{
public:
    /**
     * @brief 构造函数
     */
    QueryResult();

    /**
     * @brief 析构函数
     */
    ~QueryResult();

    // 禁用复制
    QueryResult(const QueryResult&) = delete;
    QueryResult& operator=(const QueryResult&) = delete;

    // 支持移动
    QueryResult(QueryResult&&) noexcept = default;
    QueryResult& operator=(QueryResult&&) noexcept = default;

    /**
     * @brief 获取结果集
     * @return 结果集引用
     */
    [[nodiscard]] const ResultSet& getRows() const;

    /**
     * @brief 获取行数
     * @return 行数
     */
    [[nodiscard]] size_t getRowCount() const;

    /**
     * @brief 获取列数
     * @return 列数
     */
    [[nodiscard]] size_t getColumnCount() const;

    /**
     * @brief 获取指定行
     * @param rowIndex 行索引
     * @return 行引用
     */
    [[nodiscard]] const Row& getRow(size_t rowIndex) const;

    /**
     * @brief 获取指定单元格的值
     * @param rowIndex 行索引
     * @param columnName 列名
     * @return 值引用
     */
    [[nodiscard]] const Value& getValue(size_t rowIndex, const std::string& columnName) const;

    /**
     * @brief 模板：获取指定单元格的值（类型安全）
     * @tparam T 值类型
     * @param rowIndex 行索引
     * @param columnName 列名
     * @return 值
     */
    template <typename T>
    T getValue(size_t rowIndex, const std::string& columnName) const
    {
        const Value& val = getValue(rowIndex, columnName);
        return std::get<T>(val);
    }

    /**
     * @brief 获取最后一行插入的 ID
     * @return 插入的 ID
     */
    [[nodiscard]] uint64 getLastInsertId() const;

    /**
     * @brief 获取受影响的行数
     * @return 受影响的行数
     */
    [[nodiscard]] uint64 getAffectedRows() const;

    /**
     * @brief 是否有结果
     * @return true 如果有结果
     */
    [[nodiscard]] bool hasResult() const;

    /**
     * @brief 添加一行
     * @param row 行数据
     */
    void addRow(const Row& row);

    /**
     * @brief 设置最后一行插入的 ID
     * @param id 插入的 ID
     */
    void setLastInsertId(uint64 id);

    /**
     * @brief 设置受影响的行数
     * @param rows 受影响的行数
     */
    void setAffectedRows(uint64 rows);

private:
    ResultSet m_rows;          ///< 结果集
    uint64 m_lastInsertId = 0; ///< 最后一行插入的 ID
    uint64 m_affectedRows = 0;  ///< 受影响的行数
};

} // namespace Database

END_NAMESPACE_COMMON

#endif // RENDU_DATABASE_RESULT_H
