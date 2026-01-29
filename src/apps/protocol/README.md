# 协议定义

本目录包含 server 和 client 共享的 Protobuf 协议定义。

## 文件说明

- **messages.proto** - 服务器与客户端通信的消息定义

## 协议结构

### 客户端消息 (ClientMessage)

客户端发送给服务器的消息类型：

1. **LoginRequest** - 登录请求
   ```protobuf
   message LoginRequest {
     string username = 1;
   }
   ```

2. **ChatMessage** - 聊天消息
   ```protobuf
   message ChatMessage {
     string content = 1;
   }
   ```

3. **LogoutRequest** - 登出请求
   ```protobuf
   message LogoutRequest {}
   ```

### 服务器消息 (ServerMessage)

服务器发送给客户端的消息类型：

1. **LoginResponse** - 登录响应
   ```protobuf
   message LoginResponse {
     bool success = 1;
     int32 user_id = 2;
     string message = 3;
   }
   ```

2. **ChatBroadcast** - 聊天广播
   ```protobuf
   message ChatBroadcast {
     int32 user_id = 1;
     string username = 2;
     string content = 3;
     int64 timestamp = 4;
   }
   ```

3. **UserList** - 用户列表
   ```protobuf
   message UserList {
     repeated UserInfo users = 1;
   }

   message UserInfo {
     int32 user_id = 1;
     string username = 2;
   }
   ```

4. **ErrorMessage** - 错误消息
   ```protobuf
   message ErrorMessage {
     int32 code = 1;
     string message = 2;
   }
   ```

## 编译生成

协议文件会在编译时自动生成 C++ 代码：

```bash
cd build
cmake ..
make
```

生成的文件位于：
- `proto_gen/messages.pb.h` - 头文件
- `proto_gen/messages.pb.cc` - 源文件

## 使用方法

### 在 server 中使用

```cpp
#include <messages.pb.h>

// 构造登录响应
protocol::LoginResponse response;
response.set_success(true);
response.set_user_id(user_id);
response.set_message("Welcome!");

// 序列化
std::string data;
response.SerializeToString(&data);
```

### 在 client 中使用

```cpp
#include <messages.pb.h>

// 构造登录请求
protocol::LoginRequest request;
request.set_username("alice");

// 反序列化
protocol::LoginResponse response;
response.ParseFromString(data);
if (response.success()) {
    std::cout << "User ID: " << response.user_id() << std::endl;
}
```

## 协议版本

当前协议版本: v1.0

## 修改协议

如果需要修改协议，请：

1. 编辑 `messages.proto` 文件
2. 重新编译项目
3. 确保 server 和 client 使用相同的协议版本

## 注意事项

- ⚠️ 协议变更需要 server 和 client 同步更新
- ⚠️ 字段编号一旦使用，不应再修改
- ⚠️ 建议保留字段以保持向后兼容性
- ⚠️ 使用 `optional` 标记可选字段
