#ifndef __LW_PREINCLUDE_H__
#define __LW_PREINCLUDE_H__

#ifdef WIN32
#	include <Windows.h>
#	include <mmsystem.h>
#	include "GLES/gl.h"
#	include "GLES/egl.h"
#	include "GLES/glext.h"
#else
#	include <OpenGLES/ES1/gl.h>
#	include <OpenGLES/ES1/glext.h>
#endif

#ifdef __cplusplus
#	include <vector>
#	include <list>
#   include <map>
#	include "cml/cml.h"
#   ifdef __APPLE__
#       include "lwTinyFrame/lwMinMax.h"
#   endif
#	include "lwTinyFrame/lwColor.h"
namespace cml{
	typedef matrix44f_c Matrix44;
	typedef matrix33f_c Matrix33;
	typedef matrix22f_c Matrix22;
	typedef vector2f Vector2;
	typedef vector3f Vector3;
	typedef vector4f Vector4;
}
#endif

#ifdef WIN32
#	pragma comment(lib, "libgles_cm.lib")
#	pragma comment(lib, "winmm.lib")
#	pragma comment(lib, "OpenAL32.lib")
#	pragma comment(lib, "winHTTP.lib")
#endif

#include "lwTinyFrame/lwSingleton.h"

#endif //__LW_PREINCLUDE_H__