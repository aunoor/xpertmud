//
// Created on 28.12.16.
//

#ifndef XPERTMUD_CONFIG_H
#define XPERTMUD_CONFIG_H

#include <qsystemdetection.h>

#define APP_NAME  "Xpertmud"
#define APP_VERSION  "3.2.0a"


#ifdef Q_OS_UNIX
  #ifdef Q_OS_MACOS
    #define RESOURCES_PATH_PREFIX qApp->applicationDirPath()+QDir::separator()
    #define DLL_PATH_PREFIX qApp->applicationDirPath()+QDir::separator()
  #else
    //Linux, *BSD and other unix-like OS
    #define RESOURCES_PATH_PREFIX "/usr/local/share/xpertmud/"
    #define DLL_PATH_PREFIX "/usr/local/lib/xpertmud"
  #endif
#endif

#ifdef Q_OS_WIN
  #define RESOURCES_PATH_PREFIX QString("")
  #define DLL_PATH_PREFIX QString("")
#endif



#endif //XPERTMUD_CONFIG_H
