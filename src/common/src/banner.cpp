//
// Created by 沸腾 on 2025/10/24.
//

#include "common/banner.h"
#include "common/git_revision.h"
#include "common/utils/string_format.h"
#include <cstring>

BEGIN_NAMESPACE_COMMON

void Banner::Show(char const* applicationName, void(*log)(char const* text), void(*logExtraInfo)(char const* text))
{
    auto version = GitRevision::GetFullVersion();

    // 使用更安全的格式化方式
    std::string headStr;
    try {
        headStr = Utils::StringFormat("{} ({})", version ? version : "[Unknown]", 
                                     applicationName ? applicationName : "[Unknown]");
        log(headStr.c_str());
    } catch (...) {
        log("[Error] Failed to format string");
    }
    
    log(R"(<Ctrl-C> to stop.)");
    log(R"(██████╗ ███████╗███╗   ██╗ ██████╗  ██╗   ██╗)");
    log(R"(██╔══██╗██╔════╝████╗  ██║ ██╔══██╗ ██║   ██║)");
    log(R"(██████╔╝█████╗  ██╔██╗ ██║ ██║  ██║ ██║   ██║)");
    log(R"(██╔══██╗██╔══╝  ██║╚██╗██║ ██║  ██║ ██║   ██║)");
    log(R"(██║  ██║███████╗██║ ╚████║ ██████╔╝ ╚██████╔╝)");
    log(R"(╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝ ╚═════╝   ╚═════╝ )");
    log(R"(                            CORE /\___/          )");
    log(R"(https://github.com/FRBoiling/rendu-core   \__/)");


    logExtraInfo("Banner shown");
}

END_NAMESPACE_COMMON
