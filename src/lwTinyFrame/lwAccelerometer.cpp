#include "stdafx.h"
#include "lwTinyFrame/lwAccelerometer.h"

namespace lw{

	namespace{
		cml::Vector3 g_vAcc;
	}
	
	void initAccelerometer(float updateInterval){
		g_vAcc[0] = 0.f;
		g_vAcc[1] = -1.f;
		g_vAcc[2] = 0.f;
	}

	void destroyAccelerometer(){

	}

	const cml::Vector3& getAccelerometer(){
		return g_vAcc;
	}

	void setAccelerometer(const cml::Vector3& v3){
		g_vAcc = v3;
	}


} //namespace lw