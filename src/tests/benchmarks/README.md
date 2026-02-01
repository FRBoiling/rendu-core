# Rendu Core 性能基准测试

阶段 17 性能优化相关基准测试，使用 **Catch2** 的 BENCHMARK 功能。

## 编译

```bash
# 推荐 Release 以得到有参考价值的数值
cmake -B build -DCMAKE_BUILD_TYPE=Release -DRENDU_BUILD_TESTING=ON
cmake --build build --target benchmarks
```

目标名 `benchmarks` 会构建可执行文件 `benchmark_suite`。

## 运行

```bash
./build/src/tests/benchmarks/benchmark_suite
```

或使用项目根目录下的脚本（可指定构建目录与输出文件）：

```bash
BUILD_DIR=build ./scripts/run_benchmarks.sh
# 保存输出: BENCHMARK_OUTPUT=benchmark_results.txt ./scripts/run_benchmarks.sh
```

## 基准项说明

| 类别     | 基准名称                               | 说明 |
|----------|----------------------------------------|------|
| 序列化   | protobuf_serialize_* / protobuf_serialize_with_reserve_* | 模拟 protobuf 序列化（64B/1KB/4KB） |
| 零拷贝   | buffer_view_create_* / string_copy_*   | BufferView 与 string 拷贝对比（64B/1KB） |
| 内存池   | message_pool_allocate / new_delete_message | MessagePool 与 new/delete 对比 |
| 线程池   | work_stealing_thread_pool / work_stealing_thread_pool_stealing | 工作窃取线程池提交与窃取场景 |
| 日志     | logging_sync / logging_async           | 同步默认 logger 与 AsyncBufferedLogger |

详细说明与性能目标见 `doc/doc_phase17_performance.md` 第三节。
