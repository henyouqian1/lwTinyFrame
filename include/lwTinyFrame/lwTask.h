#ifndef __LW_TASK_H__
#define __LW_TASK_H__

namespace lw {

	struct TouchEvent;

	class Task
	{
	public:
		Task();
		virtual ~Task();

		bool start(int priority);
		bool stop();
		void run(bool b);
		void show(bool b);

	private:
		void main(float elapsed);
		void draw(float elapsed);
		virtual void vBegin(){};
		virtual void vEnd(){};
		virtual void vDestroy(){};
		virtual void vMain(float elapsed){};
		virtual void vDraw(float elapsed){};
		virtual bool vOnTouchEvent(std::vector<TouchEvent>& events){return false;}	//if return true, will not process the event further
		virtual void vShow(bool b){};
		virtual void vRun(bool b){};

	protected:
		enum Status{
			STATUS_IDLE,
			STATUS_ADDING,
			STATUS_REMOVEING,
			STATUS_NORMAL,
		};
		Status _status;
		int _priority;
		bool _isVisible;
		bool _isRunning;

	private:
		typedef void (Task::*DelayCall) ();
		struct DelayCallData{
			DelayCall delayCall;
			float t;
		};
		std::list<DelayCallData> _delayCallList;

	public:
		void addDelayCall(DelayCall dc, float t);

	friend class TaskMgr;
	};

	class TaskMgr
	{
	public:
		static void quit();
		static bool add(Task& task, int priority);
		static bool remove(Task& task);
		static void removeAll();
		static void main(float elepsed);
		static void draw(float elepsed);
		static void processPending();
		static void onTouchEvent(std::vector<TouchEvent>& events);

	};

}//namespace lw

#endif //__LW_TASK_H__