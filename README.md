# RenduCore - Modern C++ Project

[![CMake](https://img.shields.io/badge/CMake-3.25+-blue.svg)](https://cmake.org)
[![C++ Standard](https://img.shields.io/badge/C++-20-orange.svg)](https://isocpp.org/)

## 项目概述

RenduCore是基于现代C++（C++20）构建的项目模板，使用CMake作为构建系统。项目模板旨在简化构建配置流程，使开发者能够专注于核心代码实现。

**项目模板**：[rendu-cxx-template](https://github.com/FRBoiling/rendu-cxx-template)

## 项目结构
rendu-cxx-template/
├── cmake/                 # CMake模块系统
│   ├── Compiler/          # 编译配置
│   ├── Packaging/         # 打包配置
│   ├── Platform/          # 平台配置
│   ├── Project/           # 核心构建逻辑
│   ├── Toolchain/         # 工具链配置
│   ├── Utils/             # CMake工具函数
│   └── Config.cmake.in    # 项目配置模板
│
├── libs/                   # 核心代码库
│   ├── core/               # ECS核心实现
│   │   └── CMakeLists.txt      # 子项目配置
│   │
│   └── common/            # 基础功能模块
│   │   └── CMakeLists.txt      # 子项目配置
│   └── CMakeLists.txt      # 子项目配置
│
├── apps/                   # 应用程序目录（新增）
│   ├── launcher/               # 应用程序启动器（新增）
│   │   └── CMakeLists.txt      # 子项目配置
│   └── demo/               # 功能演示程序
│   │   └── CMakeLists.txt      # 子项目配置
│   └── CMakeLists.txt      # 子项目配置
│
├── tests/                  # 测试套件（结构调整）
│   ├── integration/        # 集成测试（新增）
│   │   └── core/            # 集成测试
│   ├── benchmark/          # 性能测试（新增）
│   │   └── core/            # 性能测试  
│   └── unit/               # 单元测试（原结构升级）
│       ├── core/           # 对应libs/core模块
│       └── common/         # 对应libs/common模块
│
├── third_party/           # 第三方依赖（结构调整）
│
├── docs/                   # 文档系统（新增）
│   ├── architecture/       # 架构设计文档
│   ├── api/                # API参考手册
│   └── tutorials/          # 使用教程
│
└── tools/                  # 开发工具（功能扩展）
    ├── scripts/            # 构建脚本
    ├── codegen/            # 代码生成工具
    └── analysis/           # 静态分析配置

## ✨ 功能特性

- 开箱即用的CMake项目配置
- 自动化依赖管理
- 单元测试集成（需补充测试框架）
- 跨平台支持（Linux/macOS/Windows）
- 持续集成预配置（需补充CI配置）

## 🚀 快速开始

### 前置要求
- CMake ≥ 3.25
- C++20兼容编译器（GCC 12+ / Clang 15+ / MSVC 2022+）

### 构建指南
```bash
# 克隆项目
git clone https://github.com/FRBoiling/rendu-cxx-template.git
cd rendu-cxx-template

# 配置项目
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 编译项目
cmake --build build --config Release

# 运行测试（需先配置测试）
cd build && ctest -V






rendu-terminal/
├── CMakeLists.txt
├── CPackConfig.cmake
├── LICENSE
├── README.md
├── macos/
│   └── Info.plist
├── resources/
│   ├── CMakeLists.txt
│   ├── resources.qrc
│   ├── fonts/
│   │   └── DejaVuSansMono.ttf
│   ├── icons/
│   │   ├── rendu-terminal.png
│   │   ├── tab-new.png
│   │   ├── tab-close.png
│   │   └── settings.png
│   └── linux/
│       └── rendu-terminal.desktop
└── src/
    ├── CMakeLists.txt
    ├── main.cpp
    ├── mainwindow.h
    ├── mainwindow.cpp
    ├── terminalwidget.h
    ├── terminalwidget.cpp
    ├── settingsdialog.h
    ├── settingsdialog.cpp
    ├── aboutdialog.h
    └── aboutdialog.cpp