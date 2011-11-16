#ifndef __LW_TEXTFIELD_H__
#define __LW_TEXTFIELD_H__

namespace lw{
	
	class TextField{
	public:
		enum Align{
			ALIGN_LEFT,
			ALIGN_CENTER,
			ALIGN_RIGHT,
		};
		enum BorderStyle{
			BORDER_STYLE_NONE,
			BORDER_STYLE_LINE,
			BORDER_STYLE_BEZEL,
			BORDER_STYLE_ROUNDEDRECT
		};
		enum KeyboardType {
			KT_Default = 0,            // Default type for the current input method.
			KT_ASCIICapable,           // Displays a keyboard which can enter ASCII characters, non-ASCII keyboards remain active
			KT_NumbersAndPunctuation,  // Numbers and assorted punctuation.
			KT_URL,                    // A type optimized for URL entry (shows . / .com prominently).
			KT_NumberPad,              // A number pad (0-9). Suitable for PIN entry.
			KT_PhonePad,               // A phone pad (1-9, *, 0, #, with letters under the numbers).
			KT_NamePhonePad,           // A type optimized for entering a person's name or phone number.
			KT_EmailAddress,
		};
		enum AutocorrectionType{
			ACO_Default,
			ACO_No,
			ACO_Yes,
		};
		enum AutocapitalizationType {
			ACP_None,
			ACP_Words,
			ACP_Sentences,
			ACP_AllCharacters,
		};
		enum ClearButtonMode{
			CBM_Never,
			CBM_WhileEditing,
			CBM_UnlessEditing,
			CBM_ModeAlways
		};		
		
		TextField(int x, int y, int w, int h);
		~TextField();
		const char* getUtf8();
		void setPlaceholder(const char* utf8);
		void setAlign(Align align);
		void setText(const char* utf8);
		void setTextColor(const Color& color);
		void setBorderStyle(BorderStyle bs);
		void setKeyboardType(KeyboardType kt);
		void setAutocorrectionType(AutocorrectionType type);
		void setAutocapitalizationType(AutocapitalizationType type);
		void setClearButtonMode(ClearButtonMode vm);
		void show(bool s);
		
		void hideKeyboard();

	private:
		struct Data;
		Data* _pd;
	};

} //namespace lw

#endif //__LW_TEXTFIELD_H__