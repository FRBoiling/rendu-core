//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_GIT_REVISION_H
#define RENDU_GIT_REVISION_H

#include "common.h"
#include "define.h"

BEGIN_NAMESPACE_COMMON
    namespace GitRevision
    {
        RC_COMMON_API char const* GetHash();
        RC_COMMON_API char const* GetDate();
        RC_COMMON_API char const* GetBranch();
        RC_COMMON_API char const* GetCMakeCommand();
        RC_COMMON_API char const* GetCMakeVersion();
        RC_COMMON_API char const* GetHostOSVersion();
        RC_COMMON_API char const* GetBuildDirectory();
        RC_COMMON_API char const* GetSourceDirectory();
        RC_COMMON_API char const* GetMySQLExecutable();
        RC_COMMON_API char const* GetFullDatabase();
        RC_COMMON_API char const* GetHotfixesDatabase();
        RC_COMMON_API char const* GetFullVersion();
        RC_COMMON_API char const* GetCompanyNameStr();
        RC_COMMON_API char const* GetLegalCopyrightStr();
        RC_COMMON_API char const* GetFileVersionStr();
        RC_COMMON_API char const* GetProductVersionStr();
    }

END_NAMESPACE_COMMON

#endif //RENDU_GIT_REVISION_H
