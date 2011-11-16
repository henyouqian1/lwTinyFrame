#ifndef __LW_ACCELEROMETER_H__
#define __LW_ACCELEROMETER_H__

namespace lw{

	void initAccelerometer(float updateInterval);
	void destroyAccelerometer();
	const cml::Vector3& getAccelerometer();

#ifdef WIN32
	void setAccelerometer(const cml::Vector3& v3);
#endif

} //namespace lw



#endif //__LW_ACCELEROMETER_H__