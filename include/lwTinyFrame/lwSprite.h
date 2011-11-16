#ifndef __LW_SPRITE_H__
#define __LW_SPRITE_H__

namespace lw{

	class TextureRes;

	class Sprite{
	public:
		static Sprite* create(const char* texFileName, bool reserveData = false);
		~Sprite();
		void setUV(int u, int v, int w, int h);
		void setAnchor(float x, float y){
			if ( x != _ancX || y != _ancY ){
				_ancX = x; _ancY = y;
				_matrixNeedUpdate = true;
			}
		}
		void getAnchor(float& x, float& y){
			x = _ancX;
			y = _ancY;
		}
		void setPos(float x, float y){
			if ( x != _posX || y != _posY ){
				_posX = x;
				_posY = y;
				_matrixNeedUpdate = true;
			}
		}
		void getPos(float& x, float& y){
			x = _posX;
			y = _posY;
		}
		void setRotate(float r){
			if ( _rotate != r ){
				_rotate = r;
				_matrixNeedUpdate = true;
			}
		}
		float getRotate(){
			return _rotate;
		}
		void setScale(float x, float y){
			if ( _scaleX != x || _scaleY != y ){
				_scaleX = x;
				_scaleY = y;
				_matrixNeedUpdate = true;
			}
		}
		void getScale(float& x, float& y){
			x = _scaleX;
			y = _scaleY;
		}
		void setSize(float w, float h){
			setScale(w/_width, h/_height);
		}
		void getSize(float &w, float& h){
			w = _width*_scaleX;
			h = _height*_scaleY;
		}
		const TextureRes* getTextureRes(){
			return _pTextureRes;
		}
		unsigned short getWidth(){
			return _width;
		}
		unsigned short getHeight(){
			return _height;
		}
		void setColor(const Color& color){
			_color = color;
		}
		void setFilter(GLfixed minFilter, GLfixed magFilter);
		void collect();
		enum BlendMode{
			BLEND_NONE,
			BLEND_OVERLAY,
			BLEND_ADD,
		};
		static void flush(BlendMode bm = BLEND_OVERLAY);
		const cml::Matrix44& getMatrix();

		struct Vertex{
			float x, y, z;
			float u, v;
		};

	private:
		Sprite(const char* texFileName, bool reserveData, bool &ok);
		void updateMatrix();

	private:
		TextureRes* _pTextureRes;
		float _u1, _v1, _u2, _v2;
		int _width, _height;
		int _texW, _texH;
		GLuint _glId;
		bool _hasAlpha;
		Color _color;

		bool _matrixNeedUpdate;
		cml::Matrix44 _matrix;
		float _ancX, _ancY;
		float _posX, _posY;
		float _rotate;
		float _scaleX, _scaleY;
	};

	class Sprite9
	{
	public:
		static Sprite9* create(const char* fileName, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3);
		~Sprite9();
		void setPos(float x, float y);
		void setSize(float w, float h);
		void setColor(const lw::Color& color);
		void setFilter(GLfixed minFilter, GLfixed magFilter);
		void collect();
		
	private:
		Sprite9(const char* fileName, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, bool& ok);
		Sprite* _pSprite;
		void update();
		int _uvX[3], _uvY[3], _uvW[3], _uvH[3];
		float _x[3], _y[3], _w[3], _h[3];
		float _width, _height;
		bool _needUpdate;
	};
	

} //namespace lw

#endif //__LW_SPRITE_H__