#**********************************
#  Created by boil on 2022/8/14.
#**********************************

# 输出关于所选核心和构建类型的通用信息
message("")
message("* 版本信息  : ${rev_hash} ${rev_date} (${rev_branch} branch)")
if(UNIX)
  message("* 构建类型  : ${CMAKE_BUILD_TYPE}")
endif()
message("")

# 输出安装目录和位置信息
message("* 安装 core 到目录        : ${CMAKE_INSTALL_PREFIX}")
if(COPY_CONF)
  if(UNIX)
    message("* 安装 configs 到目录     : ${CONF_DIR}")
  else()
    message("* 安装 configs 到目录     : ${CMAKE_INSTALL_PREFIX}")
  endif()
endif()

message("")

# Show infomation about the options selected during configuration

if(CORES)
  message("* 构建 core/exe/runner     : Yes (default)")
else() 
  message("* 构建 core/exe/runner     : No")
endif()

if(SCRIPTS AND (NOT SCRIPTS STREQUAL "none"))
  message("* 构建 支持脚本           : Yes (${SCRIPTS})")
else()
  message("* 构建 支持脚本           : No")
endif()

if(TOOLS)
  message("* 构建 tool1/tool2 等工具  : Yes (default)")
else()
  message("* 构建 tool1/tool2 等工具  : No")
endif()

if(BUILD_TESTING)
  message("* 构建 unit 测试          : Yes")
else()
  message("* 构建 unit 测试          : No (default)")
endif()

if(USE_COREPCH)
  message("* 构建 core w/PCH         : Yes (default)")
else()
  message("* 构建 core w/PCH         : No")
endif()

if(USE_SCRIPTPCH)
  message("* 使用 scripts w/PCH      : Yes (default)")
else()
  message("* 使用 scripts w/PCH      : No")
endif()

if(WITH_WARNINGS)
  message("* 显示 all warnings         : Yes")
else()
  message("* 显示 compile-warnings     : No  (default)")
endif()

if(WITH_COREDEBUG)
  message("")
  message(" *** WITH_COREDEBUG - WARNING!")
  message(" *** 已经启用了额外的 core debug 日志!")
  message(" *** 这个设置不能帮助获得更好的崩溃日志!")
  message(" *** 如果您正在搜索更好的崩溃日志，请使用-DCMAKE_BUILD_TYPE=RelWithDebInfo")
  message(" *** DO NOT ENABLE IT UNLESS YOU KNOW WHAT YOU'RE DOING!")
  message("* 使用 coreside 调试        : Yes")
  add_definitions(-DRENDU_DEBUG)
else()
  message("* 使用 coreside 调试        : No  (default)")
endif()

if(NOT WITH_SOURCE_TREE STREQUAL "no")
  message("* 显示 源代码树               : Yes (${WITH_SOURCE_TREE})")
else()
  message("* 显示 源代码树               : No")
endif()

if(WITHOUT_GIT)
  message("* 使用 GIT revision hash  : No")
  message("")
  message(" *** WITHOUT_GIT - WARNING!")
  message(" *** 通过开启 WITHOUT_GIT 选项，放弃了所有支持的功能。所有向核心开发人员发送的支持或帮助请求 将被拒绝。")
  message(" *** 这是由于我们将无法检测您正在以适当的方式使用的代码库的什么修订。")
  message(" *** 我们提醒你，你需要使用仓库的代码库和一个支持版本的git来运行revision-hash，如果需要的话，可以请求支持。")
else()
  message("* 使用 GIT revision hash  : Yes (default)")
endif()

if(NOJEM)
  message("")
  message(" *** NOJEM - WARNING!")
  message(" *** jemalloc 链接已被禁用！")
  message(" *** 请注意这只针对 DEBUGGING WITH VALGRIND !")
  message(" *** DO NOT DISABLE IT UNLESS YOU KNOW WHAT YOU'RE DOING !")
endif()

if(HELGRIND)
  message("")
  message(" *** HELGRIND - WARNING!")
  message(" *** 如果您得到构建错误，请在VALGRIND_INCLUDE_DIR选项中指定valgrind include目录")
  message(" *** 请注意这只针对 DEBUGGING WITH HELGRIND !")
  add_definitions(-DHELGRIND)
endif()

if(ASAN)
  message("")
  message(" *** ASAN - WARNING!")
  message(" *** 请注意这只针对 DEBUGGING WITH HELGRIND !")
  add_definitions(-DASAN)
endif()

if(MSAN)
  message("")
  message(" *** MSAN - WARNING!")
  message(" *** 请注意这只针对 DEBUGGING WITH MEMORY SANITIZER !")
  add_definitions(-DMSAN)
endif()

if(UBSAN)
  message("")
  message(" *** UBSAN - WARNING!")
  message(" *** 请注意这只针对 DEBUGGING WITH UNDEFINED BEHAVIOR SANITIZER !")
  add_definitions(-DUBSAN)
endif()

if(TSAN)
  message("")
  message(" *** TSAN - WARNING!")
  message(" *** 请注意这只针对 DEBUGGING WITH THREAD SANITIZER !")
  add_definitions(-DTSAN -DNO_BUFFERPOOL)
endif()

if(PERFORMANCE_PROFILING)
  message("")
  message(" *** PERFORMANCE_PROFILING - WARNING!")
  message(" *** 请注意这只针对 PERFORMANCE PROFILING! 启用此配置时没有报告任何问题!")
  add_definitions(-DPERFORMANCE_PROFILING)
endif()

if(WITHOUT_METRICS)
  message("")
  message(" *** WITHOUT_METRICS - WARNING!")
  message(" *** 请注意，这将禁用所有指标输出(例如， InfluxDB 和 Grafana)")
  add_definitions(-DWITHOUT_METRICS)
elseif (WITH_DETAILED_METRICS)
  message("")
  message(" *** WITH_DETAILED_METRICS - WARNING!")
  message(" *** 请注意，这将启用所有指标输出(例如， InfluxDB 和 Grafana)")
  add_definitions(-DWITH_DETAILED_METRICS)
endif()

if(WITH_STRICT_DATABASE_TYPE_CHECKS)
  message("")
  message(" *** WITH_STRICT_DATABASE_TYPE_CHECKS - WARNING!")
  message(" *** 从Field类中验证数据库Get***()函数的使用无效调用将导致返回值0")
  message(" *** NOT COMPATIBLE WITH MARIADB!")
  add_definitions(-DRENDU_STRICT_DATABASE_TYPE_CHECKS)
endif()

if(WITH_BOOST_STACKTRACE)
  if (BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE)
    add_definitions(-DBOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE="${BOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE}")
  endif()
endif()

if(BUILD_SHARED_LIBS)
  message("")
  message(" *** WITH_DYNAMIC_LINKING - INFO!")
  message(" *** 链接到SHARED库")
  message(" *** 请注意，这是一个实验性的功能!")
  if(WITH_DYNAMIC_LINKING_FORCED)
    message("")
    message(" *** 动态链接是通过一个动态脚本模块强制的!")
  endif()
  add_definitions(-DRENDU_API_USE_DYNAMIC_LINKING)

  WarnAboutSpacesInBuildPath()
endif()

message("")
