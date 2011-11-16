#include "stdafx.h"
#include "lwTinyFrame/lwUIImage.h"
#include "lwTinyFrame/lwSprite.h"
#include "lwTinyFrame/lwTouchEvent.h"

namespace lw{

	UIImage* UIImage::create(const char* fileName, int uvX, int uvY, int uvW, int uvH, float w, float h){
		lwassert(fileName);
		bool ok = false;
		UIImage* pUIImage = new UIImage(fileName, uvX, uvY, uvW, uvH, w, h, ok);
		lwassert(pUIImage);
		if ( !ok ){
			delete pUIImage;
			return NULL;
		}
		return pUIImage;
	}

	UIImage::UIImage(const char* fileName, int uvX, int uvY, int uvW, int uvH, float w, float h, bool& ok):_pCallback(NULL){
		_pSprite = Sprite::create(fileName);
		if ( _pSprite == NULL ){
			lwerror("Sprite::create failed: file=" <<fileName);
			return;
		}
		_pSprite->setUV(uvX, uvY, uvW, uvH);
		setSize(w, h);
		ok = true;
	}

	UIImage::~UIImage(){
		if ( _pSprite ){
			delete _pSprite;
		}
	}

	void UIImage::vDraw(){
		_pSprite->collect();
	}

	bool UIImage::vGesture(const lw::Gesture& gesture){
		const lw::TouchEvent& evt = gesture.evt;
		switch (evt.type)
		{
		case lw::TouchEvent::TOUCH:
			if ( evt.x >= _x && evt.x < _x + _w && evt.y >= _y && evt.y < _y + _h ){
				if ( _pCallback ){
					_pCallback->onImageTouch(this);
				}
				return true;
			}
			break;
		}
		return false;
	}

	void UIImage::vSetPos(float x, float y){
		_pSprite->setPos(x, y);
		_x = x;
		_y = y;
	}
	void UIImage::setSize(float w, float h){
		_pSprite->setSize(w, h);
		_w = w;
		_h = h;
	}
	void UIImage::setColor(const Color& color){
		_pSprite->setColor(color);
	}
	void UIImage::setFilter(GLfixed filter){
		_pSprite->setFilter(filter, filter);
	}

	UIImage9* UIImage9::create(const char* fileName, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3){
		lwassert(fileName);
		bool ok = false;
		UIImage9* pUIImage9 = new UIImage9(fileName, uvX, uvY, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3, ok);
		lwassert(pUIImage9);
		if ( !ok ){
			delete pUIImage9;
			return NULL;
		}
		return pUIImage9;
	}
	
	UIImage9::UIImage9(const char* fileName, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, bool& ok){
		_pSprite9 = Sprite9::create(fileName, uvX, uvY, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3);
		if ( _pSprite9 == NULL ){
			lwerror("Sprite::create failed: file=" <<fileName);
			return;
		}
		ok = true;
	}

	UIImage9::~UIImage9(){
		if ( _pSprite9 ){
			delete _pSprite9;
		}
	}

	void UIImage9::vSetPos(float x, float y){
		_pSprite9->setPos(x, y);
	}
	void UIImage9::setSize(float w, float h){
		_pSprite9->setSize(w, h);
	}
	void UIImage9::setColor(const lw::Color& color){
		_pSprite9->setColor(color);
	}
	void UIImage9::vDraw(){
		_pSprite9->collect();
	}
	void UIImage9::setFilter(GLfixed filter){
		_pSprite9->setFilter(filter, filter);
	}
	

} //namespace lw