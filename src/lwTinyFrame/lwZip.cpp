#include "stdafx.h"
#include "lwTinyFrame/lwZip.h"

namespace lw{

	int compress(char* dest, int* destLen, const void* src, int srcLen, int level){
		return compress2((Bytef*)dest, (uLongf*)destLen, (const Bytef*)src, srcLen, level);
	}

	int decompress(char* dest, int* destLen, const void* src, int srcLen){
		return uncompress((Bytef*)dest, (uLongf*)destLen, (const Bytef*)src, srcLen);
	}

} //namespace lw