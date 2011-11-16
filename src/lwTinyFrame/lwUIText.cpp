#include "stdafx.h"
#include "lwTinyFrame/lwUIText.h"
#include "lwTinyFrame/lwFont.h"

namespace lw{

	UIText* UIText::create(const char* fontName){
		lwassert(fontName);
		bool ok = false;
		UIText* pUIText = new UIText(fontName, ok);
		lwassert(pUIText);
		if ( !ok ){
			delete pUIText;
			return NULL;
		}
		return pUIText;
	}

	UIText::UIText(const char* fontName, bool& ok): _pFont(NULL){
		_pFont = Font::create(fontName);
		if ( _pFont == NULL ){
			lwerror("Font::create failed: file=" <<fontName);
			return;
		}
		ok = true;
	}

	UIText::~UIText(){
		if ( _pFont ){
			delete _pFont;
		}
	}

	void UIText::vSetPos(float x, float y){
		_pFont->setPos(x, y);
	}

	void UIText::vDraw(){
		_pFont->collect();
	}

} //namespace lw