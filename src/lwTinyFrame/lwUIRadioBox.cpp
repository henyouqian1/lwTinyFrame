#include "stdafx.h"
#include "lwTinyFrame/lwUIRadioBox.h"
#include "lwTinyFrame/lwTouchEvent.h"

namespace lw{
	
	UIRadioBoxGrp::UIRadioBoxGrp(CheckBoxCallback* pCallback)
	:_pCallback(pCallback){
		
	}

	UIRadioBoxGrp::~UIRadioBoxGrp(){
		
	}

	void UIRadioBoxGrp::add(lw::UICheckBox* pCb){
		_checkBoxs.push_back(pCb);
		pCb->setCheckOnly();
		pCb->setCallback(this);
		pCb->setParent(this);
	}

	void UIRadioBoxGrp::vOnCheck(lw::UICheckBox* pCb){
		uncheckAll();
		pCb->check(true);
		_pCallback->vOnCheck(pCb);
	}

	void UIRadioBoxGrp::uncheckAll(){
		std::vector<lw::UICheckBox*>::iterator it = _checkBoxs.begin();
		std::vector<lw::UICheckBox*>::iterator itEnd = _checkBoxs.end();
		for ( ; it != itEnd; ++it ){
			(*it)->check(false);
		}
	}

	void UIRadioBoxGrp::hideAllCheckBox(){
		std::vector<lw::UICheckBox*>::iterator it = _checkBoxs.begin();
		std::vector<lw::UICheckBox*>::iterator itEnd = _checkBoxs.end();
		for ( ; it != itEnd; ++it ){
			(*it)->show(false);
		}
	}

	void UIRadioBoxGrp::deleteAllCheckBox(){
		std::vector<lw::UICheckBox*>::iterator it = _checkBoxs.begin();
		std::vector<lw::UICheckBox*>::iterator itEnd = _checkBoxs.end();
		for ( ; it != itEnd; ++it ){
			delete (*it);
		}
		_checkBoxs.clear();
	}


	
} //namespace lw