#include "stdafx.h"
#include "lwTinyFrame/lwApp.h"
#include "lwTinyFrame/lwTask.h"
#include "lwTinyFrame/lwFileSys.h"
#include "lwTinyFrame/lwTouchEvent.h"
#include "lwTinyFrame/lwTextureRes.h"
#include "lwTinyFrame/lwSprite.h"
#include "lwTinyFrame/lwFont.h"
#include "lwTinyFrame/lwUIWidget.h"

#ifdef __APPLE__
    #include "lwTinyFrame/macUtil.h"
    #import <UIKit/UIKit.h>
#endif

namespace lw{
	
	
#ifdef WIN32
	void App::drawEnd(){
		glFinish();
		eglWaitGL();
		eglSwapBuffers(_eglDisplay, _eglSurface);
	}
#endif
	
	
	App::App()
		:_isRunning(true){
	}

	App::~App(){
	}
	
#ifdef WIN32
	int done = 0;
	HWND g_hwnd;

	void App::transXY(short& x, short& y){
		int dOrient = _config.orientation - _config.orientation0;
		if ( dOrient < 0 ){
			dOrient += 4;
		}
		if ( abs(dOrient) == 2 ){
			x = _config.width0-x;
			y = _config.height0-y;
		}else if (dOrient == 3){
			short temp = x;
			x = _config.height0-y;
			y = temp;
		}else if (dOrient == 1){
			short temp = x;
			x = y;
			y = _config.width0-temp;
		}
	}
	
	
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		App* pApp = App::ps();
		static short prevX = 0;
		static short prevY = 0;
		static bool touched = false;
		static bool lDown = false;
		TouchEvent evt;
		std::vector<TouchEvent> evts;
		switch (message){
			case WM_CLOSE:
				done=1;
				PostQuitMessage(0);
				return 1;
			case WM_MOUSEMOVE:
				if ( lDown ){
					const App::Config& config = pApp->getConfig();
					evt.x = LOWORD(lParam);
					evt.y = HIWORD(lParam);
					pApp->transXY(evt.x, evt.y);
					if ( touched ){
						evt.prevX = prevX;
						evt.prevY = prevY;
						if ( evt.x < 0 || evt.x >= config.width || evt.y < 0 || evt.y >= config.height ){
							touched = false;
							evt.type = TouchEvent::UNTOUCH;
						}else{
							evt.type = TouchEvent::MOVE;
							prevX = evt.x;
							prevY = evt.y;
						}
						evts.push_back(evt);
						g_gestrueMgr.onTouchEvent(evts);
						if ( UIEventProcess(evts) == false ){
							TaskMgr::onTouchEvent(evts);
						}
						g_gestrueMgr.main();
					}else{
						if ( (evt.prevX < 0 || evt.prevX >= config.width || evt.prevY < 0 || evt.prevY >= config.height)
							&&(evt.x >= 0 && evt.x < config.width && evt.y>=0 && evt.y < config.height )){
							touched = true;
							evt.type = TouchEvent::TOUCH;
							prevX = evt.x;
							prevY = evt.y;
							evt.prevX = evt.x; 
							evt.prevY = evt.y;
							evts.push_back(evt);
							g_gestrueMgr.onTouchEvent(evts);
							if ( UIEventProcess(evts) == false ){
								TaskMgr::onTouchEvent(evts);
							}
							g_gestrueMgr.main();
						}
					}
				}
				break;
			case WM_LBUTTONDOWN:
				touched = true;
				lDown = true;
				evt.type = TouchEvent::TOUCH;
				evt.x = LOWORD(lParam);
				evt.y = HIWORD(lParam);
				pApp->transXY(evt.x, evt.y);
				evt.prevX = evt.x;
				evt.prevY = evt.y;
				prevX = evt.x;
				prevY = evt.y;
				evts.push_back(evt);
				g_gestrueMgr.onTouchEvent(evts);
				if ( UIEventProcess(evts) == false ){
					TaskMgr::onTouchEvent(evts);
				}
				g_gestrueMgr.main();
				::SetCapture(hWnd);
				break;
			case WM_LBUTTONUP:
				touched = false;
				lDown = false;
				evt.type = TouchEvent::UNTOUCH;
				evt.x = LOWORD(lParam);
				evt.y = HIWORD(lParam);
				pApp->transXY(evt.x, evt.y);
				evt.prevX = prevX;
				evt.prevY = prevY;
				evts.push_back(evt);
				g_gestrueMgr.onTouchEvent(evts);
				if ( UIEventProcess(evts) == false ){
					TaskMgr::onTouchEvent(evts);
				}
				g_gestrueMgr.main();
				::ReleaseCapture();
				break;
			default:
				break;
		}
		
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	int TestEGLError(HWND hWnd, char* pszLocation)
	{
		/*
		 eglGetError returns the last error that has happened using egl,
		 not the status of the last called function. The user has to
		 check after every single egl call or at least once every frame.
		 */
		
		EGLint iErr=eglGetError();
		if (iErr!=EGL_SUCCESS)
		{
			TCHAR pszStr[256];
			swprintf_s(pszStr, 256, L"%s failed (%d).\n", pszLocation, iErr);
			MessageBox(hWnd, pszStr, L"Error", MB_OK|MB_ICONEXCLAMATION);
			return 0;
		}
		
		return 1;
	}

	int App::run(){
		// Windows variables
		HDC  hDC=0;
		
		// Register the windows class
		WNDCLASS sWC;
		
		sWC.style=CS_HREDRAW | CS_VREDRAW;
		sWC.lpfnWndProc=WndProc;
		sWC.cbClsExtra=0;
		sWC.cbWndExtra=0;
		sWC.hInstance=GetModuleHandle(NULL);
		sWC.hIcon=0;
		sWC.hCursor=0;
		sWC.lpszMenuName=0;
		sWC.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
		sWC.lpszClassName=L"LWCLS";
		
		int nWidth=_config.width;
		int nHeight=_config.height;
		
		ATOM registerClass=RegisterClass(&sWC);
		if (!registerClass)
		{
			MessageBox(0, L"Failed to register the window class", L"Error", MB_OK | MB_ICONEXCLAMATION);
		}
		
		// Create the eglWindow
		RECT sRect;
		SetRect(&sRect, 0, 0, nWidth, nHeight);
		AdjustWindowRectEx(&sRect, WS_CAPTION | WS_SYSMENU, 0, 0);
		int w = sRect.right - sRect.left;
		int h = sRect.bottom - sRect.top;
		g_hwnd=CreateWindow(sWC.lpszClassName, _config.title.c_str(), WS_VISIBLE | WS_SYSMENU, 0, 0, w, h, NULL, NULL, GetModuleHandle(NULL), NULL);
		hDC = GetDC(g_hwnd);

		initEgl(g_hwnd);
		
		DWORD t1 = ::timeGetTime();
		DWORD t2 = ::timeGetTime();
		/* Render stuff */
		do {
			//main
			int dt = t2-t1;
			if (dt == 0)
				dt = 1;
			if ( dt > 100 ){
				dt = 100;
			}
			t1 = t2;

			if ( _config.maxFPS > 0 ){
				float frameDuration = 1000.f/_config.maxFPS;
				main(frameDuration);
			}else{
				main((float)dt);
			}

			if ( _isRunning ){
				draw((float)dt);
				drawEnd();
			}
			
			if (done){
				break;
			}

			MSG msg;
			while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			} // while (PeekMessage())

			t2 = ::timeGetTime();
			if ( _config.maxFPS > 0 ){
				float frameDuration = 1000.f/_config.maxFPS;
				dt = t2 - t1;
				int sleepT = (int)(frameDuration-dt);
				if ( dt < frameDuration ){
					Sleep(sleepT);
				}
				t2 += sleepT;
			}
		} while(1);
		
		eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglTerminate(_eglDisplay);
		
		if (hDC){
			ReleaseDC(g_hwnd, hDC);
		}
		if (g_hwnd)
		{
			DestroyWindow(g_hwnd);
		}
		
		return 0;
	}
	
	void App::initEgl(NativeWindowType eglWindow){
		// Windows variables
		HDC  hDC=0;
		
		// EGL variables
		EGLConfig  eglConfig=0;
		EGLContext eglContext=0;
		EGLint pi32ConfigAttribs[128];
		EGLint iMajorVersion, iMinorVersion;
		int iConfigs;
		
		// Get the associated device context
		hDC=GetDC(eglWindow);
		lwassert(hDC);
		
		_eglDisplay=eglGetDisplay((NativeDisplayType)hDC);
		if(_eglDisplay==EGL_NO_DISPLAY){
			_eglDisplay=eglGetDisplay((NativeDisplayType)EGL_DEFAULT_DISPLAY);
		}
		lwassert(_eglDisplay!=EGL_NO_DISPLAY);
		
		if (!eglInitialize(_eglDisplay, &iMajorVersion, &iMinorVersion)){
			lwassert(0);
		}
		
		int i=0;
		pi32ConfigAttribs[i++]=EGL_RED_SIZE;
		pi32ConfigAttribs[i++]=5;
		pi32ConfigAttribs[i++]=EGL_GREEN_SIZE;
		pi32ConfigAttribs[i++]=6;
		pi32ConfigAttribs[i++]=EGL_BLUE_SIZE;
		pi32ConfigAttribs[i++]=5;
		pi32ConfigAttribs[i++]=EGL_ALPHA_SIZE;
		pi32ConfigAttribs[i++]=0;
		pi32ConfigAttribs[i++]=EGL_DEPTH_SIZE;
		pi32ConfigAttribs[i++]=16;
		pi32ConfigAttribs[i++]=EGL_SURFACE_TYPE;
		pi32ConfigAttribs[i++]=EGL_WINDOW_BIT;
		pi32ConfigAttribs[i++]=EGL_NONE;
		
		if (!eglChooseConfig(_eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs) || (iConfigs != 1)){
			lwassert(0);
		}
		
		_eglSurface=eglCreateWindowSurface(_eglDisplay, eglConfig, eglWindow, NULL);
		if (_eglSurface==EGL_NO_SURFACE){
			eglGetError(); // Clear error
			_eglSurface=eglCreateWindowSurface(_eglDisplay, eglConfig, NULL, NULL);
		}
		lwassert(_eglSurface!=EGL_NO_SURFACE);
		
		if (!TestEGLError(eglWindow, "eglCreateWindowSurface")){
			lwassert(0);
		}
		
		eglContext=eglCreateContext(_eglDisplay, eglConfig, NULL, NULL);
		if (!TestEGLError(eglWindow, "eglCreateContext")){
			lwassert(0);
		}
		
		eglMakeCurrent(_eglDisplay, _eglSurface, _eglSurface, eglContext);
		if (!TestEGLError(eglWindow, "eglMakeCurrent"))
		{
			lwassert(0);
		}
		
		//glViewport(0, 0, _config.height, _config.width);
		glViewport(0, 0, _config.width, _config.height);
	}

	void App::flMain(){
		static DWORD t1 = ::timeGetTime();
		static DWORD t2 = 0;
		t2 = ::timeGetTime();
		int dt = t2-t1;
		if (dt == 0){
			Sleep(1);
			return;
		}
		if ( dt > 100 ){
			dt = 100;
		}
		t1 = t2;

		main((float)dt);
		draw((float)dt);
		drawEnd();
		Sleep(1);
	}

	HWND App::getHwnd(){
		return g_hwnd;
	}
	
#endif //WIN32
	
	const App::Config& App::getConfig(){
		return _config;
	}

	void App::setOrient(App::Orientation orient){
		Config& conf = _config;
		conf.prevOrient = conf.orientation;
		conf.orientation = orient;
		if ( abs(orient-conf.orientation0) == 1 ){
			conf.width = conf.height0;
			conf.height = conf.width0;
		}else{
			conf.width = conf.width0;
			conf.height = conf.height0;
		}
#ifdef __APPLE__
		if ( _config.orientation == ORIENTATION_LEFT ){
			macOnChangeOrientation(0);
		}else if ( _config.orientation == ORIENTATION_UP ){
			macOnChangeOrientation(1);
		}else if ( _config.orientation == ORIENTATION_RIGHT ){
			macOnChangeOrientation(2);
		}else if ( _config.orientation == ORIENTATION_DOWN ){
			macOnChangeOrientation(3);
		}
#endif //#ifdef __APPLE__
	}

	void App::popOrient(){
		_config.orientation = _config.prevOrient;

		Config& conf = _config;
		if ( abs(conf.orientation-conf.orientation0) == 1 ){
			conf.width = conf.height0;
			conf.height = conf.width0;
		}else{
			conf.width = conf.width0;
			conf.height = conf.height0;
		}
#ifdef __APPLE__
		if ( _config.orientation == ORIENTATION_LEFT ){
			macOnChangeOrientation(0);
		}else if ( _config.orientation == ORIENTATION_UP ){
			macOnChangeOrientation(1);
		}else if ( _config.orientation == ORIENTATION_RIGHT ){
			macOnChangeOrientation(2);
		}else if ( _config.orientation == ORIENTATION_DOWN ){
			macOnChangeOrientation(3);
		}

#endif //#ifdef __APPLE__
	}
	
	float App::getAspect(){
		return (float)_config.width/(float)_config.height;
	}

	float App::getCurrFPS(){
		return _currFPS;
	}
	
	void App::init(){
		vInit();
		
#ifdef __APPLE__
		_config.width = 320;
		_config.height = 480;
#endif
		switch (_config.orientation){
		case ORIENTATION_LEFT:
		case ORIENTATION_RIGHT:
			{
				unsigned short temp = _config.height;
				_config.height = _config.width;
				_config.width = temp;
			}
			break;
		default:
			break;
		}

		_config.orientation0 = _config.orientation;
		_config.width0 = _config.width;
		_config.height0 = _config.height;

		UIInit();
	}
	
	void App::quit(){
		vQuit();
		TaskMgr::quit();
		UIQuit();
		TextureRes::quit();
		Font::quit();
		
	}
	
	void App::main(float dt){
        vMain();
		TaskMgr::processPending();
		TaskMgr::main(dt);
		UIMain(dt);
		
		static float t = 0;
		static int frm = 0;
		t += dt;
		frm += 1;
		if ( t > 200 ){
			_currFPS = frm/t*1000.f;
			t = 0;
			frm = 0;
		}
	}
	
	void App::draw(float dt){
		TaskMgr::draw(dt);
		if ( UIIsAutoDraw() ){
			UIDraw();
		}
		Sprite::flush();
	}

	void App::setRunning(bool b){
		_isRunning = b;
	}

	bool App::isRunning(){
		return _isRunning;
	}

	void App::setFrameInterval(int interval){
		_config.maxFPS = 60;
#ifdef __APPLE__
        [[UIApplication sharedApplication].keyWindow.rootViewController setAnimationFrameInterval:interval];
#endif
	}

	float time2Frame(float ms, float fps){
		return ms * 0.001f * fps;
	}
	
	
} //namespace lw