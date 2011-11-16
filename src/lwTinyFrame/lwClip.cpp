#include "stdafx.h"
#include "lwTinyFrame/lwClip.h"
#include "lwTinyFrame/lwSprite.h"
#include "lwTinyFrame/lwFileSys.h"
#include "lwTinyFrame/lwTextureRes.h"

namespace lw{
	Clip* Clip::create(const char* fileName, int borderLeft, int borderRight, 
	int u0, int v0, int w, int h, float interval, int frameNum, bool loop){
		lwassert(fileName);
		lwassert(borderLeft < borderRight);
		lwassert( u0 >= borderLeft && u0+w <= borderRight );
		lwassert(frameNum > 0);
		bool ok = false;
		Clip* pClip = new Clip(fileName, borderLeft, borderRight, u0, v0, w, h, interval, frameNum, loop, ok);
		lwassert(pClip);
		if ( !ok ){
			delete pClip;
			return NULL;
		}
		return pClip;
	}

	Clip::Clip(const char* fileName, int borderLeft, int borderRight, 
	int u0, int v0, int w, int h, float interval, int frameNum, bool loop, bool& ok)
	:_pSprite(NULL), _bl(borderLeft), _br(borderRight), _u0(u0), _v0(v0), _w(w), _h(h), _interval(interval), _frameNum(frameNum), _loop(loop), _t(0){
		_pSprite = Sprite::create(fileName);
		if ( _pSprite == NULL ){
			lwerror("Sprite::create failed: fileName=" << fileName);
			return;
		}

		ok = true;
	}

	Clip::~Clip(){
		if ( _pSprite ){
			delete _pSprite;
		}
	}

	void Clip::main(float dt){
		if ( _interval >= 0 ){
			_t += dt;
		}else{
			_t -= dt;
		}
		
		float period = (_frameNum)*fabs(_interval);
		if ( _loop ){
			_t = fmod(_t, period);
		}else{
			_t = max(min(_t, period-0.001f), 0.f);
		}
		int currFrame = (int)(_t/fabs(_interval));

		int framesRow1 = (_br-_u0)/_w;
		unsigned short u = 0;
		unsigned short v = 0;
		if ( currFrame < framesRow1 ){
			u = _u0 + currFrame*_w;
			v = _v0;
		}else{
			int framesPerRow = (_br-_bl)/_w;
			int currFrameFromSecondRow = currFrame - framesRow1;
			int row = currFrameFromSecondRow/framesPerRow;
			int	col = currFrameFromSecondRow-row*framesPerRow;
			u = (col)*_w;
			v = _v0 + (row+1)*_h;
		}
		_pSprite->setUV(u, v, _w, _h);
	}

	void Clip::setFrame(int frame){
		lwassert(frame >= 0);
		_t = fabs(_interval)* frame;
	}

} //namespace lw
