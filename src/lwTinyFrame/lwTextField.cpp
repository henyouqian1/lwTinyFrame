#include "stdafx.h"
#include "lwTinyFrame/lwTextField.h"
#include "lwTinyFrame/lwApp.h"
#include "lwTinyFrame/lwUtil.h"


namespace lw{

	struct TextField::Data{
		HWND hwnd;
		std::string str;
	};

	TextField::TextField(int x, int y, int w, int h){
		_pd = new Data;

		HWND hwnd = lw::App::s().getHwnd();
		_pd->hwnd = CreateWindow(L"EDIT",      // predefined class 
			NULL,        // no window title 
			WS_CHILD | WS_VISIBLE | 
			ES_LEFT | ES_AUTOHSCROLL, 
			x, y, w, h,  // set size in WM_SIZE message 
			hwnd,        // parent window 
			(HMENU) 123,   // edit control ID 
			(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);       // pointer not needed 

		// Add text to the window. 
		ShowWindow(_pd->hwnd, SW_SHOW);
	}
	TextField::~TextField(){
		DestroyWindow(_pd->hwnd);
		delete _pd;
	}
	const char* TextField::getUtf8(){
		wchar_t buf[1024];
		GetWindowText(_pd->hwnd, buf, 1023);
		W2UTF8 utf8(buf);
		_pd->str = utf8.data();
		return _pd->str.c_str();
	}
	void TextField::setPlaceholder(const char* text){

	}
	void TextField::setAlign(Align align){

	}
	void TextField::setText(const char* utf8){
		UTF82W w(utf8);
		SendMessage(_pd->hwnd, WM_SETTEXT, 0, (LPARAM)w.data());
	}
	void TextField::setTextColor(const Color& color){

	}
	void TextField::setBorderStyle(BorderStyle bs){

	}
	void TextField::setKeyboardType(KeyboardType kt){

	}
	void TextField::setAutocorrectionType(AutocorrectionType type){

	}
	void TextField::setAutocapitalizationType(AutocapitalizationType type){

	}
	void TextField::setClearButtonMode(ClearButtonMode vm){

	}
	void TextField::show(bool s){
		if ( s ){
			ShowWindow(_pd->hwnd, SW_SHOW);
		}else{
			ShowWindow(_pd->hwnd, SW_HIDE);
		}
	}
	void TextField::hideKeyboard(){

	}

} //namespace lw