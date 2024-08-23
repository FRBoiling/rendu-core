/*
* Created by boil on 2024/11/14.
*/

#ifndef RENDU_CORE_CORE_OBJECT_DISPOSE_OBJECT_H_
#define RENDU_CORE_CORE_OBJECT_DISPOSE_OBJECT_H_

#include "core_define.h"


RD_NAMESPACE_BEGIN

class DisposeObject : public Object, public IDisposable, public ISupportInitialize {

};

RD_NAMESPACE_END

#endif//RENDU_CORE_CORE_OBJECT_DISPOSE_OBJECT_H_
