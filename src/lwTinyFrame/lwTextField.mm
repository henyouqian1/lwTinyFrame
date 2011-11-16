#include "stdafx.h"
#include "lwTextField.h"
#include "lwApp.h"
#import "UIKit/UITextField.h"
#import <UIKit/UIApplication.h>

namespace lw{

	struct TextField::Data{
		UITextField* pTextField;
		
	};

	TextField::TextField(int x, int y, int w, int h){
		_pd = new Data;
		
		const lw::App::Config& conf = lw::App::s().getConfig();
		if ( conf.orientation == lw::App::ORIENTATION_LEFT ){
			_pd->pTextField = [[UITextField alloc] initWithFrame:CGRectMake(320-(y+h/2)-w/2, x+w/2-h/2, w, h)];
			_pd->pTextField.transform = CGAffineTransformMakeRotation(-M_PI * 0.5);
		}else if ( conf.orientation == lw::App::ORIENTATION_RIGHT ){
			_pd->pTextField = [[UITextField alloc] initWithFrame:CGRectMake(320-(y+h/2)-w/2, x+w/2-h/2, w, h)];
			_pd->pTextField.transform = CGAffineTransformMakeRotation(M_PI * 0.5);
		}else {
			_pd->pTextField = [[UITextField alloc] initWithFrame:CGRectMake(x, y, w, h)];
		}

		
		_pd->pTextField.borderStyle = UITextBorderStyleRoundedRect;
		[[UIApplication sharedApplication].keyWindow addSubview:_pd->pTextField];
	}
	TextField::~TextField(){
		[_pd->pTextField removeFromSuperview];
		[_pd->pTextField release];
		delete _pd;
	}
	const char* TextField::getUtf8(){
		return [_pd->pTextField.text UTF8String];
	}
	void TextField::setPlaceholder(const char* utf8){
		NSString* str = [NSString stringWithFormat:@"%s", utf8];
		_pd->pTextField.placeholder = str;
	}
	void TextField::setAlign(Align align){
		if ( align == ALIGN_LEFT ){
			_pd->pTextField.textAlignment = UITextAlignmentLeft;
		}else if ( align == ALIGN_CENTER ){
			_pd->pTextField.textAlignment = UITextAlignmentCenter;
		}else if ( align == ALIGN_RIGHT ){
			_pd->pTextField.textAlignment = UITextAlignmentRight;
		}
	}
	void TextField::setText(const char* utf8){
		NSString* str = [NSString stringWithFormat:@"%s", utf8];
		_pd->pTextField.text = str;
	}
	void TextField::setTextColor(const Color& color){
		_pd->pTextField.textColor = [UIColor colorWithRed:(float)color.r/255.0 green:(float)color.g/255.0 blue:(float)color.b/255.0 alpha:(float)color.a/255.0];
	}
	void TextField::setBorderStyle(BorderStyle bs){
		if ( bs == BORDER_STYLE_NONE ){
			_pd->pTextField.borderStyle = UITextBorderStyleNone;
		}else if ( bs == BORDER_STYLE_LINE ){
			_pd->pTextField.borderStyle = UITextBorderStyleLine;
		}else if ( bs == BORDER_STYLE_BEZEL ){
			_pd->pTextField.borderStyle = UITextBorderStyleBezel;
		}else if ( bs == BORDER_STYLE_ROUNDEDRECT ){
			_pd->pTextField.borderStyle = UITextBorderStyleRoundedRect;
		}
	}
	void TextField::setKeyboardType(KeyboardType kt){
		UIKeyboardType a[] = {
			UIKeyboardTypeDefault,
			UIKeyboardTypeASCIICapable,
			UIKeyboardTypeNumbersAndPunctuation,
			UIKeyboardTypeURL,
			UIKeyboardTypeNumberPad,
			UIKeyboardTypePhonePad,
			UIKeyboardTypeNamePhonePad,
			UIKeyboardTypeEmailAddress,
		};
		_pd->pTextField.keyboardType = a[kt];
	}
	void TextField::setAutocorrectionType(AutocorrectionType t){
		UITextAutocorrectionType a[] = {
			UITextAutocorrectionTypeDefault,
			UITextAutocorrectionTypeNo,
			UITextAutocorrectionTypeYes,
		};
		_pd->pTextField.autocorrectionType = a[t];
	}
	void TextField::setAutocapitalizationType(AutocapitalizationType t){
		UITextAutocapitalizationType a[] = {
			UITextAutocapitalizationTypeNone,
			UITextAutocapitalizationTypeWords,
			UITextAutocapitalizationTypeSentences,
			UITextAutocapitalizationTypeAllCharacters,
		};
		_pd->pTextField.autocapitalizationType = a[t];
	}
	void TextField::setClearButtonMode(ClearButtonMode cbm){
		UITextFieldViewMode a[] ={
			UITextFieldViewModeNever,
			UITextFieldViewModeWhileEditing,
			UITextFieldViewModeUnlessEditing,
			UITextFieldViewModeAlways
		};
		_pd->pTextField.clearButtonMode = a[cbm];
	}
	void TextField::show(bool s){
		_pd->pTextField.hidden = !s;
	}
	void TextField::hideKeyboard(){
		[_pd->pTextField resignFirstResponder];
	}

} //namespace lw