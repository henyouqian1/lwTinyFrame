// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __LW_TINYFRAME_STDAFX_H__
#define __LW_TINYFRAME_STDAFX_H__

#ifdef WIN32
	#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
		#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
	#endif

	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#ifdef __cplusplus
#   include <string>
#   include <set>

#	include "lwTinyFrame/lwLog.h"
#	include "lwTinyFrame/lwPreInclude.h"

#	ifdef WIN32
#		include "AL/al.h"
#		include "AL/alc.h"
#		include "AL/alut.h"
#   endif
#	ifdef __APPLE__
#       include <OpenAL/al.h>
#       include <OpenAL/alc.h>
#       include "AL/alut.h"
#   endif
#endif

#ifdef __OBJC__
#   include <Foundation/Foundation.h>
#   include <CFNetwork/CFNetwork.h>
#endif



#endif //__LW_TINYFRAME_STDAFX_H__
