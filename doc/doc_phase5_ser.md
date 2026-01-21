# 阶段 5: Common 层 - 序列化 (ser)

## 目标
提供统一的序列化接口，实现 Protobuf 支持。

---

## 任务清单

### 1. serializer.h
**文件路径**: `src/common/include/common/ser/serializer.h`

**职责**:
- 序列化接口抽象
- 定义统一的 API
- 支持多种序列化格式

**核心接口**:
```cpp
class ISerializer {
public:
    virtual ~ISerializer() = default;

    // 序列化
    virtual bool Serialize(const google::protobuf::Message& msg,
                          std::string& output) = 0;

    // 反序列化
    virtual bool Deserialize(const std::string& input,
                            google::protobuf::Message& msg) = 0;

    // 获取格式名称
    virtual std::string GetFormatName() const = 0;
};
```

---

### 2. protobuf_ser.h
**文件路径**: `src/common/include/common/ser/protobuf_ser.h`

**职责**:
- Protobuf 二进制序列化实现
- 高效、紧凑的二进制格式

**核心接口**:
```cpp
class ProtobufSerializer : public ISerializer {
public:
    ProtobufSerializer();

    bool Serialize(const google::protobuf::Message& msg,
                  std::string& output) override;

    bool Deserialize(const std::string& input,
                    google::protobuf::Message& msg) override;

    std::string GetFormatName() const override;
};
```

---

### 3. json_ser.h
**文件路径**: `src/common/include/common/ser/json_ser.h`

**职责**:
- JSON 序列化实现
- 可读性好，便于调试

**核心接口**:
```cpp
class JsonSerializer : public ISerializer {
public:
    JsonSerializer();

    // 设置是否美化输出
    void SetPretty(bool pretty);

    bool Serialize(const google::protobuf::Message& msg,
                  std::string& output) override;

    bool Deserialize(const std::string& input,
                    google::protobuf::Message& msg) override;

    std::string GetFormatName() const override;
};
```

---

## 设计要点

### 1. 零拷贝优化
- 序列化直接写入目标缓冲区
- 反序列化使用 string_view

### 2. 错误处理
- 统一的错误返回
- 详细的错误信息

### 3. 性能优化
- 预分配内存
- 避免不必要的拷贝

### 4. 扩展性
- 接口抽象支持其他格式
- 工厂模式创建序列化器

---

## 验收标准

- [ ] Protobuf 序列化/反序列化正确
- [ ] JSON 序列化/反序列化正确
- [ ] 性能满足预期
- [ ] 大数据量处理正常
- [ ] 单元测试覆盖

---

## 依赖
- 阶段 1 (util) - 基础工具
- 3rdparty::protobuf - Protocol Buffers
