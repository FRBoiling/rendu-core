// This file is part of the rendu-core Project. See AUTHORS file for Copyright information
// Created by Administrator on 2022/7/6.
//

#ifdef _WIN32
#ifndef _WIN32_SERVICE_
#define _WIN32_SERVICE_

bool WinServiceInstall();
bool WinServiceUninstall();
bool WinServiceRun();

#endif                                                      // _WIN32_SERVICE_
#endif                                                      // _WIN32