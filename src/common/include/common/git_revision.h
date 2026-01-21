//
// Created by 沸腾 on 2025/10/24.
//

// ============================================================================
// git_revision.h - Git 版本信息和构建元数据
// ============================================================================

#ifndef RENDU_GIT_REVISION_H
#define RENDU_GIT_REVISION_H

#include "common.h"
#include "define.h"

BEGIN_NAMESPACE_COMMON

namespace GitRevision
{
// Git 版本信息
/**
 * @brief 获取 Git 提交哈希值
 * @return Git 提交哈希字符串
 */
RC_COMMON_API char const* GetHash();

/**
 * @brief 获取 Git 提交日期
 * @return 提交日期字符串
 */
RC_COMMON_API char const* GetDate();

/**
 * @brief 获取 Git 分支名称
 * @return 分支名称字符串
 */
RC_COMMON_API char const* GetBranch();

// 构建系统信息
/**
 * @brief 获取 CMake 命令行
 * @return CMake 命令字符串
 */
RC_COMMON_API char const* GetCMakeCommand();

/**
 * @brief 获取 CMake 版本
 * @return CMake 版本字符串
 */
RC_COMMON_API char const* GetCMakeVersion();

/**
 * @brief 获取主机操作系统版本
 * @return 主机操作系统版本字符串
 */
RC_COMMON_API char const* GetHostOSVersion();

/**
 * @brief 获取构建目录路径
 * @return 构建目录绝对路径
 */
RC_COMMON_API char const* GetBuildDirectory();

/**
 * @brief 获取源代码目录路径
 * @return 源代码目录绝对路径
 */
RC_COMMON_API char const* GetSourceDirectory();

// 数据库配置
/**
 * @brief 获取 MySQL 可执行文件路径
 * @return MySQL 可执行文件路径
 */
RC_COMMON_API char const* GetMySQLExecutable();

/**
 * @brief 获取完整数据库名称
 * @return 数据库名称字符串
 */
RC_COMMON_API char const* GetFullDatabase();

/**
 * @brief 获取热修复数据库名称
 * @return 热修复数据库名称字符串
 */
RC_COMMON_API char const* GetHotfixesDatabase();

// 版本和版权信息
/**
 * @brief 获取完整版本字符串
 * @return 包含版本、平台、架构、构建类型的完整版本信息
 */
RC_COMMON_API char const* GetFullVersion();

/**
 * @brief 获取公司名称字符串
 * @return 公司名称
 */
RC_COMMON_API char const* GetCompanyNameStr();

/**
 * @brief 获取版权信息字符串
 * @return 法律版权信息
 */
RC_COMMON_API char const* GetLegalCopyrightStr();

/**
 * @brief 获取文件版本字符串
 * @return 文件版本号
 */
RC_COMMON_API char const* GetFileVersionStr();

/**
 * @brief 获取产品版本字符串
 * @return 产品版本号
 */
RC_COMMON_API char const* GetProductVersionStr();
}

END_NAMESPACE_COMMON

#endif // RENDU_GIT_REVISION_H
