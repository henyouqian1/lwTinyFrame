#ifndef __LW_ZIP_H__
#define __LW_ZIP_H__

#include "zlib/zlib.h"

namespace lw{
	int compress(char* dest, int* destLen, const void* src, int srcLen, int level = Z_DEFAULT_COMPRESSION);
	int decompress(char* dest, int* destLen, const void* src, int srcLen);

} //namespace lw

#endif //__LW_ZIP_H__