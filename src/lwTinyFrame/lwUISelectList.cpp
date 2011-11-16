#include "stdafx.h"
#include "lwTinyFrame/lwUISelectList.h"
#include "lwTinyFrame/lwSprite.h"
#include "lwTinyFrame/lwUIImage.h"
#include "lwTinyFrame/lwTouchEvent.h"

namespace lw{


	UISelectList* UISelectList::create(const char* bgFile, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, int marginH, int marginV, int gapH){
		lwassert(bgFile);
		bool ok = false;
		UISelectList* pUIList = new UISelectList(bgFile, uvX, uvY, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3, marginH, marginV, gapH, ok);
		lwassert(pUIList);
		if ( !ok ){
			delete pUIList;
			return NULL;
		}
		return pUIList; 
	}

	UISelectList::UISelectList(const char* bgFile, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, int marginH, int marginV, int gapH, bool& ok)
	:_marginH(marginH), _marginV(marginV), _gapH(gapH), _isDown(false), _headIndex(0), _headOffset(0.f), _snapIndex(0), _isDrag(false)
	,_selectIndex(-1), _pImageBG(NULL), _pImageSelectFrame(NULL), _pCallback(NULL)/*, _pLine(NULL)*/{
		_pImageBG = lw::UIImage9::create(bgFile, uvX, uvY, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3);
		if ( _pImageBG == NULL ){
			lwerror("lw::UIImage9::create failed: bgFile=" <<bgFile);
			return;
		}
		_pImageBG->setParent(this);

		_pImageSelectFrame = lw::UIImage9::create("ui1.png", 56, 41, 3, 2, 3, 3, 2, 3);
		if ( _pImageSelectFrame == NULL ){
			lwerror("lw::UIImage9::create failed: file=ui1.png");
			return;
		}
		_pImageSelectFrame->setParent(NULL);

		//_pLine = Sprite::create("ui1.png");
		//lwassert(_pLine);
		//_pLine->setUV(255, 0, 1, 1);

		ok = true;
	}

	UISelectList::~UISelectList(){
		std::vector<ListItem>::iterator it = _items.begin();
		std::vector<ListItem>::iterator itEnd = _items.end();
		for ( ; it != itEnd; ++it ){
			delete it->pSprite;
		}
		if ( _pImageSelectFrame){
			delete _pImageSelectFrame;
		}
		//if ( _pLine ){
		//	delete _pLine;
		//}
	}

	void UISelectList::setSize(int w, int h){
		Widget::setSize(w, h);
		_pImageBG->setSize(w, h);
	}

	void UISelectList::vMain(float dt){
		//lwinfo(_snapIndex);
		//if ( !_isDown && _headOffset != 0 ){
		//	if ( _snapIndex == _headIndex ){
		//		_headOffset -= 0.05f*dt/**max(0.05f, _headOffset)*/;
		//		_headOffset = max(0.f, _headOffset);
		//	}else{
		//		ListItem& item = _items[_headIndex];
		//		int currW = (int)((item.pSprite->getW())*item.scale)+_gapH;
		//		_headOffset += 0.05f*dt/**max(0.05f, (currW-_headOffset))*/;
		//		if ( _headOffset >= (float)currW ){
		//			++_headIndex;
		//			if ( _headIndex == (int)_items.size() ){
		//				_headIndex = 0;
		//			}
		//			_headOffset = 0;
		//		}
		//	}
		//}
	}

	void UISelectList::vDrawFG(){
		if ( _items.empty() ){
			return;
		}
		int idx = _headIndex - 1;
		if ( idx < 0 ){
			idx = (int)_items.size() - 1;
		}
		ListItem& item = _items[idx];
		int x = _x + _marginH - (int)_headOffset - (int)(item.pSprite->getW()*item.scale) - _gapH/2 ;
		int y = _y + _marginV;
		cml::Matrix44 m;
		while ( x < _w ){
			ListItem& item = _items[idx];
			cml::matrix_scale(m, item.scale, item.scale, item.scale);
			cml::matrix_set_translation(m, (float)x, (float)y, 0.f);
			item.pSprite->collect(m);
			if ( idx == _selectIndex ){
				_pImageSelectFrame->setScreenPos(x, _y);
				_pImageSelectFrame->setSize((int)(item.pSprite->getW()*item.scale), _h);
				_pImageSelectFrame->vDraw();
			}
			x += (int)(item.pSprite->getW()*item.scale + _gapH);
			++idx;
			if ( idx == (int)_items.size() ){
				idx = 0;
			}
		}
	}

	bool UISelectList::vEvent(std::vector<lw::TouchEvent>& events){
		const lw::TouchEvent& evt = events[0];

		if ( _items.empty() ){
			if ( evt.x >= _x && evt.x < _x + _w && evt.y >= _y && evt.y < _y +_h ){
				return true;
			}else{
				return false;
			}
		}
		
		switch (evt.type)
		{
		case TouchEvent::TOUCH:
			if ( evt.x >= _x && evt.x < _x + _w && evt.y >= _y && evt.y < _y +_h ){
				_isDown = true;
				_downX = evt.x;
				return true;
			}
			break;
		case TouchEvent::UNTOUCH:
			if ( _isDown && !_isDrag && evt.x >= _x && evt.x < _x + _w && evt.y >= _y && evt.y < _y +_h ){
				int idx = _headIndex - 1;
				if ( idx < 0 ){
					idx = (int)_items.size() - 1;
				}
				ListItem& item = _items[idx];
				int x = _x + _marginH - (int)_headOffset - (int)(item.pSprite->getW()*item.scale) - _gapH/2 ;
				//int y = _y + _marginV;
				while ( x < _w ){
					ListItem& item = _items[idx];
					if ( evt.x >= x && evt.x < x + item.pSprite->getW()*item.scale ){
						if ( _selectIndex == idx ){
							if ( _pCallback ){
								_pCallback->onSelectListConfirm(this);
							}
						}else{
							_selectIndex = idx;
							if ( _pCallback ){
								_pCallback->onSelectListSelect(this);
							}
						}
						return true;
					}
					x += (int)(item.pSprite->getW()*item.scale + _gapH);
					++idx;
					if ( idx == (int)_items.size() ){
						idx = 0;
					}
				}
			}
			_isDown = false;
			_isDrag = false;
			break;
		case TouchEvent::MOVE:
			if ( _isDown ){
				if ( !_isDrag ){
					if ( abs(_downX - evt.x) > 10 ){
						_isDrag = true;
					}
				}
				if ( _isDrag ){
					_headOffset -= (float)(evt.x - evt.prevX);

					ListItem& item = _items[_headIndex];
					int currW = (int)((item.pSprite->getW())*item.scale)+_gapH;
					if ( _headOffset < 0 ){
						int prevIndex = _headIndex - 1;
						if ( prevIndex < 0 ){
							prevIndex = (int)_items.size()-1;
						}
						ListItem& prevItem = _items[prevIndex];
						int prevW = (int)((prevItem.pSprite->getW())*prevItem.scale)+_gapH;
						_headIndex = prevIndex;
						_headOffset = (float)prevW + _headOffset;
					}else{

						if ( _headOffset >= (float)currW ){
							++_headIndex;
							if ( _headIndex == (int)_items.size() ){
								_headIndex = 0;
							}
							_headOffset -= (float)currW;
						}
					}

					if ( _headOffset > (float)(currW>>1) ){
						_snapIndex = _headIndex + 1;
						if ( _snapIndex == (int)_items.size() ){
							_snapIndex = 0;
						}
					}else{
						_snapIndex = _headIndex;
					}
				}
				return true;
			}
			break;
		default:
			break;
		}
		
		return false;
	}

	void UISelectList::clearItem(){
		std::vector<ListItem>::iterator it = _items.begin();
		std::vector<ListItem>::iterator itEnd = _items.end();
		for ( ; it != itEnd; ++it ){
			delete it->pSprite;
		}
		_items.clear();
		reset();
	}

	void UISelectList::reset(){
		_isDown = false;
		_isDrag = false;
		_headIndex = 0;
		_headOffset = 0;
		_snapIndex = 0;
		_selectIndex = -1;
	}

	void UISelectList::addItem(const char* imageFile, int u, int v, int w, int h, int count, void* pData){
		lwassert(imageFile);
		ListItem item;
		item.pSprite = lw::Sprite::create(imageFile);
		if ( item.pSprite == NULL ){
			lwerror("lw::Sprite::create failed: imageFile=" << imageFile);
			return;
		}
		item.pSprite->setUV(u, v, w, h);
		item.scale = (float)(_h-2.f*_marginV)/item.pSprite->getH();
		item.count = count;
		item.data = pData;
		_items.push_back(item);
	}

	void UISelectList::select(int index, bool invokeCallback){
		if ( index < 0 || index >= (int)_items.size() ){
			lwerror("invalid index");
			return;
		}
		_selectIndex = index;
		if ( invokeCallback && _pCallback ){
			_pCallback->onSelectListSelect(this);
		}
	}

	void* UISelectList::getSelectedItemData(){
		if ( _selectIndex < 0 ){
			return NULL;
		}
		return _items[_selectIndex].data;
	}

} //namespace lw