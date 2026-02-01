#!/bin/bash
# Rendu Core 性能基准测试运行脚本
# 阶段 17: 编译并运行 benchmark_suite（Catch2 BENCHMARK）

set -e

BUILD_DIR="${BUILD_DIR:-build}"
OUTPUT_FILE="${BENCHMARK_OUTPUT:-}"

echo "==================================================================="
echo "RenduCore 性能基准测试"
echo "==================================================================="
echo ""

if [[ ! -d "$BUILD_DIR" ]]; then
    echo "构建目录不存在: $BUILD_DIR"
    echo "请先配置并编译，例如:"
    echo "  cmake -B build -DCMAKE_BUILD_TYPE=Release -DRENDU_BUILD_TESTING=ON"
    echo "  cmake --build build --target benchmarks"
    exit 1
fi

echo "编译基准测试目标 (benchmarks)..."
cmake --build "$BUILD_DIR" --target benchmarks
echo ""

BENCHMARK_BIN="$BUILD_DIR/src/tests/benchmarks/benchmark_suite"
if [[ ! -x "$BENCHMARK_BIN" ]]; then
    echo "错误: 可执行文件不存在: $BENCHMARK_BIN"
    exit 1
fi

echo "运行基准测试..."
if [[ -n "$OUTPUT_FILE" ]]; then
    "$BENCHMARK_BIN" | tee "$OUTPUT_FILE"
    echo ""
    echo "输出已保存到: $OUTPUT_FILE"
else
    "$BENCHMARK_BIN"
fi

echo ""
echo "基准测试完成。"
