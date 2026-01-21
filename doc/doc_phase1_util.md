# 阶段 1: Common 层 - 基础工具 (util)

## 目标
实现通用工具函数和类型定义，为其他模块提供基础支撑。

---

## 任务清单

### 1. define.h
**文件路径**: `src/common/include/common/define.h`

**内容**:
- 平台检测宏
- 编译器检测宏
- 公共类型别名
- 导入/导出宏
- 断言宏
- 字节序宏
- 数学常量

---

### 2. string.h
**文件路径**: `src/common/include/common/util/string.h`

**内容**:
- 字符串分割
- 字符串连接
- 字符串修剪
- 大小写转换
- 字符串替换
- 字符串格式化（基于 fmt）
- 编码转换（UTF-8/GBK 等）

---

### 3. time.h
**文件路径**: `src/common/include/common/util/time.h`

**内容**:
- 时间戳获取（毫秒、微秒）
- 时间格式化
- 时间解析
- 高精度计时器
- 睡眠函数

---

### 4. error.h
**文件路径**: `src/common/include/common/util/error.h`

**内容**:
- 错误码枚举
- 错误信息映射
- 错误处理辅助函数
- 异常类定义

---

### 5. container.h
**文件路径**: `src/common/include/common/util/container.h`

**内容**:
- 容器遍历辅助
- 查找辅助函数
- 容器转换工具

---

## 验收标准

- [ ] 所有工具函数有单元测试
- [ ] 测试覆盖率 ≥ 80%
- [ ] 无编译警告
- [ ] 符合 C++20 标准
- [ ] 性能符合预期
- [ ] 头文件文档完整（Doxygen）

---

## 依赖
无（这是最底层的模块）
