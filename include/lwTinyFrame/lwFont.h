#ifndef __LW_FONT_H__
#define __LW_FONT_H__

namespace lw{
	
	class FontRes;
	class Font;

	enum FontAlign{
		ALIGN_TOP_LEFT,
		ALIGN_TOP_MID,
		ALIGN_TOP_RIGHT,
		ALIGN_MID_LEFT,
		ALIGN_MID_MID,
		ALIGN_MID_RIGHT,
		ALIGN_BOTTOM_LEFT,
		ALIGN_BOTTOM_MID,
		ALIGN_BOTTOM_RIGHT,
	};

	class Font{
	public:
		static Font* create(const char* fileName);
		~Font();

		void setPos(float x, float y){
			_posX = x;
			_posY = y;
			_needUpdate = true;
		}
		void setAlign(FontAlign align){
			_align = align;
			_needUpdate = true;
		}
		void setText(const wchar_t* text){
			_text = text;
			_needUpdate = true;
		}
		const wchar_t* getText(){
			return _text.c_str();
		}
		void setRotate(float rotate){
			_rotate = rotate;
			_needUpdate = true;
		}
		void setScale(float scaleX, float scaleY){
			_scaleX = scaleX;
			_scaleY = scaleY;
			_needUpdate = true;
		}
		void setColor(const Color& color){
			_color = color;
		}

		FontRes* getFontRes(){
			return _pRes;
		}
		int getH();
		void collect();
		void getSize(float& w, float& h);
		void getPos(float& x, float& y){
			x = _posX;
			y = _posY;
		}

	private:
		Font(FontRes* pRes);
		void update();

	private:
		FontRes* _pRes;
		float _posX, _posY;
		std::wstring _text;
		FontAlign _align;
		std::vector<float> _linesOffset;
		float _scaleX, _scaleY;
		float _rotate;
		Color _color;
		bool _needUpdate;
		float _width, _height;
		float _y0;

	public:
		static void quit();
	};
	

} //namespace lw

#endif //__LW_FONT_H__