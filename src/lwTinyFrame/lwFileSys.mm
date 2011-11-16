#include "lwFileSys.h"

_f::_f(const char* filename){
	//NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];	
	NSString* str = [[NSString alloc] initWithCString:filename];
	NSString* path = [[NSBundle mainBundle] pathForResource:str ofType:nil];
	if ( path == nil ){
		//lwerror("file miss: " << filename);
		_pathStr.clear();
	}else{
		_pathStr = [path UTF8String];
		//[path release];
	}
	[str dealloc];
	
	//[pool release];
}

namespace lw {
	const char* getDocDir(){
		static std::string docDir;
		if ( docDir.empty() ){
			NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
			NSString* documentsDirectory = [paths objectAtIndex:0];
			docDir = [documentsDirectory UTF8String];
		}
		return docDir.c_str();
	}
}

