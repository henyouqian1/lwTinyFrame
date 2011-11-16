#ifndef __LW_UI_WIDGET_H__
#define __LW_UI_WIDGET_H__

#include "lwTinyFrame/lwApp.h"

namespace lw{

	struct TouchEvent;
	struct Gesture;
	class UIGroup;
	class UIWidget;

	void UIInit();
	void UIQuit();
	void UIClear();
	void UIMain(float dt);
	void UIDraw();
	bool UIEventProcess(std::vector<lw::TouchEvent>& events);
	void UISetAutoDraw(bool b);
	bool UIIsAutoDraw();
	void UISetModal(lw::UIWidget* pWidget);

	class UIGroup;

	class UIWidget
	{
	public:
		UIWidget(UIWidget* pParent = NULL);
		virtual ~UIWidget();
		void setParent(UIWidget* pParent);

		void main(float dt);
		void draw();
		bool processGesture(const lw::Gesture& gesture);
		void setPos(float x, float y);
		void show(bool b);
		void enable(bool b);
		bool isVisible(){
			return _isVisible;
		}
		bool isEnable(){
			return _isEnable;
		}
		
	protected:
		bool _isEnable;
		bool _isVisible;
		UIGroup* _pGroup;
		UIWidget* _pParent;
		std::list<UIWidget*> _children;

	private:
		virtual void vMain(float dt){}
		virtual void vDraw(){}
		virtual bool vGesture(const lw::Gesture& gesture){return false;}
		virtual void vShow(bool b){}
		virtual void vEnable(){}
		virtual void vSetPos(float x, float y){};

	friend class UIGroup;
	};
	
	class UIGroup{
	public:
		UIGroup();
		void add(UIWidget* pWidget);
		void clear();
		void setPos(float x, float y);
		void getPos(float& x, float& y);
		void show(bool b);
		void enable(bool b);
		void draw();
		void setOrient(lw::App::Orientation ort);
		void transXY(lw::Gesture& gesture);
		const std::vector<UIWidget*>& getWidgets(){
			return _widgets;
		}

	private:
		float _x, _y;
		std::vector<UIWidget*> _widgets;
		lw::App::Orientation _ort;
	};


} //namespace lw



#endif //__LW_UI_WIDGET_H__