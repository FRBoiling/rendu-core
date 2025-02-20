# ... 文件头保持不变 ...

# 增强版平台检测逻辑
function(detect_platform)
  # 架构位数检测（支持Universal Binary）
  if(CMAKE_OSX_ARCHITECTURES)
    message(STATUS "跨架构编译: ${CMAKE_OSX_ARCHITECTURES}")
  endif()
  set(RENDU_PLATFORM_BITS ${CMAKE_SIZEOF_VOID_P} PARENT_SCOPE)  # 直接存储字节数

  # 增强架构检测逻辑
  cmake_host_system_information(RESULT raw_arch QUERY OS_PLATFORM)
  string(TOLOWER "${raw_arch}" arch)

  # 架构类型映射表
  set(arch_map
      "x86_64;x86_64"   "amd64;x86_64"    "x64;x86_64"
      "arm64;arm64"     "aarch64;arm64"
      "armv7;armv7"     "armv7l;armv7"
      "riscv64;riscv"   "loongarch64;loongarch"
  )

  set(target_arch "unknown")
  foreach(mapping IN LISTS arch_map)
    list(GET mapping 0 pattern)
    list(GET mapping 1 mapped)
    if(arch MATCHES "${pattern}")
      set(target_arch "${mapped}")
      break()
    endif()
  endforeach()

  # 生成标准化架构名称
  string(TOUPPER "${target_arch}" RENDU_TARGET_ARCH)
  set(RENDU_TARGET_ARCH ${RENDU_TARGET_ARCH} PARENT_SCOPE)
  message(STATUS "标准化架构: ${RENDU_TARGET_ARCH}")

  # 平台配置加载（支持多级回退）
  find_package(PlatformSettings
      PATHS "${CMAKE_CURRENT_SOURCE_DIR}/platform/${CMAKE_SYSTEM_NAME}"
      NO_DEFAULT_PATH
  )
  if(NOT PlatformSettings_FOUND)
    message(WARNING "使用默认平台配置")
    include("${CMAKE_CURRENT_SOURCE_DIR}/platform/Default/settings.cmake")
  endif()
endfunction()

# 生成带版本信息的架构定义
target_compile_definitions(rendu-compile-option-interface INTERFACE
    $<$<BOOL:${RENDU_TARGET_ARCH}>:RENDU_ARCH_${RENDU_TARGET_ARCH}>
    $<$<BOOL:${RENDU_PLATFORM_BITS}>:RENDU_BITS_$<IF:$<EQUAL:${RENDU_PLATFORM_BITS},8>,64,32>>
)


