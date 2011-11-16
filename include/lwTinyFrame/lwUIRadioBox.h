#ifndef __LW_UI_RADIOBOX_H__
#define __LW_UI_RADIOBOX_H__

#include "lwTinyFrame/lwUICheckbox.h"

namespace lw{

	class UIRadioBoxGrp : public UIWidget, public CheckBoxCallback{
	public:
		UIRadioBoxGrp(CheckBoxCallback* pCallback);
		~UIRadioBoxGrp();
		void add(lw::UICheckBox* pCb);
		void hideAllCheckBox();
		void deleteAllCheckBox();
		void uncheckAll();

	private:
		virtual void vOnCheck(lw::UICheckBox* pCb);
		CheckBoxCallback* _pCallback;
		std::vector<lw::UICheckBox*> _checkBoxs;
	};
	

} //namespace lw



#endif //__LW_UI_RADIOBOX_H__