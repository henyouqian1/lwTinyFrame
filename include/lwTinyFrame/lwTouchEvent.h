#ifndef __LW_TOUCH_EVENT_H__
#define __LW_TOUCH_EVENT_H__

namespace lw{
	
	struct TouchEvent{
		enum Type{
			TOUCH,
			UNTOUCH,
			MOVE,
		};
		Type type;
		short x;
		short y;
		short prevX;
		short prevY;
	};

	struct Gesture{
		Gesture(int inid, const TouchEvent& event):id(inid), evt(event), updated(true){}
		int id;
		bool updated;
		TouchEvent evt;
	};

	class GestureMgr{
	public:
		GestureMgr():_currGestureId(1){}
		void main();
		void onTouchEvent(const std::vector<lw::TouchEvent>& events);
		void deleteGesture(float x, float y);
		const std::list<lw::Gesture>& getGestures(){
			return _gestures;
		}

	private:
		std::list<Gesture> _gestures;
		int _currGestureId;
	};
	
	extern GestureMgr g_gestrueMgr;

} //namespace lw

#endif //__LW_TOUCH_EVENT_H__