#ifndef __LW_RES_H__
#define __LW_RES_H__

namespace lw{

	class Res
	{
	public:
		Res(): _refCount(1){}
		virtual ~Res() {};

		void addRef(){ 
			++_refCount;  
		}
		void release(){ 
			--_refCount; if ( _refCount == 0 ) delete this; 
		}

	private:
		int _refCount;
	};

} //namespace lw


#endif //__LW_RES_H__