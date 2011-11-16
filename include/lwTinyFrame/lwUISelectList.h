#ifndef __LW_UI_SELECT_LIST_H__
#define __LW_UI_SELECT_LIST_H__

#include "lwUIWidget.h"

namespace lw{

	class Sprite;
	class UIImage9;
	class UISelectList;

	class SelectListCallback{
	public:
		virtual ~SelectListCallback(){}
		virtual void onSelectListSelect(lw::UISelectList* pList){}
		virtual void onSelectListConfirm(lw::UISelectList* pList){}
	};

	class UISelectList : public Widget{
	public:
		static UISelectList* create(const char* bgFile, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3
									, int marginH, int marginV, int gapH);
		~UISelectList();

		virtual void setSize(int w, int h);
		virtual void vMain(float dt);
		virtual void vDrawFG();
		virtual bool vEvent(std::vector<lw::TouchEvent>& events);

		void addItem(const char* imageFile, int u, int v, int w, int h, int count, void* pData);
		void clearItem();
		void setCallback(SelectListCallback* pCallback){
			_pCallback = pCallback;
		}
		void reset();

		void select(int index, bool invokeCallback);
		int getSelect(){
			return _selectIndex;
		}
		
		void* getSelectedItemData();

		

	private:
		struct ListItem{
			lw::Sprite* pSprite;
			float scale;
			int count;
			void* data;
		};

		UISelectList(const char* bgFile, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, int marginH, int marginV, int gapH, bool& ok);
		UIImage9* _pImageBG;
		UIImage9* _pImageSelectFrame;
		//Sprite* _pLine;
		int _marginH, _marginV;
		int _gapH;
		std::vector<ListItem> _items;
		bool _isDown;
		int _downX;
		int _headIndex;
		float _headOffset;
		int _snapIndex;
		bool _isDrag;
		int _selectIndex;
		SelectListCallback* _pCallback;
	};

} //namespace lw



#endif //__LW_UI_SELECT_LIST_H__