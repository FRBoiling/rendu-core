#pragma once

#include "common/config/config.h"
#include "common/util/error.h"
#include <string>
#include <memory>

BEGIN_NAMESPACE_COMMON
    namespace config
    {
        /**
         * @brief 配置加载器抽象基类
         */
        class Loader
        {
        public:
            virtual ~Loader() = default;

            /**
             * @brief 从文件加载配置
             * @param filepath 配置文件路径
             * @return Result<Config> 配置对象或错误信息
             */
            virtual Result<Config> load(const std::string& filepath) = 0;
        };
    } // namespace config
END_NAMESPACE_COMMON
