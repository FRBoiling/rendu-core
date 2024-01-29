/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_BASE_DEFINE_H
#define RENDU_BASE_DEFINE_H

#ifndef RD_EXPORT
#    if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#        define RD_EXPORT __declspec(dllexport)
#        define RD_IMPORT __declspec(dllimport)
#        define RD_HIDDEN
#    elif defined __GNUC__ && __GNUC__ >= 4
#        define RD_EXPORT __attribute__((visibility("default")))
#        define RD_IMPORT __attribute__((visibility("default")))
#        define RD_HIDDEN __attribute__((visibility("hidden")))
#    else /* Unsupported compiler */
#        define RD_EXPORT
#        define RD_IMPORT
#        define RD_HIDDEN
#    endif
#endif

#ifndef RD_API
#    if defined RD_API_EXPORT
#        define RD_API RD_EXPORT
#    elif defined RD_API_IMPORT
#        define RD_API RD_IMPORT
#    else /* No API */
#        define RD_API
#    endif
#endif

#define RD_NAMESPACE_BEGIN  namespace rendu {

#define RD_NAMESPACE_END } /*namespace rendu*/

#define ALIAS(type, name) typedef type name // 定义一个宏，用来为type类型取别名为name

#endif //RENDU_BASE_DEFINE_H
