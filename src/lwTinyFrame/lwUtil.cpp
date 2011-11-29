#include "stdafx.h"
#include "lwTinyFrame/lwUtil.h"

#ifdef __APPLE__
#	include <CoreFoundation/cfDate.h>
#endif

namespace lw{

	void srand(){
#ifdef WIN32
		std::srand(GetTickCount());
#else ifdef __APPLE__
		std::srand(CFAbsoluteTimeGetCurrent());
#endif
	}

} //namespace lw

#ifdef WIN32
W2UTF8::W2UTF8(const wchar_t* w){
	_size = WideCharToMultiByte(CP_UTF8, NULL, w, -1, NULL, 0, NULL, NULL);
	_buffer = new char[_size];
	WideCharToMultiByte(CP_UTF8, NULL, w, -1, _buffer, _size, NULL, NULL);
}
W2UTF8::~W2UTF8(){
	delete [] _buffer;
}
int W2UTF8::size(){
	return _size;
}
char* W2UTF8::data(){
	return _buffer;
}

W2UTF8::operator const char*(){
	return _buffer;
}

UTF82W::UTF82W(const char* c){
	_size = MultiByteToWideChar(CP_UTF8, NULL, c, -1, NULL, 0);
	_buffer = new WCHAR[_size];
	MultiByteToWideChar(CP_UTF8, NULL, c, -1, _buffer, _size);
}
UTF82W::~UTF82W(){
	delete [] _buffer;
}
int UTF82W::size(){
	return _size;
}
wchar_t* UTF82W::data(){
	return _buffer;
}

UTF82W::operator const wchar_t*(){
	return _buffer;
}
#endif //#ifdef WIN32

#ifdef __APPLE__
#include "utf8.h"
W2UTF8::W2UTF8(const wchar_t* w){
	_size = wchar_to_utf8(w, wcslen(w), NULL, 0, 0);
	_buffer = new char[_size+1];
	wchar_to_utf8(w, _size, _buffer, _size, 0);
	_buffer[_size] = 0;
}
W2UTF8::~W2UTF8(){
	delete [] _buffer;
}
int W2UTF8::size(){
	return _size;
}
const char* W2UTF8::data(){
	return _buffer;
}

W2UTF8::operator const char*(){
	return _buffer;
}

UTF82W::UTF82W(const char* c){
	_size = utf8_to_wchar(c, strlen(c), NULL, 0, 0);
	_buffer = new wchar_t[_size+1];
	utf8_to_wchar(c, _size, _buffer, _size, 0);
	_buffer[_size] = 0;
}
UTF82W::~UTF82W(){
	delete [] _buffer;
}
int UTF82W::size(){
	return _size;
}
const wchar_t* UTF82W::data(){
	return _buffer;
}

UTF82W::operator const wchar_t*(){
	return _buffer;
}
#endif //#ifdef __APPLE__