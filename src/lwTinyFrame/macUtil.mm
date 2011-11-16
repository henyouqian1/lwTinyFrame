#include "macUtil.h"
#import <UIKit/UIKit.h>

void macOnChangeOrientation(int orient){
    if ( orient == 0 ){
        [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeLeft;
    }else if ( orient == 1 ){
        [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortrait;
    }else if ( orient == 2 ){
        [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
    }else if ( orient == 3 ){
        [UIApplication sharedApplication].statusBarOrientation = UIInterfaceOrientationPortraitUpsideDown;
    }
}