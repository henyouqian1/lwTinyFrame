#include "stdafx.h"
#include "lwTinyFrame/lwFileSys.h"

namespace lw{

	namespace{
		std::set<std::string> g_dirSet;
		std::map<std::string, const char*> g_fileNameMap;
	}

	bool FileSys::addDirectory(const char* dir, bool recursive){
		lwassert(dir);

		std::set<std::string>* pSet = &g_dirSet;
		std::map<std::string, const char*>* pMap = &g_fileNameMap;

		std::string str = dir;
		str += "/*.*";
		WIN32_FIND_DATAA findData;
		bool retVal = true;
		HANDLE h = FindFirstFileA(str.c_str(), &findData);
		if ( INVALID_HANDLE_VALUE == h ){
			lwerror("FindFirstFile failed: " << str.c_str() );
			return false;
		}
		BOOL b = FindNextFileA(h, &findData);	// find ..
		lwassert( b != FALSE);
		std::string strDir;
		while ( FindNextFileA(h, &findData) != FALSE ){
			str = dir;
			str += "/";
			strDir = str;
			str += findData.cFileName;
			if ( (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ){
				if ( recursive && findData.cFileName[0] != '.' ){
					if ( !addDirectory(str.c_str(), true) ){
						lwerror("addDirectory failed");
						retVal = false;
					}
				}
			}else if ( findData.cFileName[0] == '.' || strcmp(findData.cFileName, "Thumbs.db") == 0 ){
				continue;
			}else{
				std::set<std::string>::iterator it = pSet->find(strDir);
				if ( it == pSet->end() ){
					pSet->insert(strDir);
					it = pSet->find(strDir);
				}
				if ( pMap->find(findData.cFileName) != pMap->end() ){
					lwerror("file name collide: " << findData.cFileName);
					retVal = false;
				}else{
					pMap->insert(std::make_pair(findData.cFileName, it->c_str()));
				}
			}
		}
		FindClose(h);
		return retVal;
	}

	const char* getDocDir(){
		return "./doc";
	}

} //namespace lw

_f::_f(const char* filename){
	lwassert(filename);
	std::map<std::string, const char*>* pMap = &lw::g_fileNameMap;
	std::map<std::string, const char*>::iterator it = pMap->find(filename);
	if ( it == pMap->end() ){
		_pathStr.clear();
	}else{
		_pathStr = it->second;
		_pathStr.append(filename); 
	}
}


