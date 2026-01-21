# RenduCore ECS 文档索引

本文档目录包含了 RenduCore ECS 框架的完整文档。

## 📚 文档分类

### 入门文档

| 文档 | 描述 | 最后更新 |
|------|------|----------|
| [ECS_USAGE.md](ECS_USAGE.md) | ECS 框架使用指南 | - |

### 优化文档

| 文档 | 描述 | 阶段 |
|------|------|------|
| [ECS_OPTIMIZATION_GUIDE.md](ECS_OPTIMIZATION_GUIDE.md) | 优化指南和最佳实践 | Phase 1-2 |
| [ECS_OPTIMIZATION_PROGRESS.md](ECS_OPTIMIZATION_PROGRESS.md) | 优化进度记录 | Phase 1-2 |
| [ECS_OPTIMIZATION_FINAL_REPORT.md](ECS_OPTIMIZATION_FINAL_REPORT.md) | 优化最终报告 | Phase 2 |
| [ECS_CACHE_OPTIMIZATION.md](ECS_CACHE_OPTIMIZATION.md) | 缓存优化专题 | Phase 2 |
| [ECS_PERFORMANCE_REPORT.md](ECS_PERFORMANCE_REPORT.md) | 性能测试报告 | Phase 2 |

### 阶段文档

#### Phase 2: 性能优化

| 文档 | 描述 |
|------|------|
| [ECS_PHASE2_SUMMARY.md](ECS_PHASE2_SUMMARY.md) | Phase 2 优化总结 |

#### Phase 3: 高级功能

| 文档 | 描述 |
|------|------|
| [ECS_PHASE3_PLAN.md](ECS_PHASE3_PLAN.md) | Phase 3 开发计划 |
| [ECS_PHASE3_SUMMARY.md](ECS_PHASE3_SUMMARY.md) | Phase 3 完成总结 |

#### Phase 4: 完善和优化

| 文档 | 描述 |
|------|------|
| [ECS_PHASE4_PLAN.md](ECS_PHASE4_PLAN.md) | Phase 4 开发计划 |
| [ECS_PHASE4_SUMMARY.md](ECS_PHASE4_SUMMARY.md) | Phase 4 完成总结 |
| [PHASE4_PROGRESS.md](PHASE4_PROGRESS.md) | Phase 4 进度更新 (2026-01-17) |

### 迁移文档

| 文档 | 描述 |
|------|------|
| [ECS_MIGRATION_COMPLETE.md](ECS_MIGRATION_COMPLETE.md) | ECS 框架迁移完成总结 |

### 测试文档

| 文档 | 描述 |
|------|------|
| [TESTING.md](TESTING.md) | 测试框架说明和测试指南 |
| [TESTS_CONVERSION_COMPLETE.md](TESTS_CONVERSION_COMPLETE.md) | 测试转换为 Catch2 完成 |
| [TESTS_MIGRATION_COMPLETE.md](TESTS_MIGRATION_COMPLETE.md) | 测试系统迁移完成 |

### 总体文档

| 文档 | 描述 |
|------|------|
| [ECS_FINAL_SUMMARY.md](ECS_FINAL_SUMMARY.md) | ECS 框架完整总结 |

## 🚀 快速导航

### 新手入门
1. 阅读 [ECS_USAGE.md](ECS_USAGE.md) 了解基本概念和使用方法
2. 查看 [ECS_FINAL_SUMMARY.md](ECS_FINAL_SUMMARY.md) 了解框架全貌

### 性能优化
1. [ECS_OPTIMIZATION_GUIDE.md](ECS_OPTIMIZATION_GUIDE.md) - 学习优化技巧
2. [ECS_CACHE_OPTIMIZATION.md](ECS_CACHE_OPTIMIZATION.md) - 了解缓存优化
3. [ECS_PERFORMANCE_REPORT.md](ECS_PERFORMANCE_REPORT.md) - 查看性能基准

### 功能扩展
1. [ECS_PHASE3_PLAN.md](ECS_PHASE3_PLAN.md) - 了解高级功能设计
2. [ECS_PHASE4_PLAN.md](ECS_PHASE4_PLAN.md) - 了解完善计划

### 当前状态
- [PHASE4_PROGRESS.md](PHASE4_PROGRESS.md) - 最新的开发进度和状态

## 📊 性能基准

| 操作 | 性能 | 说明 |
|------|------|------|
| 单组件遍历 (10000 实体) | ~0.13 ms | Phase 4 测试 |
| 批量创建 (10000 实体) | ~2.70 ms | Phase 4 测试 |
| 多线程并发 | 3.8x 加速 | 4 线程 |

## 🎯 框架特性

- ✅ **高性能** - SOA 内存布局，缓存友好
- ✅ **类型安全** - 模板元编程保证类型安全
- ✅ **易于使用** - 清晰的 API 设计
- ✅ **线程安全** - 读写锁策略
- ✅ **可扩展** - 支持动态组件和关系管理
- ✅ **可调试** - 内置性能分析和内存分析工具

## 📖 阅读建议

### 根据需求选择文档

**我想了解 ECS 的基本概念和使用方法**
→ [ECS_USAGE.md](ECS_USAGE.md)

**我想了解如何优化性能**
→ [ECS_OPTIMIZATION_GUIDE.md](ECS_OPTIMIZATION_GUIDE.md)

**我想了解最新的开发进度**
→ [PHASE4_PROGRESS.md](PHASE4_PROGRESS.md)

**我想了解框架的整体架构和功能**
→ [ECS_FINAL_SUMMARY.md](ECS_FINAL_SUMMARY.md)

**我想了解具体某个优化阶段的详情**
→ Phase 2: [ECS_PHASE2_SUMMARY.md](ECS_PHASE2_SUMMARY.md)
→ Phase 3: [ECS_PHASE3_SUMMARY.md](ECS_PHASE3_SUMMARY.md)
→ Phase 4: [ECS_PHASE4_SUMMARY.md](ECS_PHASE4_SUMMARY.md)

**我想了解测试相关信息**
→ [TESTING.md](TESTING.md) - 测试框架说明
→ [TESTS_CONVERSION_COMPLETE.md](TESTS_CONVERSION_COMPLETE.md) - 测试转换记录
→ [TESTS_MIGRATION_COMPLETE.md](TESTS_MIGRATION_COMPLETE.md) - 测试迁移记录

## 🔗 相关资源

- **源代码**: `/src/common/include/common/ecs/`
- **示例程序**: `/src/apps/example/`
- **单元测试**: `/src/apps/tests/`
- **主 README**: `/README.md`
- **文档整理**: `/DOC_SUMMARY.md`

---

**最后更新**: 2026-01-17
