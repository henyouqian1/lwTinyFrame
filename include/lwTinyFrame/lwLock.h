#ifndef __LW_LOCK_H__
#define	__LW_LOCK_H__

namespace lw
{

#ifdef WIN32
class CriticalSection
{
public:
	CriticalSection() {
		InitializeCriticalSection(&_cs);
	}
	~CriticalSection() {
		DeleteCriticalSection(&_cs);
	}

	void enter() {
		EnterCriticalSection(&_cs);
	}
	void leave() {
		LeaveCriticalSection(&_cs);
	}

	static CriticalSection& getGlobal() {
		static CriticalSection cs;
		return cs;
	}

private:
	CRITICAL_SECTION _cs;
};
#endif //#ifdef WIN32

#ifdef __APPLE__
class CriticalSection
{
public:
	CriticalSection() {
		pthread_mutex_init(&_mutex, NULL);
	}
	~CriticalSection() {
		pthread_mutex_destroy(&_mutex);
	}

	void enter() {
		pthread_mutex_lock(&_mutex);
	}
	void leave() {
		pthread_mutex_unlock(&_mutex);
	}

	static CriticalSection& getGlobal() {
		static CriticalSection cs;
		return cs;
	}

private:
	pthread_mutex_t _mutex;
};
#endif //#ifdef __APPLE__

class Locker
{
public:
	Locker(CriticalSection& cs) :_cs(&cs){
		_cs->enter();
	}
	~Locker() {
		_cs->leave();
	}
private:
	CriticalSection* _cs;
};


} //namespace lw

#endif //__LW_LOCK_H__