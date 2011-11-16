#ifndef __LW_UI_BUTTON_H__
#define __LW_UI_BUTTON_H__

#include "lwUIWidget.h"

namespace lw{

	struct TouchEvent;
	class Sprite;
	class Sprite9;
	class Font;

	class UIButton;

	class ButtonCallback{
	public:
		virtual ~ButtonCallback(){}
		virtual void onButtonDown(lw::UIButton* pButton){}
		virtual void onButtonUp(lw::UIButton* pButton){}
		virtual void vOnClick(lw::UIButton* pButton){}
	};

	struct Button1Def{
		Button1Def(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName);
		void set(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName);
		const char* file;
		int upU, upV, downU, downV, disableU, disableV;
		int w, h;
		const char* fontName;
	};
	struct Button9Def{
		Button9Def(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter);
		void set(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter);
		const char* file;
		int upU, upV, downU, downV, disableU, disableV;
		int uvW1, uvW2, uvW3, uvH1, uvH2, uvH3;
		const char* fontName;
		GLfixed minMagFilter;
	};

	class UIButton : public UIWidget{
	public:
		static UIButton* create1(const Button1Def& def);
		static UIButton* create1(const char* file, int upU, int upV, int downU, int downV, int w, int h, const char* fontName);
		static UIButton* create1(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName);
		static UIButton* create9(const Button9Def& def);
		static UIButton* create9(const char* file, int upU, int upV, int downU, int downV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName);
		static UIButton* create9(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter);

		UIButton();
		virtual ~UIButton();

		virtual bool vGesture(const lw::Gesture& gesture);
		virtual void setColor(const lw::Color& color) = 0;
		virtual void setColor(const lw::Color& color, const lw::Color& downColor, const lw::Color& disableColor) = 0;
		virtual void setSize(float w, float h){}
		void setText(const wchar_t* text);
		void setTextColor(const lw::Color& color);
		void setTextColor(const lw::Color& color, const lw::Color& downColor, const lw::Color& disableColor);
		void setExt(int top, int bottom, int left, int right){
			_topExt = top; _bottomExt = bottom; _leftExt = left; _rightExt = right;
		}
		bool isDown(){
			return _isDown;
		}
		void setDown(bool b){
			_isDown = b;
			_isTracing = b;
		}
		void setCallback(ButtonCallback* pCallback){
			_pCallback = pCallback;
		}

		ButtonCallback* _pCallback;
		bool _isTracing;
		bool _isDown;
		float _x, _y, _w, _h;
		int _topExt, _bottomExt, _leftExt, _rightExt;
		Font* _pFontUp;
		Font* _pFontDown;
		Font* _pFontDisable;
		int _gesId;
	};

	

} //namespace lw



#endif //__LW_UI_BUTTON_H__