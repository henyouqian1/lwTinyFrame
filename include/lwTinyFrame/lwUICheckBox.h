#ifndef __LW_UI_CHECKBOX_H__
#define __LW_UI_CHECKBOX_H__

#include "lwUIWidget.h"

namespace lw{

	struct TouchEvent;
	class Sprite;
	class Sprite9;
	class Font;

	class UICheckBox;

	class CheckBoxCallback{
	public:
		virtual ~CheckBoxCallback(){}
		virtual void vOnCheck(lw::UICheckBox* pCb){}
		virtual void vOnUncheck(lw::UICheckBox* pCb){}
	};

	struct CheckBox1Def{
		CheckBox1Def(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName);
		void set(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName);
		const char* file;
		int upU, upV, downU, downV, disableU, disableV;
		int w, h;
		const char* fontName;
	};
	struct CheckBox9Def{
		CheckBox9Def(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter);
		void set(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter);
		const char* file;
		int upU, upV, downU, downV, disableU, disableV;
		int uvW1, uvW2, uvW3, uvH1, uvH2, uvH3;
		const char* fontName;
		GLfixed minMagFilter;
	};

	class UICheckBox : public UIWidget{
	public:
		static UICheckBox* create1(const CheckBox1Def& def);
		static UICheckBox* create1(const char* file, int upU, int upV, int downU, int downV, int w, int h, const char* fontName);
		static UICheckBox* create1(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName);
		static UICheckBox* create9(const CheckBox9Def& def);
		static UICheckBox* create9(const char* file, int upU, int upV, int downU, int downV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName);
		static UICheckBox* create9(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter);

		UICheckBox();
		virtual ~UICheckBox();

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
		bool isChecked(){
			return _isDown;
		}
		void check(bool b){
			_isDown = b;
		}
		void setCallback(CheckBoxCallback* pCallback){
			_pCallback = pCallback;
		}
		void setCheckOnly(){
			_isCheckOnly = true;
		}

		CheckBoxCallback* _pCallback;
		bool _isDown;
		float _x, _y, _w, _h;
		int _topExt, _bottomExt, _leftExt, _rightExt;
		Font* _pFontUp;
		Font* _pFontDown;
		Font* _pFontDisable;
		bool _isCheckOnly;
	};

	

} //namespace lw



#endif //__LW_UI_CHECKBOX_H__