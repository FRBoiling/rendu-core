//
// Created by 沸腾 on 2025/10/24.
//

#include "common/banner.h"
#include "common/git_revision.h"
#include "common/utils/string_format.h"

using namespace common;

void Banner::Show(char const* applicationName, void(*log)(char const* text), void(*logExtraInfo)())
{
    log(Utils::StringFormat("{} ({})", GitRevision::GetFullVersion(), applicationName).c_str());
    log(R"(<Ctrl-C> to stop.)" "\n");
    log(R"( ______                       __)");
    log(R"(/\__  _\       __          __/\ \__)");
    log(R"(\/_/\ \/ _ __ /\_\    ___ /\_\ \, _\  __  __)");
    log(R"(   \ \ \/\`'__\/\ \ /' _ `\/\ \ \ \/ /\ \/\ \)");
    log(R"(    \ \ \ \ \/ \ \ \/\ \/\ \ \ \ \ \_\ \ \_\ \)");
    log(R"(     \ \_\ \_\  \ \_\ \_\ \_\ \_\ \__\\/`____ \)");
    log(R"(      \/_/\/_/   \/_/\/_/\/_/\/_/\/__/ `/___/> \)");
    log(R"(                                 C O R E  /\___/)");
    log(R"(http://RenduCore.org                    \/__/)" "\n");

    if (logExtraInfo)
        logExtraInfo();
}

