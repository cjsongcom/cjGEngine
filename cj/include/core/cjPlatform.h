
#ifndef _CJPLATFORM_H_
#define _CJPLATFORM_H_

#include "cjPlatformDef.h"

#ifdef _DEBUG
#ifndef CJDEBUG
#define CJDEBUG
#endif
#endif


#if defined(CJ_PLATFORM_WIN32)
	#include "cjPlatform_Win.h"
#elif defined(CJ_PLATFORM_APPLE)
	#include "cjPlatform_Apple.h"
#else
	#include "cjPlatform_Linux.h"
#endif

#if !defined(CJLIB) && !defined(CJDLL)
        #error "Err! - need to define CJLIB or CJDLL"
#endif

#endif
