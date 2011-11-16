#ifndef __LW_UI_TEXT_H__
#define __LW_UI_TEXT_H__

#include "lwUIWidget.h"
#include "lwFont.h"

namespace lw{
	class UIText : public UIWidget{
	public:
		static UIText* create(const char* fontName);
		~UIText();
		virtual void vSetPos(float x, float y);

		void setAlign(FontAlign align){
			_pFont->setAlign(align);
		}
		void setText(const wchar_t* text){
			_pFont->setText(text);
		}
		void setRotate(float rotate){
			_pFont->setRotate(rotate);
		}
		void setScale(float scaleX, float scaleY){
			_pFont->setScale(scaleX, scaleY);
		}
		void setColor(Color color){
			_pFont->setColor(color);
		}
		Font* getFont(){
			return _pFont;
		}

	private:
		virtual void vDraw();

	private:
		UIText(const char* fileName, bool& ok);
		Font* _pFont;
	};

} //namespace lw



#endif //__LW_UI_TEXT_H__