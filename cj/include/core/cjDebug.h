
#ifndef _CJAPPDEBUG_H_
#define _CJAPPDEBUG_H_

#if defined(_MSCVER)

#define cjDEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new cjDEBUG_NEW

#endif

#endif
