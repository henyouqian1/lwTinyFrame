#include "stdafx.h"
#include "lwTinyFrame/lwTouchEvent.h"

namespace lw{
	void GestureMgr::main(){
		std::list<Gesture>::iterator git = _gestures.begin();
		std::list<Gesture>::iterator gitEnd = _gestures.end();
		for ( ; git != gitEnd; ){
			git->updated = false;
			if ( git->evt.type == TouchEvent::UNTOUCH ){
				git = _gestures.erase(git);
				gitEnd = _gestures.end();
			}else{
				++git;
			}
		}
	}

	void GestureMgr::onTouchEvent(const std::vector<lw::TouchEvent>& events){
		std::list<Gesture>::iterator git = _gestures.begin();
		std::list<Gesture>::iterator gitEnd = _gestures.end();
		//for ( ; git != gitEnd; ){
		//	git->updated = false;
		//	if ( git->evt.type == TouchEvent::UNTOUCH ){
		//		git = _gestures.erase(git);
		//		gitEnd = _gestures.end();
		//	}else{
		//		++git;
		//	}
		//}

		std::vector<lw::TouchEvent>::const_iterator it = events.begin();
		std::vector<lw::TouchEvent>::const_iterator itEnd = events.end();
		for ( ; it != itEnd; ++it ){
			switch ( it->type )
			{
			case TouchEvent::TOUCH:{
					Gesture g(_currGestureId, *it);
					_gestures.push_back(g);
					++_currGestureId;
					break;
				}
			case TouchEvent::UNTOUCH:{
					git = _gestures.begin();
					gitEnd = _gestures.end();
					for ( ; git != gitEnd; ++git ){
						if ( (it->prevX == git->evt.x && it->prevY == git->evt.y)
						||(it->x == git->evt.x && it->y == git->evt.y)){
							git->evt = *it;
							git->updated = true;
						}
					}
					break;
				}
			case TouchEvent::MOVE:{
					git = _gestures.begin();
					gitEnd = _gestures.end();
					for ( ; git != gitEnd; ++git ){
						if ( (it->prevX == git->evt.x && it->prevY == git->evt.y)){
							git->evt = *it;
							git->updated = true;
						}
					}
					break;
				}
			default:
				lwassert(0);
				break;
			}
		}
	}
	
	void GestureMgr::deleteGesture(float x, float y){
		std::list<Gesture>::iterator git = _gestures.begin();
		std::list<Gesture>::iterator gitEnd = _gestures.end();
		for ( ; git != gitEnd; ++git ){
			if ( x == git->evt.x && y == git->evt.y ){
				_gestures.erase(git);
				break;
			}
		}
	}
	
	GestureMgr g_gestrueMgr;

} //namespace lw