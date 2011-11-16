#include "stdafx.h"
#include "lwTinyFrame/lwUIMenu.h"
#include "lwTinyFrame/lwFont.h"
#include "lwTinyFrame/lwTouchEvent.h"

namespace lw{

	Menu* Menu::create(const char* fontFile){
		lwassert(fontFile);
		bool ok = false;
		Menu* p = new Menu(fontFile, ok);
		lwassert(p);
		if ( !ok ){
			delete p;
			return NULL;
		}
		return p;
	}

	Menu::Menu(const char* fontFile, bool& ok)
	:_callbackFn(NULL), _trackIndex(-1), _trackIsDown(false){
		ok = false;
		_fontFile = fontFile;
		Font* pFont = Font::create(fontFile);
		if ( pFont == NULL ){
			lwerror("Font::create failed: fontFile=" << fontFile);
			return;
		}
		delete pFont;
		ok = true;
	}
	Menu::~Menu(){
		std::vector<Item>::iterator it = _items.begin();
		std::vector<Item>::iterator itEnd = _items.end();
		for ( ; it != itEnd; ++it ){
			delete it->pFont;
		}
	}
	void Menu::addItem(const wchar_t* text, float x, float y, void* data){
		lwassert(text);
		Item item;
		item.pFont = Font::create(_fontFile.c_str());
		item.pFont->setText(text);
		item.pFont->setPos(x, y);
		item.x = x;
		item.y = y;
		item.pFont->getSize(item.w, item.h);
		item.data = data;
		_items.push_back(item);
	}

	void Menu::collect(){
		std::vector<Item>::iterator it =  _items.begin();
		std::vector<Item>::iterator itEnd =  _items.end();
		lw::Color color1 = lw::COLOR_WHITE;
		lw::Color color2(1.f, 0.2f, 0.f, 1.f);
		for ( int i = 0; it != itEnd; ++it, ++i ){
			if ( _trackIndex == i && _trackIsDown ){
				it->pFont->setColor(color2);
			}else{
				it->pFont->setColor(color1);
			}
			it->pFont->collect();
		}
	}

	bool Menu::onTouchEvent(std::vector<lw::TouchEvent>& events){
		lw::TouchEvent& evt = events[0];
		std::vector<Item>::iterator it = _items.begin();
		std::vector<Item>::iterator itEnd = _items.end();
		switch(evt.type)
		{
		case lw::TouchEvent::TOUCH:
			_trackIndex = -1;
			_trackIsDown = false;
			for ( int i = 0; it != itEnd; ++it, ++i ){
				if ( evt.x >= it->x && evt.x < it->x+it->w
				&& evt.y >= it->y && evt.y < it->y+it->h ){
					_trackIndex = i;
					_trackIsDown = true;
					break;
				}
			}
			break;
		case lw::TouchEvent::UNTOUCH:
			if ( _trackIndex >= 0 && _trackIsDown && _callbackFn ){
				_callbackFn(_trackIndex, _items[_trackIndex].data);
			}
			_trackIndex = -1;
			_trackIsDown = false;
			break;
		case lw::TouchEvent::MOVE:
			if ( _trackIndex >= 0 ){
				Item& item = _items[_trackIndex];
				if ( evt.x >= item.x && evt.x < item.x+item.w
				&& evt.y >= item.y && evt.y < item.y+item.h ){
					_trackIsDown = true;
				}else{
					_trackIsDown = false;
				}
			}
			break;
		default:
			break;
		}
		return false;
	}

} //namespace lw