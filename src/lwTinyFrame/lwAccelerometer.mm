#include "stdafx.h"
#include "lwAccelerometer.h"
#include "lwApp.h"
#import "UIKit/UIAccelerometer.h"

cml::Vector3 g_vAcc;

@interface AccelerometerDelegate : NSObject <UIAccelerometerDelegate> {};
@end

@implementation AccelerometerDelegate

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration{
	
	lw::App::Orientation ort = lw::App::s().getConfig().orientation;
	if ( ort == lw::App::ORIENTATION_UP ){
		g_vAcc[0] = acceleration.x;
		g_vAcc[1] = acceleration.y;
		g_vAcc[2] = acceleration.z;
	}else if ( ort == lw::App::ORIENTATION_RIGHT ){
		g_vAcc[0] = -acceleration.y;
		g_vAcc[1] = -acceleration.x;
		g_vAcc[2] = acceleration.z;
	}else if ( ort == lw::App::ORIENTATION_LEFT ){
		g_vAcc[0] = acceleration.y;
		g_vAcc[1] = acceleration.x;
		g_vAcc[2] = acceleration.z;
	}
	
}

@end

AccelerometerDelegate* g_accelerometerDelegate;


namespace lw{

	void initAccelerometer(float updateInterval){
		g_vAcc[0] = 0.f;
		g_vAcc[1] = -1.f;
		g_vAcc[2] = 0.f;
		
		g_accelerometerDelegate = [[AccelerometerDelegate alloc] init];
		
		UIAccelerometer* p = [UIAccelerometer sharedAccelerometer];
		p.delegate = g_accelerometerDelegate;
		p.updateInterval = updateInterval;
	}
	
	void destroyAccelerometer(){
		UIAccelerometer* p = [UIAccelerometer sharedAccelerometer];
		p.delegate = nil;
		[g_accelerometerDelegate release];
	}

	const cml::Vector3& getAccelerometer(){
		return g_vAcc;
	}

} //namespace lw