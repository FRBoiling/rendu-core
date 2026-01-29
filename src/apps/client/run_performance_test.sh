#!/bin/bash

# 服务器性能测试脚本
# 用法: ./run_performance_test.sh [--clients N] [--duration N] [--interval N] [--port N]

set -e

# 默认参数
NUM_CLIENTS=100
DURATION=60
INTERVAL=1000
PORT=8080
SERVER_BINARY="./build/bin/rendu_server"
TEST_BINARY="./build/bin/test/performance_test"

# 解析参数
while [[ $# -gt 0 ]]; do
    case $1 in
        --clients)
            NUM_CLIENTS="$2"
            shift 2
            ;;
        --duration)
            DURATION="$2"
            shift 2
            ;;
        --interval)
            INTERVAL="$2"
            shift 2
            ;;
        --port)
            PORT="$2"
            shift 2
            ;;
        --help|-h)
            echo "用法: $0 [选项]"
            echo ""
            echo "选项:"
            echo "  --clients N       客户端数量 (默认: 100)"
            echo "  --duration N     测试时长，单位秒 (默认: 60)"
            echo "  --interval N     聊天间隔，单位毫秒 (默认: 1000)"
            echo "  --port N         服务器端口 (默认: 8080)"
            echo "  --help, -h       显示帮助信息"
            exit 0
            ;;
        *)
            echo "未知参数: $1"
            echo "使用 --help 查看帮助"
            exit 1
            ;;
    esac
done

echo "========================================="
echo "     服务器性能测试脚本"
echo "========================================="
echo ""
echo "测试配置:"
echo "  客户端数量: $NUM_CLIENTS"
echo "  测试时长: $DURATION 秒"
echo "  聊天间隔: $INTERVAL 毫秒"
echo "  服务器端口: $PORT"
echo ""
echo "========================================="
echo ""

# 检查可执行文件是否存在
if [ ! -f "$SERVER_BINARY" ]; then
    echo "错误: 服务器可执行文件不存在: $SERVER_BINARY"
    echo "请先编译项目"
    exit 1
fi

if [ ! -f "$TEST_BINARY" ]; then
    echo "错误: 测试可执行文件不存在: $TEST_BINARY"
    echo "请先编译项目"
    exit 1
fi

# 检查端口是否被占用
if lsof -Pi :$PORT -sTCP:LISTEN -t >/dev/null 2>&1 ; then
    echo "警告: 端口 $PORT 已被占用"
    echo "请先停止正在运行的服务器"
    exit 1
fi

# 创建日志目录
mkdir -p logs
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
SERVER_LOG="logs/server_${TIMESTAMP}.log"
TEST_LOG="logs/test_${TIMESTAMP}.log"

echo "[步骤 1] 启动服务器..."
$SERVER_BINARY > "$SERVER_LOG" 2>&1 &
SERVER_PID=$!
echo "服务器 PID: $SERVER_PID"
echo "服务器日志: $SERVER_LOG"

# 等待服务器启动
echo "等待服务器启动..."
sleep 3

# 检查服务器是否正常运行
if ! ps -p $SERVER_PID > /dev/null; then
    echo "错误: 服务器启动失败，请查看日志: $SERVER_LOG"
    exit 1
fi

echo "服务器已启动"
echo ""

# 运行性能测试
echo "[步骤 2] 运行性能测试..."
$TEST_BINARY \
    --clients $NUM_CLIENTS \
    --duration $DURATION \
    --interval $INTERVAL \
    --port $PORT \
    2>&1 | tee "$TEST_LOG"
TEST_EXIT_CODE=${PIPESTATUS[0]}

echo ""

# 停止服务器
echo "[步骤 3] 停止服务器..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null || true
echo "服务器已停止"

echo ""
echo "========================================="
echo "         测试完成"
echo "========================================="
echo ""
echo "日志文件:"
echo "  服务器日志: $SERVER_LOG"
echo "  测试日志: $TEST_LOG"
echo ""

# 返回测试退出码
exit $TEST_EXIT_CODE
