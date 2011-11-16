#ifndef __LW_MINMAX_H__
#define	__LW_MINMAX_H__

template<typename T>
const T& min(const T& a, const T& b){
	return a < b ? a : b;
}
template<typename T>
const T& max(const T& a, const T& b){
	return a > b ? a : b;
}

#endif //__LW_MINMAX_H__