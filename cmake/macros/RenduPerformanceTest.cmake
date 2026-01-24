# ====================================================================
# 模块: RenduPerformanceTest
# 描述: 性能测试基准脚本，用于验证优化效果
# ====================================================================

# ====================================================================
# 函数: rendu_test_collect_dirs_performance
# 描述: 测试 RenduCollectDirectories 的性能
#
# 参数:
#   test_dir     - 测试目录路径
#   dir_count    - 要创建的测试目录数量
#   exclude_count - 排除目录数量
#
# 用法示例:
#   rendu_test_collect_dirs_performance(
#       "${CMAKE_BINARY_DIR}/perf_test"
#       1000
#       10
#   )
# ====================================================================
function(rendu_test_collect_dirs_performance test_dir dir_count exclude_count)
    # 清理旧的测试目录
    if (EXISTS "${test_dir}")
        file(REMOVE_RECURSE "${test_dir}")
    endif ()
    file(MAKE_DIRECTORY "${test_dir}")

    message(STATUS "开始 RenduCollectDirectories 性能测试...")
    message(STATUS "测试目录: ${test_dir}")
    message(STATUS "目录数量: ${dir_count}")
    message(STATUS "排除数量: ${exclude_count}")

    # 创建测试目录
    string(TIMESTAMP create_start "%s%N")
    foreach(i RANGE 1 ${dir_count})
        file(MAKE_DIRECTORY "${test_dir}/dir${i}")
    endforeach ()
    string(TIMESTAMP create_end "%s%N")
    math(EXPR create_time "(${create_end}-${create_start})/1000000" OUTPUT_FORMAT DECIMAL)
    message(STATUS "创建 ${dir_count} 个目录耗时: ${create_time}ms")

    # 准备排除目录列表
    set(exclude_dirs "")
    foreach(i RANGE 1 ${exclude_count})
        list(APPEND exclude_dirs "${test_dir}/dir${i}")
    endforeach ()

    # 测试无缓存场景
    message(STATUS "")
    message(STATUS "=== 测试 1: 无缓存场景 ===")
    string(TIMESTAMP start1 "%s%N")
    rendu_collect_include_directories(result "${test_dir}")
    string(TIMESTAMP end1 "%s%N")
    math(EXPR duration1 "(${end1}-${start1})/1000000" OUTPUT_FORMAT DECIMAL)
    list(LENGTH result result_count1)
    message(STATUS "耗时: ${duration1}ms")
    message(STATUS "结果数量: ${result_count1}")

    # 测试有缓存场景
    message(STATUS "")
    message(STATUS "=== 测试 2: 有缓存场景 ===")
    string(TIMESTAMP start2 "%s%N")
    rendu_collect_include_directories(result "${test_dir}")
    string(TIMESTAMP end2 "%s%N")
    math(EXPR duration2 "(${end2}-${start2})/1000000" OUTPUT_FORMAT DECIMAL)
    list(LENGTH result result_count2)
    message(STATUS "耗时: ${duration2}ms")
    message(STATUS "结果数量: ${result_count2}")
    message(STATUS "缓存加速比: ${duration1}/${duration2} = ")
    if (NOT duration2 EQUAL 0)
        math(EXPR speedup "${duration1}*100/${duration2}" OUTPUT_FORMAT DECIMAL)
        math(EXPR speedup_ratio "${speedup}/100.0" OUTPUT_FORMAT DECIMAL)
        message(STATUS "缓存加速比: ${speedup_ratio}x")
    endif ()

    # 测试带排除目录场景
    message(STATUS "")
    message(STATUS "=== 测试 3: 带排除目录场景 (${exclude_count} 个排除) ===")
    string(TIMESTAMP start3 "%s%N")
    rendu_collect_include_directories(
        result
        "${test_dir}"
        EXCLUDE_DIRS ${exclude_dirs}
    )
    string(TIMESTAMP end3 "%s%N")
    math(EXPR duration3 "(${end3}-${start3})/1000000" OUTPUT_FORMAT DECIMAL)
    list(LENGTH result result_count3)
    math(EXPR expected_count "${dir_count}-${exclude_count}")
    message(STATUS "耗时: ${duration3}ms")
    message(STATUS "结果数量: ${result_count3} (预期: ${expected_count})")

    # 测试带正则表达式场景
    message(STATUS "")
    message(STATUS "=== 测试 4: 带正则表达式场景 ===")
    string(TIMESTAMP start4 "%s%N")
    rendu_collect_include_directories(
        result
        "${test_dir}"
        EXCLUDE_REGEX ".*/dir[0-9]{1,2}$"
    )
    string(TIMESTAMP end4 "%s%N")
    math(EXPR duration4 "(${end4}-${start4})/1000000" OUTPUT_FORMAT DECIMAL)
    list(LENGTH result result_count4)
    message(STATUS "耗时: ${duration4}ms")
    message(STATUS "结果数量: ${result_count4}")

    # 清理测试目录
    message(STATUS "")
    message(STATUS "清理测试目录...")
    file(REMOVE_RECURSE "${test_dir}")
    message(STATUS "测试完成")
endfunction()

# ====================================================================
# 函数: rendu_test_logging_performance
# 描述: 测试 RenduLogging 的性能
#
# 参数:
#   log_count - 要记录的日志数量
#   log_level - 日志级别
#
# 用法示例:
#   rendu_test_logging_performance(1000 INFO)
# ====================================================================
function(rendu_test_logging_performance log_count log_level)
    message(STATUS "")
    message(STATUS "开始 RenduLogging 性能测试...")
    message(STATUS "日志数量: ${log_count}")
    message(STATUS "日志级别: ${log_level}")

    # 设置日志级别
    rendu_set_log_level(${log_level})

    # 测试 1: 时间戳生成性能
    message(STATUS "")
    message(STATUS "=== 测试 1: 时间戳生成性能 ===")
    string(TIMESTAMP ts_start "%s%N")
    foreach(i RANGE 1 ${log_count})
        string(TIMESTAMP test_ts "%Y-%m-%d %H:%M:%S")
    endforeach ()
    string(TIMESTAMP ts_end "%s%N")
    math(EXPR ts_duration "(${ts_end}-${ts_start})/1000000" OUTPUT_FORMAT DECIMAL)
    math(EXPR ts_avg "${ts_duration}.0/${log_count}" OUTPUT_FORMAT DECIMAL)
    message(STATUS "总耗时: ${ts_duration}ms")
    message(STATUS "平均耗时: ${ts_avg}ms/条")

    # 测试 2: 日志输出性能 (控制台)
    message(STATUS "")
    message(STATUS "=== 测试 2: 日志输出性能 (控制台) ===")
    string(TIMESTAMP log_start "%s%N")
    foreach(i RANGE 1 ${log_count})
        rendu_log_info("测试日志消息 ${i} - 这是一条用于性能测试的日志消息")
    endforeach ()
    string(TIMESTAMP log_end "%s%N")
    math(EXPR log_duration "(${log_end}-${log_start})/1000000" OUTPUT_FORMAT DECIMAL)
    math(EXPR log_avg "${log_duration}.0/${log_count}" OUTPUT_FORMAT DECIMAL)
    message(STATUS "总耗时: ${log_duration}ms")
    message(STATUS "平均耗时: ${log_avg}ms/条")
    message(STATUS "吞吐量: ")
    if (NOT log_duration EQUAL 0)
        math(EXPR throughput "${log_count}*1000/${log_duration}" OUTPUT_FORMAT DECIMAL)
        message(STATUS "${throughput} 条/秒")
    endif ()

    # 测试 3: 日志输出性能 (文件 I/O)
    message(STATUS "")
    message(STATUS "=== 测试 3: 日志输出性能 (文件 I/O) ===")
    # 强制刷新缓冲区
    rendu_log_flush()
    string(TIMESTAMP file_start "%s%N")
    foreach(i RANGE 1 ${log_count})
        rendu_log_info("测试日志消息 ${i} - 这是一条用于性能测试的日志消息")
    endforeach ()
    rendu_log_flush()
    string(TIMESTAMP file_end "%s%N")
    math(EXPR file_duration "(${file_end}-${file_start})/1000000" OUTPUT_FORMAT DECIMAL)
    math(EXPR file_avg "${file_duration}.0/${log_count}" OUTPUT_FORMAT DECIMAL)
    message(STATUS "总耗时: ${file_duration}ms")
    message(STATUS "平均耗时: ${file_avg}ms/条")

    # 测试 4: 不同级别性能对比
    message(STATUS "")
    message(STATUS "=== 测试 4: 不同级别性能对比 ===")
    foreach(level DEBUG INFO WARN ERROR)
        rendu_set_log_level(${level})
        string(TIMESTAMP level_start "%s%N")
        foreach(i RANGE 1 100)
            rendu_log_info("测试日志 ${i}")
        endforeach ()
        string(TIMESTAMP level_end "%s%N")
        math(EXPR level_duration "(${level_end}-${level_start})/1000000" OUTPUT_FORMAT DECIMAL)
        message(STATUS "${level}: ${level_duration}ms (100 条)")
    endforeach ()

    # 测试 5: 缓冲区大小影响
    message(STATUS "")
    message(STATUS "=== 测试 5: 缓冲区大小影响 ===")
    set(original_buffer_size ${RENDU_LOG_BUFFER_MAX})
    foreach(buffer_size 1 10 50 100)
        rendu_set_log_buffer_size(${buffer_size})
        string(TIMESTAMP buffer_start "%s%N")
        foreach(i RANGE 1 ${log_count})
            rendu_log_info("测试日志消息 ${i}")
        endforeach ()
        rendu_log_flush()
        string(TIMESTAMP buffer_end "%s%N")
        math(EXPR buffer_duration "(${buffer_end}-${buffer_start})/1000000" OUTPUT_FORMAT DECIMAL)
        message(STATUS "缓冲区大小 ${buffer_size}: ${buffer_duration}ms")
    endforeach ()
    rendu_set_log_buffer_size(${original_buffer_size})

    message(STATUS "")
    message(STATUS "测试完成")
endfunction()

# ====================================================================
# 函数: rendu_run_all_performance_tests
# 描述: 运行所有性能测试
#
# 用法示例:
#   rendu_run_all_performance_tests()
# ====================================================================
function(rendu_run_all_performance_tests)
    message(STATUS "")
    message(STATUS "====================================================================")
    message(STATUS "                 Rendu CMake 性能测试套件")
    message(STATUS "====================================================================")
    message(STATUS "")

    # RenduCollectDirectories 测试
    message(STATUS ">>> 测试模块: RenduCollectDirectories")
    rendu_test_collect_dirs_performance("${CMAKE_BINARY_DIR}/perf_test_dirs" 1000 10)

    # RenduLogging 测试
    message(STATUS "")
    message(STATUS ">>> 测试模块: RenduLogging")
    rendu_test_logging_performance(1000 INFO)

    message(STATUS "")
    message(STATUS "====================================================================")
    message(STATUS "                     所有测试完成")
    message(STATUS "====================================================================")
endfunction()

# ====================================================================
# 使用说明
# ====================================================================
# 在 CMakeLists.txt 中添加:
#
#   include(${CMAKE_SOURCE_DIR}/cmake/macros/RenduPerformanceTest.cmake)
#
# 运行测试:
#   cmake -DRENDU_RUN_PERF_TEST=ON ..
#
# 或在 CMakeLists.txt 中调用:
#   rendu_run_all_performance_tests()
# ====================================================================
