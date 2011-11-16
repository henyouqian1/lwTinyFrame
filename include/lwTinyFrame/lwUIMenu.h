#ifndef __LW_UI_MENU_H__
#define __LW_UI_MENU_H__

namespace lw{

	class Font;
	struct TouchEvent;

	class Menu{
	public:
		static Menu* create(const char* fontFile);
		~Menu();
		void addItem(const wchar_t* text, float x, float y, void* data);

		typedef void (*CallbackFn) (int itemIdx, void*);
		void callback(CallbackFn fn){
			_callbackFn = fn;
		}

		void collect();
		bool onTouchEvent(std::vector<lw::TouchEvent>& events);

	private:
		Menu(const char* fontFile, bool& ok);
		
	private:
		CallbackFn _callbackFn;
		std::string _fontFile;
		struct Item {
			float x, y, w, h;
			Font* pFont;
			void* data;
		};
		std::vector<Item> _items;
		int _trackIndex;
		bool _trackIsDown;
	};

} //namespace lw



#endif //__LW_UI_MENU_H__