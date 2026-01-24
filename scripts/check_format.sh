#!/bin/bash

# 代码格式检查脚本
# 检查C++代码是否符合项目规范

echo "==================================================================="
echo "RenduCore 代码格式检查"
echo "==================================================================="
echo ""

# 需要检查的目录
DIRS=(
    "src/common"
)

# 总计统计
TOTAL_FILES=0
FORMATTED_FILES=0
NEEDS_FORMATTING=0

# 检查clang-format是否可用
if ! command -v clang-format &> /dev/null; then
    echo "❌ 错误: clang-format 未安装"
    echo ""
    echo "安装方法:"
    echo "  macOS: brew install clang-format"
    echo "  Ubuntu: sudo apt-get install clang-format"
    echo "  Fedora: sudo dnf install clang-format"
    echo ""
    exit 1
fi

echo "✅ clang-format 版本: $(clang-format --version | head -n1)"
echo ""

# 检查格式
for dir in "${DIRS[@]}"; do
    if [ ! -d "$dir" ]; then
        echo "⚠️  目录不存在: $dir"
        continue
    fi

    echo "-------------------------------------------------------------------"
    echo "检查目录: $dir"
    echo "-------------------------------------------------------------------"

    # 查找所有C++文件
    while IFS= read -r -d '' file; do
        ((TOTAL_FILES++))
        
        # 检查文件格式
        if clang-format --dry-run --Werror "$file" &> /dev/null; then
            echo "✅ $file"
            ((FORMATTED_FILES++))
        else
            echo "❌ $file (需要格式化)"
            ((NEEDS_FORMATTING++))
            
            # 显示建议的修改
            echo "   建议修改:"
            clang-format "$file" | diff -u "$file" - | head -20 || true
        fi
    done < <(find "$dir" -type \( -name "*.h" -o -name "*.cpp" \) -print0)

    echo ""
done

echo "==================================================================="
echo "检查结果汇总"
echo "==================================================================="
echo "总文件数: $TOTAL_FILES"
echo "已格式化: $FORMATTED_FILES"
echo "需要格式化: $NEEDS_FORMATTING"
echo ""

# 计算百分比
if [ $TOTAL_FILES -gt 0 ]; then
    PERCENT=$((FORMATTED_FILES * 100 / TOTAL_FILES))
    echo "格式合规率: $PERCENT%"
else
    echo "格式合规率: N/A (未找到文件)"
fi
echo ""

if [ $NEEDS_FORMATTING -eq 0 ]; then
    echo "🎉 所有文件格式检查通过!"
    exit 0
else
    echo "⚠️  发现 $NEEDS_FORMATTING 个文件需要格式化"
    echo ""
    echo "自动修复命令:"
    echo "  find src -name '*.h' -o -name '*.cpp' | xargs clang-format -i"
    exit 1
fi
