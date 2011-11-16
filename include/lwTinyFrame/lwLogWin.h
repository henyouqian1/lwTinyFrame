#ifndef __LW_LOGW32_H__
#define	__LW_LOGW32_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

#include "lwLock.h"

namespace lw
{

	static const char DEFAULT_LOG_FILE[] = "log.txt";
	static const char LOG_SEPARATOR[] = "--------------------------\n";

	class Log 
	{
	public:
		enum {
			WHITE			=	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
			RED				=	FOREGROUND_RED,
			GREEN			=	FOREGROUND_GREEN,
			BLUE			=	FOREGROUND_BLUE,
			YELLOW			=	FOREGROUND_RED | FOREGROUND_GREEN,
			RED_LIGHT		=	FOREGROUND_RED | FOREGROUND_INTENSITY,
			GREEN_LIGHT		=	FOREGROUND_GREEN | FOREGROUND_INTENSITY,
			BLUE_LIGHT		=	FOREGROUND_BLUE | FOREGROUND_INTENSITY,
			YELLOW_LIGHT	=	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
			BG_YELLOW_LIGHT	=	BACKGROUND_RED | BACKGROUND_GREEN | FOREGROUND_INTENSITY,
		};

		Log(){
			_hCon = GetStdHandle(STD_OUTPUT_HANDLE);
			errno_t err = fopen_s(&_pf, DEFAULT_LOG_FILE, "wt, ccs=UNICODE");
			if ( err != 0 || _pf == NULL ) {
				//{_asm int 3};
				//ExitProcess(-1);
				std::cout << "warning: no log file\n";
				_pf = NULL;
			}
		}
		~Log(){
			if (_pf)
				fclose(_pf);
		}

		std::wstringstream& logBegin(const WCHAR* filePath, int lineNum, const WCHAR* func, const WCHAR* tag, WORD logColor) {
			//filename
			size_t size = wcslen(filePath);
			size_t n = 0;
			for ( n = size-1; n >= 0; --n ){
				if ( filePath[n] == (WCHAR)'\\' ){
					++n;
					break;
				}
			}
			const WCHAR* fileName = filePath+n;

			//time
			WCHAR strTime[16];
			GetTimeFormatW(LOCALE_SYSTEM_DEFAULT, 0, NULL, L"HH:mm:ss", strTime, 16);

			//output
			_sstream.str(L"");
			_sstream<<LOG_SEPARATOR<<L"["<<tag<<L"] - "<<strTime<<L" - "<<fileName<<L"("<<lineNum<<L"): "<<func<<L"\n";

			//color
			SetConsoleTextAttribute(_hCon, logColor);
			return _sstream;
		}

		void logEnd(){
			DWORD ws;
			WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),_sstream.str().c_str(),(DWORD)_sstream.str().length(),&ws,NULL);
			if (_pf) {
				fwprintf_s(_pf, L"%ls", _sstream.str().c_str());
				fflush(_pf);
			}
		}

		static Log& getInstance() {
			static Log inst;
			return inst;
		}

	private:
		std::wstringstream _sstream;
		FILE* _pf;
		HANDLE _hCon;
	};


	//==========================================================
	//log macro
	//==========================================================
#define bout(_e, _color, _tag) do {																	\
	lw::Locker __l(lw::CriticalSection::getGlobal());													\
	lw::Log::getInstance().logBegin(__FILEW__, __LINE__, __FUNCTIONW__, _tag, _color)<<_e<<L"\n";	\
	lw::Log::getInstance().logEnd();} while (0)

#ifdef NO_LWLOG
#define lwinfo(_e) ((void)0)
#define lwwarning(_e) ((void)0)
#define lwerror(_e) ((void)0)
#define lwfatal(_e) ((void)0)
#define lwprompt(_e) ((void)0)
#else
#define lwinfo(_e) bout(_e, lw::Log::GREEN_LIGHT, L"INFO")
#define lwwarning(_e) bout(_e, lw::Log::YELLOW, L"WARNING")
#define lwerror(_e) bout(_e, lw::Log::YELLOW_LIGHT, L"ERROR")
#define lwfatal(_e) do {bout(_e, lw::Log::RED_LIGHT, L"FATAL"); {_asm int 3};} while(0)
#define lwprompt(_e) bout(_e, lw::Log::WHITE, L"PROMPT")
#endif

#ifndef NDEBUG
#define lwassert(_e) do {if (_e != 0) break; else { lw::Log::getInstance().logBegin(__FILEW__, __LINE__, __FUNCTIONW__, L"ASSERT", lw::Log::RED_LIGHT)<<#_e"\n"; lw::Log::getInstance().logEnd(); {_asm int 3}; }} while (1)
#else
#define lwassert(_e) ((void)0)
#endif



}	//namespace lw

#endif //__LW_LOGW32_H__