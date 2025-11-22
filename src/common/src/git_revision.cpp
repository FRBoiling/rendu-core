//
// Created by 沸腾 on 2025/10/24.
//

#include "common/git_revision.h"
#include "revision_data.h"

BEGIN_NAMESPACE_COMMON

char const* GitRevision::GetHash()
{
    return RENDU_GIT_COMMIT_HASH;
}

char const* GitRevision::GetDate()
{
    return RENDU_GIT_COMMIT_DATE;
}

char const* GitRevision::GetBranch()
{
    return RENDU_GIT_COMMIT_BRANCH;
}

char const* GitRevision::GetCMakeCommand()
{
    return RENDU_BUILD_CMAKE_COMMAND;
}

char const* GitRevision::GetCMakeVersion()
{
    return RENDU_BUILD_CMAKE_VERSION;
}

char const* GitRevision::GetHostOSVersion()
{
    return ""
#ifdef RENDU_BUILD_HOST_DISTRO_NAME
    RENDU_BUILD_HOST_DISTRO_NAME
#ifdef RENDU_BUILD_HOST_DISTRO_VERSION_ID
    " " RENDU_BUILD_HOST_DISTRO_VERSION_ID
#endif
    "; "
#endif
    RENDU_BUILD_HOST_SYSTEM     " "    RENDU_BUILD_HOST_SYSTEM_VERSION;
}

char const* GitRevision::GetBuildDirectory()
{
    return RENDU_BUILD_CMAKE_BUILD_DIRECTORY;
}

char const* GitRevision::GetSourceDirectory()
{
    return RENDU_BUILD_CMAKE_SOURCE_DIRECTORY;
}

char const* GitRevision::GetMySQLExecutable()
{
    return DATABASE_MYSQL_EXECUTABLE;
}

char const* GitRevision::GetFullDatabase()
{
    return DATABASE_FULL_DATABASE;
}

char const* GitRevision::GetHotfixesDatabase()
{
    return DATABASE_HOTFIXES_DATABASE;
}

#ifndef RENDU_API_USE_DYNAMIC_LINKING
#  define RENDU_LINKAGE_TYPE_STR "Static"
#else
#  define RENDU_LINKAGE_TYPE_STR "Dynamic"
#endif

char const* GitRevision::GetFullVersion()
{
    return "RenduCore rev. "
    RENDU_PRODUCTVERSION_STR
    " ("
    RENDU_BUILD_HOST_SYSTEM
    ", "
    RENDU_BUILD_PROCESSOR
    ", "
    RENDU_BUILD_TYPE
    ", " RENDU_LINKAGE_TYPE_STR ")";
}

char const* GitRevision::GetCompanyNameStr()
{
    return RENDU_COMPANYNAME_STR;
}

char const* GitRevision::GetLegalCopyrightStr()
{
    return RENDU_LEGALCOPYRIGHT_STR;
}

char const* GitRevision::GetFileVersionStr()
{
    return RENDU_FILEVERSION_STR;
}

char const* GitRevision::GetProductVersionStr()
{
    return RENDU_PRODUCTVERSION_STR;
}

END_NAMESPACE_COMMON