//
// Created by 沸腾 on 2026/2/2.
//

#ifndef RENDUCORE_JSON_LOADER_H
#define RENDUCORE_JSON_LOADER_H

#include "loader.h"


BEGIN_NAMESPACE_COMMON
    namespace config
    {
        /**
         * @brief JSON 格式配置加载器
         *
         * 使用 simdjson 高性能解析器加载 JSON 配置文件
         */
        class JsonLoader : public Loader
        {
        public:
            JsonLoader() = default;
            ~JsonLoader() override = default;

            Result<Config> load(const std::string& filepath) override;
        };
    }

END_NAMESPACE_COMMON

#endif //RENDUCORE_JSON_LOADER_H
