
#ifndef _CJSDM_H_
#define _CJSDM_H_

//
//#define cjDeclareSDM(libname, exptag) \
//class exptag libname##SDM \
//{ \
//public: \
//	libname##SDM(); \
//	static bool Init(); \
//	static void Shutdown(); \
//protected: \
//	static bool ms_init; \
//};
//
//
//#define cjImplSDMConstructor(libname, dep) \
//    bool libname##SDM::ms_init = false; \
//    libname##SDM::libname##SDM() \
//    { \
//        static int numCalls = 0; \
//        if(numCalls++ == 0) \
//        { \
//			cjOn##libname##SDMConstructCall(); \
//            cjStaticDataManager::AddLib(#libname, \
//                libname##SDM::Init, libname##SDM::Shutdown, dep); \
//        } \
//    }

#endif

