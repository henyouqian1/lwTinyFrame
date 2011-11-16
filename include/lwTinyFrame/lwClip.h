#ifndef __LW_CLIP_H__
#define __LW_CLIP_H__

namespace lw{

	class Sprite;

	class Clip{
	public:
		static Clip* create(const char* fileName, int borderLeft, int borderRight, 
			int u0, int v0, int w, int h, float interval, int frameNum, bool loop);
		~Clip();
		Sprite* getSprite(){
			return _pSprite;
		}
		void main(float dt);
		void setFrame(int frame);

	private:
		Clip(const char* fileName, int borderLeft, int borderRight, 
			int u0, int v0, int w, int h, float interval, int frameNum, bool loop, bool& ok);
		Sprite* _pSprite;
		int _bl, _br;
		int _u0, _v0, _w, _h;
		float _interval;
		int _frameNum;
		bool _loop;
		float _t;
	};

} //namespace lw

#endif //__LW_CLIP_H__