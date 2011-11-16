#ifndef __LW_APP_H__
#define __LW_APP_H__

namespace lw{

	class App : public lw::Singleton<App>{
	public:
		
		enum Orientation{
			ORIENTATION_LEFT,
			ORIENTATION_UP,
			ORIENTATION_RIGHT,
			ORIENTATION_DOWN,
		};
		
		struct Config
		{
			Config()
			:title(L"no title")
			,width(320)
			,height(480)
			,orientation(ORIENTATION_UP)
			,prevOrient(ORIENTATION_UP)
			,maxFPS(60){
			}
			std::wstring title;
			unsigned short width; 
			unsigned short height;
			unsigned short width0; 
			unsigned short height0;
			Orientation orientation;
			Orientation prevOrient;
			Orientation orientation0;
			int maxFPS;
		};

		App();
		virtual ~App();

		void main(float dt);
		void draw(float dt);

		const Config& getConfig();
		void setOrient(Orientation orient);
		void popOrient();

		float getAspect();
		float getCurrFPS();

		void setRunning(bool b);
		bool isRunning();
		void setFrameInterval(int interval);

#ifdef WIN32
		int run();
		void initEgl(NativeWindowType hwnd);
		void flMain();
		HWND getHwnd();
		void drawEnd();
		void transXY(short& x, short& y);
#endif

	private:
		virtual void vInit(){};
		virtual void vQuit(){};
		virtual void vMain(){};

	public:
		void init();
		void quit();
		
	protected:
		Config _config;
		float _currFPS;
		bool _isRunning;
#ifdef WIN32
		EGLDisplay _eglDisplay;
		EGLSurface _eglSurface;
#endif
	};

	float time2Frame(float ms, float fps);
	

} //namespace lw



#endif //__LW_APP_H__