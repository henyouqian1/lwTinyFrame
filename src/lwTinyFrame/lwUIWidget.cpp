#include "stdafx.h"
#include "lwTinyFrame/lwUIWidget.h"
#include "lwTinyFrame/lwSprite.h"
#include "lwTinyFrame/lwTouchEvent.h"

namespace lw{

	namespace{
		struct UIData{
			std::list<UIWidget*> widgets;
			bool isAutoDraw;
			GestureMgr gesMgr;
			UIWidget* modalWidget;
		};

		UIData* g_pUIData = NULL;
	}

	void UIInit(){
		lwassert(g_pUIData == NULL);
		g_pUIData = new UIData;
		g_pUIData->isAutoDraw = true;
		g_pUIData->modalWidget = NULL;
	}
	void UIQuit(){
		UIClear();
		lwassert(g_pUIData);
		delete g_pUIData;
		g_pUIData = NULL;
	}
	void UIClear(){
		UIData* pd = g_pUIData;
		while( pd->widgets.size() ){
			std::list<UIWidget*>::iterator it = pd->widgets.begin();
			delete *it;
		}
	}
	void UIMain(float dt){
		UIData* pd = g_pUIData;
		std::list<UIWidget*>::iterator it = pd->widgets.begin();
		std::list<UIWidget*>::iterator itEnd = pd->widgets.end();
		for ( ; it != itEnd; ++it ){
			(*it)->main(dt);
		}
	}
	void UIDraw(){
		UIData* pd = g_pUIData;
		if ( pd->modalWidget ){
			pd->modalWidget->draw();
		}else{
			std::list<UIWidget*>::iterator it = pd->widgets.begin();
			std::list<UIWidget*>::iterator itEnd = pd->widgets.end();
			for ( ; it != itEnd; ++it ){
				(*it)->draw();
			}
		}
		
	}
	void UISetAutoDraw(bool b){
		UIData* pd = g_pUIData;
		pd->isAutoDraw = b;
	}
	bool UIIsAutoDraw(){
		UIData* pd = g_pUIData;
		return pd->isAutoDraw;
	}
	bool UIEventProcess(std::vector<lw::TouchEvent>& events){
		UIData* pd = g_pUIData;
		pd->gesMgr.onTouchEvent(events);

		const std::list<Gesture>& gesList = pd->gesMgr.getGestures();
		std::list<Gesture>::const_iterator gesIt = gesList.begin();
		std::list<Gesture>::const_iterator gesItEnd = gesList.end();
		std::list<UIWidget*>::reverse_iterator it;
		std::list<UIWidget*>::reverse_iterator itEnd;
		std::vector<UIWidget*>::const_reverse_iterator itg;
		std::vector<UIWidget*>::const_reverse_iterator itgEnd;
		bool isProcessed = false;
		for ( ; gesIt != gesItEnd; ++gesIt ){
			if ( gesIt->updated ){
				if ( pd->modalWidget ){
					isProcessed = true;
					if ( pd->modalWidget->processGesture(*gesIt) ){
						break;
					}
					
				}else{
					it = pd->widgets.rbegin();
					itEnd = pd->widgets.rend();
					for ( ; it != itEnd; ++it ){
						if ( (*it)->processGesture(*gesIt) ){
							isProcessed = true;
							break;
						}
					}
				}
			}
		}

		pd->gesMgr.main();
		return isProcessed;
	}

	void UISetModal(lw::UIWidget* pWidget){
		g_pUIData->modalWidget = pWidget;
	}

	UIWidget::UIWidget(UIWidget* pParent):
	_isEnable(true), _isVisible(true), _pGroup(NULL), _pParent(pParent){
		if ( pParent == NULL ){
			UIData* pd = g_pUIData;
			pd->widgets.push_back(this);
		}else{
			pParent->_children.push_back(this);
		}
		
	}
	UIWidget::~UIWidget(){
		while ( !_children.empty() ){
			_children.front()->setParent(_pParent);
		}
		if ( _pParent ){
			_pParent->_children.remove(this);
		}else{
			UIData* pd = g_pUIData;
			pd->widgets.remove(this);
		}
	}

	void UIWidget::setParent(UIWidget* pParent){
		if ( _pParent == pParent ){
			return;
		}
		if ( _pParent ){
			_pParent->_children.remove(this);
		}else{
			UIData* pd = g_pUIData;
			pd->widgets.remove(this);
		}
		_pParent = pParent;
		if ( pParent ){
			pParent->_children.push_back(this);
		}else{
			UIData* pd = g_pUIData;
			pd->widgets.push_back(this);
		}
	}

	void UIWidget::main(float dt){
		vMain(dt);
		if ( !_children.empty() ){
			std::list<UIWidget*>::iterator it = _children.begin();
			std::list<UIWidget*>::iterator itEnd = _children.end();
			for ( ; it != itEnd; ++it ){
				(*it)->main(dt);
			}
		}
	}
	void UIWidget::draw(){
		if ( _isVisible ){
			vDraw();
			if ( !_children.empty() ){
				std::list<UIWidget*>::iterator it = _children.begin();
				std::list<UIWidget*>::iterator itEnd = _children.end();
				for ( ; it != itEnd; ++it ){
					(*it)->draw();
				}
			}
		}
	}
	bool UIWidget::processGesture(const lw::Gesture& gesture){
		if ( _isEnable && _isVisible ){
			if ( _pGroup ){
				lw::Gesture ges = gesture;
				_pGroup->transXY(ges);
				if ( vGesture(ges) ){
					return true;
				}
			}else{
				if ( !_children.empty() ){
					std::list<UIWidget*>::iterator it = _children.begin();
					std::list<UIWidget*>::iterator itEnd = _children.end();
					for ( ; it != itEnd; ++it ){
						if ( (*it)->processGesture(gesture) ){
							return true;
						}
					}
				}
				if ( vGesture(gesture) ){
					return true;
				}
			}
		}
		return false;
	}
	void UIWidget::setPos(float x, float y){
		if ( _pGroup ){
			float gx, gy;
			_pGroup->getPos(gx, gy);
			x += gx;
			y += gy;
		}
		vSetPos(x, y);
	}
	void UIWidget::show(bool b){
		_isVisible = b;
		if ( !b && this == g_pUIData->modalWidget ){
			g_pUIData->modalWidget = NULL;
		}
		vShow(b);
	}
	void UIWidget::enable(bool b){
		_isEnable = b;
		vEnable();
	}

	UIGroup::UIGroup():_x(0.f), _y(0.f){
		_ort = lw::App::s().getConfig().orientation;
	}
	void UIGroup::add(UIWidget* pWidget){
		lwassert(pWidget);
		lwassert(pWidget->_pGroup == NULL);
		_widgets.push_back(pWidget);
		pWidget->_pGroup = this;
	}
	void UIGroup::clear(){
		std::vector<UIWidget*>::iterator it = _widgets.begin();
		std::vector<UIWidget*>::iterator itEnd = _widgets.end();
		for ( ; it != itEnd; ++it ){
			(*it)->_pGroup = NULL;
		}
		_widgets.clear();
	}
	void UIGroup::setPos(float x, float y){
		_x = x;
		_y = y;
	}
	void UIGroup::getPos(float& x, float& y){
		x = _x;
		y = _y;
	}
	void UIGroup::show(bool b){
		std::vector<UIWidget*>::iterator it = _widgets.begin();
		std::vector<UIWidget*>::iterator itEnd = _widgets.end();
		for ( ; it != itEnd; ++it ){
			(*it)->show(b);
		}
	}
	void UIGroup::enable(bool b){
		std::vector<UIWidget*>::iterator it = _widgets.begin();
		std::vector<UIWidget*>::iterator itEnd = _widgets.end();
		for ( ; it != itEnd; ++it ){
			(*it)->enable(b);
		}
	}
	void UIGroup::setOrient(lw::App::Orientation ort){
		_ort = ort;
	}
	void UIGroup::draw(){
		lw::App::Orientation ort = lw::App::s().getConfig().orientation;
		if ( ort != _ort ){
			lw::Sprite::flush();
			lw::App::s().setOrient(_ort);
		}
		std::vector<UIWidget*>::iterator it = _widgets.begin();
		std::vector<UIWidget*>::iterator itEnd = _widgets.end();
		for ( ; it != itEnd; ++it ){
			(*it)->draw();
		}
		lw::Sprite::flush();
		if ( ort != _ort ){
			lw::App::s().popOrient();
		}
	}
	void UIGroup::transXY(lw::Gesture& gesture){
		lw::TouchEvent& evt = gesture.evt;
		const lw::App::Config& config = lw::App::s().getConfig();
		int dOrient = _ort - config.orientation;
		if ( dOrient == 0 ){
			return;
		}
		if ( dOrient < 0 ){
			dOrient += 4;
		}
		if ( abs(dOrient) == 2 ){
			evt.x = config.width0-evt.x;
			evt.y = config.height0-evt.y;
		}else if (dOrient == 3){
			short temp = evt.x;
			evt.x = config.height0-evt.y;
			evt.y = temp;
		}else if (dOrient == 1){
			short temp = evt.x;
			evt.x = evt.y;
			evt.y = config.width0-temp;
		}
	}

} //namespace lw