#ifndef __LW_UI_IMAGE_H__
#define __LW_UI_IMAGE_H__

#include "lwUIWidget.h"

namespace lw{

	class Sprite;
	class Sprite9;
	class UIImage;

	class UIImageCallback{
	public:
		virtual ~UIImageCallback(){}
		virtual void onImageTouch(lw::UIImage* pUIImage){}
	};

	class UIImage : public UIWidget{
	public:
		static UIImage* create(const char* fileName, int uvX, int uvY, int uvW, int uvH, float w, float h);
		~UIImage();

		void setCallback(UIImageCallback* p){
			_pCallback = p;
		}
		virtual void vSetPos(float x, float y);
		void setSize(float w, float h);
		void setColor(const lw::Color& color);
		void setFilter(GLfixed filter);

	private:
		virtual void vDraw();
		virtual bool vGesture(const lw::Gesture& gesture);
		UIImage(const char* fileName, int uvX, int uvY, int uvW, int uvH, float w, float h, bool& ok);
		Sprite* _pSprite;
		UIImageCallback* _pCallback;
		float _x, _y, _w, _h;
	};

	class UIImage9: public UIWidget{
	public:
		static UIImage9* create(const char* fileName, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3);
		~UIImage9();

		virtual void vSetPos(float x, float y);
		void setSize(float w, float h);
		void setColor(const lw::Color& color);
		void setFilter(GLfixed filter);

	private:
		UIImage9(const char* fileName, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, bool& ok);
		virtual void vDraw();
		Sprite9* _pSprite9;
	};


} //namespace lw



#endif //__LW_UI_IMAGE_H__