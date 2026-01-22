//
// Created by 沸腾 on 2025/10/24.
//

#include "common/banner.h"
#include "common/git_revision.h"
#include "common/util/string.h"
#include <cstring>

BEGIN_NAMESPACE_COMMON

void Banner::Show(
    char const* applicationName,
    void (*log)(char const* text),
    void (*logExtraInfo)(char const* text))
{
    auto version = GitRevision::GetFullVersion();

    // 格式化并输出版本信息
    std::string headStr;
    try
    {
        headStr = str::format("{} ({})",
            version ? version : "[Unknown]",
            applicationName ? applicationName : "[Unknown]");
        log(headStr.c_str());
    }
    catch (...)
    {
        log("[Error] Failed to format string");
    }

    // 输出控制提示
    log(R"(<Ctrl-C> to stop.)");

    // 输出 ASCII 艺术 Logo
    log(R"(██████╗ ███████╗███╗   ██╗ ██████╗  ██╗   ██║)");
    log(R"(██╔══██╗██╔════╝████╗  ██║ ██╔══██╗ ██║   ██║)");
    log(R"(██████╔╝█████╗  ██╔██╗ ██║ ██║  ██║ ██║   ██║)");
    log(R"(██╔══██╗██╔══╝  ██║╚██╗██║ ██║  ██║ ██║   ██║)");
    log(R"(██║  ██║███████╗██║ ╚████║ ██████╔╝ ╚██████╔╝)");
    log(R"(╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝ ╚═════╝   ╚═════╝ )");
    log(R"(                            CORE /\___/          )");
    log(R"(https://github.com/FRBoiling/rendu-core   \__/)");

    // 记录额外信息
    logExtraInfo("Banner shown");
}

END_NAMESPACE_COMMON
