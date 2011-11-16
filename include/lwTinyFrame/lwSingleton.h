#ifndef __LW_SINGLETON_H__
#define __LW_SINGLETON_H__

#include "lwTinyFrame/lwLog.h"

namespace lw {

template<typename T>
class Singleton
{
public:
	static T& s() {
		return *_pInstance;
	}
	static T* ps(){
		return _pInstance;
	}

protected:
	Singleton()	{
		lwassert(!_pInstance);
		_pInstance = static_cast< T* >( this );
	}
	virtual ~Singleton() {
		lwassert(_pInstance);
		_pInstance = NULL;
	}

protected:
	static T *_pInstance;
};

template<typename T> T* Singleton<T>::_pInstance = 0;


} //namespace lw

#endif //__LW_SINGLETON_H__