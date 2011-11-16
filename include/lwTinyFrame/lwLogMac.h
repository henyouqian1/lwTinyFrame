#ifndef __LW_LOG_MAC_H__
#define	__LW_LOG_MAC_H__

#ifdef __cplusplus
    #include <iostream>
#endif
#include <assert.h>

#ifdef NO_LWLOG
	#define lwinfo(_e) ((void)0)
	#define lwwarning(_e) ((void)0)
	#define lwerror(_e) ((void)0)
	#define lwfatal(_e) ((void)0)
	#define lwassert(_e) ((void)0)
#else
	#define lwinfo(_e) std::cout<<"Info: " << __FILE__ << " - " << __LINE__ << __FUNCTION__ << "\n" << _e << "\n";
	#define lwwarning(_e) std::cout<<"Warning: " << __FILE__ << " - " << __LINE__ << __FUNCTION__ << "\n" << _e << "\n";
	#define lwerror(_e) std::cout<<"Error: " << __FILE__ << " - " << __LINE__ << __FUNCTION__ << "\n" << _e << "\n";
	#define lwfatal(_e) std::cout<<"Fatal: " << __FILE__ << " - " << __LINE__ << __FUNCTION__ << "\n" << _e << "\n";
	#define lwassert(_e) assert(_e);
#endif

#endif __LW_LOG_MAC_H__