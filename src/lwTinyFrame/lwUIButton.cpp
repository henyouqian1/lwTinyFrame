#include "stdafx.h"
#include "lwTinyFrame/lwUIButton.h"
#include "lwTinyFrame/lwTouchEvent.h"
#include "lwTinyFrame/lwSprite.h"
#include "lwTinyFrame/lwFont.h"
#include "lwTinyFrame/lwUIImage.h"

namespace lw{
	Button1Def::Button1Def(const char* f, int uU, int uV, int dnU, int dnV, int dU, int dV, int ww, int hh, const char* fName){
		set(f, uU, uV, dnU, dnV, dU, dV, ww, hh, fName);
	}
	void Button1Def::set(const char* f, int uU, int uV, int dnU, int dnV, int dU, int dV, int ww, int hh, const char* fName){
		file = f;
		upU = uU;
		upV = uV;
		downU = dnU;
		downV = dnV;
		disableU = dU;
		disableV = dV;
		w = ww;
		h = hh;
		fontName = fName;
	}

	Button9Def::Button9Def(const char* f, int uU, int uV, int dnU, int dnV, int dU, int dV, int w1, int w2, int w3, int h1, int h2, int h3, const char* fName, GLfixed mmFilter){
		set(f, uU, uV, dnU, dnV, dU, dV, w1, w2, w3, h1, h2, h3, fName, mmFilter);
	}

	void Button9Def::set(const char* f, int uU, int uV, int dnU, int dnV, int dU, int dV, int w1, int w2, int w3, int h1, int h2, int h3, const char* fName, GLfixed mmFilter){
		file = f;
		upU = uU;
		upV = uV;
		downU = dnU;
		downV = dnV;
		disableU = dU;
		disableV = dV;
		uvW1 = w1;
		uvW2 = w2;
		uvW3 = w3;
		uvH1 = h1;
		uvH2 = h2;
		uvH3 = h3;
		fontName = fName;
		minMagFilter = mmFilter;
	}

	class UIButton1 : public UIButton{
	public:
		~UIButton1();
		virtual void vDraw();
		virtual void vSetPos(float x, float y);
		virtual void setColor(const lw::Color& color);
		virtual void setColor(const lw::Color& color, const lw::Color& downColor, const lw::Color& disableColor);

	public:
		UIButton1(const char* file, int upU, int upV, int downU, int downV, int w, int h, const char* fontName, bool& ok);
		UIButton1(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName, bool& ok);

	private:
		Sprite* _pSpriteUp;
		Sprite* _pSpriteDown;
		Sprite* _pSpriteDisable;
	};

	class UIButton9 : public UIButton{
	public:
		~UIButton9();

		virtual void vDraw();
		virtual void vSetPos(float x, float y);
		virtual void setColor(const lw::Color& color);
		virtual void setColor(const lw::Color& color, const lw::Color& downColor, const lw::Color& disableColor);
		void setSize(float w, float h);

	public:
		UIButton9(const char* file, int upU, int upV, int downU, int downV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, bool& ok);
		UIButton9(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter, bool& ok);

	private:
		Sprite9* _pSpriteUp;
		Sprite9* _pSpriteDown;
		Sprite9* _pSpriteDisable;
	};

	UIButton* UIButton::create1(const Button1Def& def){
		return create1(def.file, def.upU, def.upV, def.downU, def.downV, def.disableU, def.disableV, def.w, def.h, def.fontName);
	}

	UIButton* UIButton::create1(const char* file, int upU, int upV, int downU, int downV, int w, int h, const char* fontName){
		lwassert(file);
		bool ok = false;
		UIButton1* p = new UIButton1(file, upU, upV, downU, downV, w, h, fontName, ok);
		lwassert(p);
		if ( !ok ){
			lwerror("new UIButton1 failed: file="<<file);
			delete p;
			return NULL;
		}
		return p;
	}

	UIButton* UIButton::create1(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName){
		lwassert(file);
		bool ok = false;
		UIButton1* p = new UIButton1(file, upU, upV, downU, downV, disableU, disableV, w, h, fontName, ok);
		lwassert(p);
		if ( !ok ){
			lwerror("new UIButton1 failed: file="<<file);
			delete p;
			return NULL;
		}
		return p;
	}
	UIButton::UIButton()
	:_pCallback(NULL), _isTracing(false), _isDown(false), _gesId(-1)
	,_pFontUp(NULL), _pFontDown(NULL), _pFontDisable(NULL)
	,_x(0), _y(0),_topExt(0), _bottomExt(0), _leftExt(0), _rightExt(0){

	}

	UIButton::~UIButton(){
		if ( _pFontUp ){
			delete _pFontUp;
		}
		if ( _pFontDown ){
			delete _pFontDown;
		}
		if ( _pFontDisable ){
			delete _pFontDisable;
		}
	}

	void UIButton::setText(const wchar_t* text){
		lwassert(text);
		if ( !_pFontUp ){
			lwerror("no font");
			return;
		}
		_pFontUp->setText(text);
		_pFontDown->setText(text);
		_pFontDisable->setText(text);
	}

	void UIButton::setTextColor(const lw::Color& color){
		if ( _pFontUp ){
			_pFontUp->setColor(color);
		}
		if ( _pFontDown ){
			_pFontDown->setColor(color);
		}
		if ( _pFontDisable ){
			_pFontDisable->setColor(color);
		}
	}

	void UIButton::setTextColor(const lw::Color& color, const lw::Color& downColor, const lw::Color& disableColor){
		if ( _pFontUp ){
			_pFontUp->setColor(color);
		}
		if ( _pFontDown ){
			_pFontDown->setColor(downColor);
		}
		if ( _pFontDisable ){
			_pFontDisable->setColor(disableColor);
		}
	}

	bool UIButton::vGesture(const lw::Gesture& gesture){
		const lw::TouchEvent& evt = gesture.evt;
		switch (evt.type)
		{
		case lw::TouchEvent::TOUCH:
			if ( evt.x >= _x-_leftExt && evt.x < _x+_w+_rightExt 
			&& evt.y >= _y-_topExt && evt.y < _y+_h+_bottomExt ){
				_isTracing = true;
				_isDown = true;
				_gesId = gesture.id;
				if ( _pCallback ){
					_pCallback->onButtonDown(this);
				}
				g_gestrueMgr.deleteGesture(evt.x, evt.y);
				return true;
			}
			break;
		case lw::TouchEvent::UNTOUCH:{
				if ( _gesId == gesture.id ){
					_gesId = -1;
					bool isDown = _isDown;
					_isTracing = false;
					_isDown = false;
					if ( isDown && _pCallback ){
						_pCallback->vOnClick(this);
						_pCallback->onButtonUp(this);
						return true;
					}
				}
			}
			break;
		case lw::TouchEvent::MOVE:
			if ( _isTracing && _gesId == gesture.id ){
				if ( evt.x >= _x-_leftExt && evt.x < _x+_w+_rightExt 
					&& evt.y >= _y-_topExt && evt.y < _y+_h+_bottomExt ){
						_isDown = true;
				}else{
					if ( _isDown && _pCallback ){
						_pCallback->onButtonUp(this);
					}
					_isDown = false;
				}
				return true;
			}
			break;
		}
		return false;
	}

	UIButton1::UIButton1(const char* file, int upU, int upV, int downU, int downV, int w, int h, const char* fontName, bool& ok){
		lwassert(file);
		if ( fontName ){
			_pFontUp = lw::Font::create(fontName);
			if ( _pFontUp == NULL ){
				lwerror("lw::Font::create failed: fontName=" << fontName);
				return;
			}
			_pFontUp->setAlign(ALIGN_MID_MID);
			_pFontDown = lw::Font::create(fontName);
			_pFontDown->setAlign(ALIGN_MID_MID);
			_pFontDisable = lw::Font::create(fontName);
			_pFontDisable->setAlign(ALIGN_MID_MID);
		}
		_w = (float)w;
		_h = (float)h;

		_pSpriteUp = lw::Sprite::create(file);
		if ( _pSpriteUp == NULL ){
			lwerror("lw::Sprite::create file: " << file);
			return;
		}
		_pSpriteUp->setUV(upU, upV, w, h);
		_pSpriteDown = lw::Sprite::create(file);
		_pSpriteDown->setUV(downU, downV, w, h);
		_pSpriteDisable = lw::Sprite::create(file);
		_pSpriteDisable->setUV(upU, upV, w, h);

		ok = true;
	}

	UIButton1::UIButton1(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int w, int h, const char* fontName, bool& ok){
		lwassert(file);
		if ( fontName ){
			_pFontUp = lw::Font::create(fontName);
			if ( _pFontUp == NULL ){
				lwerror("lw::Font::create failed: fontName=" << fontName);
				return;
			}
			_pFontUp->setAlign(ALIGN_MID_MID);
			_pFontDown = lw::Font::create(fontName);
			_pFontDown->setAlign(ALIGN_MID_MID);
			_pFontDisable = lw::Font::create(fontName);
			_pFontDisable->setAlign(ALIGN_MID_MID);
		}
		_w = (float)w;
		_h = (float)h;

		_pSpriteUp = lw::Sprite::create(file);
		if ( _pSpriteUp == NULL ){
			lwerror("lw::Sprite::create file: " << file);
			return;
		}
		_pSpriteUp->setUV(upU, upV, w, h);
		_pSpriteDown = lw::Sprite::create(file);
		_pSpriteDown->setUV(downU, downV, w, h);
		_pSpriteDisable = lw::Sprite::create(file);
		_pSpriteDisable->setUV(disableU, disableV, w, h);

		ok = true;
	}

	UIButton1::~UIButton1(){
		if ( _pSpriteUp ){
			delete _pSpriteUp;
		}
		if ( _pSpriteDown ){
			delete _pSpriteDown;
		}
		if ( _pSpriteDisable ){
			delete _pSpriteDisable;
		}
	}

	void UIButton1::vSetPos(float x, float y){
		_x = x;
		_y = y;
		_pSpriteUp->setPos(x, y);
		_pSpriteDown->setPos(x, y);
		if ( _pSpriteDisable ){
			_pSpriteDisable->setPos(x, y);
		}
		float fx = _x+_w*.5f;
		float fy = _y+_h*.5f;
		if ( _pFontUp ){
			_pFontUp->setPos(fx, fy);
		}
		if ( _pFontDown ){
			_pFontDown->setPos(fx, fy);
		}
		if ( _pFontDisable ){
			_pFontDisable->setPos(fx, fy);
		}
	}

	void UIButton1::vDraw(){
		if ( _isEnable ){
			if ( _isDown ){
				_pSpriteDown->collect();
				if ( _pFontDown ){
					_pFontDown->collect();
				}
			}else{
				_pSpriteUp->collect();
				if ( _pFontUp ){
					_pFontUp->collect();
				}
			}
		}else{
			_pSpriteDisable->collect();
			if ( _pFontDisable ){
				_pFontDisable->collect();
			}else{
				if ( _pFontUp ){
					_pFontUp->collect();
				}
			}
		}
	}

	void UIButton1::setColor(const lw::Color& color){
		_pSpriteUp->setColor(color);
		_pSpriteDown->setColor(color);
		_pSpriteDisable->setColor(color);
	}
	void UIButton1::setColor(const lw::Color& color, const lw::Color& downColor, const lw::Color& disableColor){
		_pSpriteUp->setColor(color);
		_pSpriteDown->setColor(downColor);
		_pSpriteDisable->setColor(disableColor);
	}

	UIButton* UIButton::create9(const Button9Def& def){
		return create9(def.file, def.upU, def.upV, def.downU, def.downV, def.disableU, def.disableV, def.uvW1, def.uvW2, def.uvW3, def.uvH1, def.uvH2, def.uvH3, def.fontName, def.minMagFilter);
	}

	UIButton* UIButton::create9(const char* file, int upU, int upV, int downU, int downV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName){
		lwassert(file);
		bool ok = false;
		UIButton9* p = new UIButton9(file, upU, upV, downU, downV, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3, fontName, ok);
		lwassert(p);
		if ( !ok ){
			lwerror("new UIButton9 failed: file="<<file);
			delete p;
			return NULL;
		}
		return p;
	}

	UIButton* UIButton::create9(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter){
		lwassert(file);
		bool ok = false;
		UIButton9* p = new UIButton9(file, upU, upV, downU, downV, disableU, disableV, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3, fontName, minMagFilter, ok);
		lwassert(p);
		if ( !ok ){
			lwerror("new UIButton9 failed: file="<<file);
			delete p;
			return NULL;
		}
		return p;
	}

	UIButton9::UIButton9(const char* file, int upU, int upV, int downU, int downV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, bool& ok){
			lwassert(file);
			if ( fontName ){
				_pFontUp = lw::Font::create(fontName);
				if ( _pFontUp == NULL ){
					lwerror("lw::Font::create failed: fontName=" << fontName);
					return;
				}
				_pFontUp->setAlign(ALIGN_MID_MID);
				_pFontDown = lw::Font::create(fontName);
				_pFontDown->setAlign(ALIGN_MID_MID);
				_pFontDisable = lw::Font::create(fontName);
				_pFontDisable->setAlign(ALIGN_MID_MID);
			}
			_w = (float)uvW1+uvW2+uvW3;
			_h = (float)uvH1+uvH2+uvH3;

			_pSpriteUp = lw::Sprite9::create(file, upU, upV, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3);
			if ( _pSpriteUp == NULL ){
				lwerror("lw::Sprite::create file: " << file);
				return;
			}
			_pSpriteDown = lw::Sprite9::create(file, downU, downV, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3);
			_pSpriteDisable = lw::Sprite9::create(file, upU, upV, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3);

			ok = true;
	}

	UIButton9::UIButton9(const char* file, int upU, int upV, int downU, int downV, int disableU, int disableV, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, const char* fontName, GLfixed minMagFilter, bool& ok){
			lwassert(file);
			if ( fontName ){
				_pFontUp = lw::Font::create(fontName);
				if ( _pFontUp == NULL ){
					lwerror("lw::Font::create failed: fontName=" << fontName);
					return;
				}
				_pFontUp->setAlign(ALIGN_MID_MID);
				_pFontDown = lw::Font::create(fontName);
				_pFontDown->setAlign(ALIGN_MID_MID);
				_pFontDisable = lw::Font::create(fontName);
				_pFontDisable->setAlign(ALIGN_MID_MID);
			}
			_w = (float)uvW1+uvW2+uvW3;
			_h = (float)uvH1+uvH2+uvH3;

			_pSpriteUp = lw::Sprite9::create(file, upU, upV, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3);
			if ( _pSpriteUp == NULL ){
				lwerror("lw::Sprite::create file: " << file);
				return;
			}
			_pSpriteDown = lw::Sprite9::create(file, downU, downV, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3);
			_pSpriteDisable = lw::Sprite9::create(file, disableU, disableV, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3);

			_pSpriteUp->setFilter(GL_NEAREST, minMagFilter);
			_pSpriteDown->setFilter(GL_NEAREST, minMagFilter);
			_pSpriteDisable->setFilter(GL_NEAREST, minMagFilter);

			ok = true;
	}

	UIButton9::~UIButton9(){
		if ( _pSpriteUp ){
			delete _pSpriteUp;
		}
		if ( _pSpriteDown ){
			delete _pSpriteDown;
		}
		if ( _pSpriteDisable ){
			delete _pSpriteDisable;
		}
	}

	void UIButton9::vSetPos(float x, float y){
		_x = x;
		_y = y;
		_pSpriteUp->setPos(x, y);
		_pSpriteDown->setPos(x, y);
		if ( _pSpriteDisable ){
			_pSpriteDisable->setPos(x, y);
		}
		float fx = _x+_w*.5f;
		float fy = _y+_h*.5f;
		if ( _pFontUp ){
			_pFontUp->setPos(fx, fy);
		}
		if ( _pFontDown ){
			_pFontDown->setPos(fx, fy);
		}
		if ( _pFontDisable ){
			_pFontDisable->setPos(fx, fy);
		}
	}

	void UIButton9::setSize(float w, float h){
		_pSpriteUp->setSize(w, h);
		_pSpriteDown->setSize(w, h);
		_pSpriteDisable->setSize(w, h);
		_w = w;
		_h = h;
		float fx = _x+_w*.5f;
		float fy = _y+_h*.5f;
		if ( _pFontUp ){
			_pFontUp->setPos(fx, fy);
		}
		if ( _pFontDown ){
			_pFontDown->setPos(fx, fy);
		}
		if ( _pFontDisable ){
			_pFontDisable->setPos(fx, fy);
		}
	}

	void UIButton9::vDraw(){
		if ( _isEnable ){
			if ( _isDown ){
				_pSpriteDown->collect();
				if ( _pFontDown ){
					_pFontDown->collect();
				}
			}else{
				_pSpriteUp->collect();
				if ( _pFontUp ){
					_pFontUp->collect();
				}
			}
		}else{
			_pSpriteDisable->collect();
			if ( _pFontDisable ){
				_pFontDisable->collect();
			}else{
				if ( _pFontUp ){
					_pFontUp->collect();
				}
			}
		}
	}

	void UIButton9::setColor(const lw::Color& color){
		_pSpriteUp->setColor(color);
		_pSpriteDown->setColor(color);
		_pSpriteDisable->setColor(color);
	}
	void UIButton9::setColor(const lw::Color& color, const lw::Color& downColor, const lw::Color& disableColor){
		_pSpriteUp->setColor(color);
		_pSpriteDown->setColor(downColor);
		_pSpriteDisable->setColor(disableColor);
	}

} //namespace lw