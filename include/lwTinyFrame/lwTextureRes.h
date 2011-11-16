#ifndef __LW_POD_TEXTURE_H__
#define __LW_POD_TEXTURE_H__

#include "lwRes.h"

namespace lw{

	class TextureRes : public Res{
	public:
		static TextureRes* create(const char* fileName, bool reserveData = false, bool revertY = false);	//reserveData only for PNG
		static TextureRes* create(int w, int h, const char* pData, bool hasAlpha, bool reserveData, bool revertY);
		GLuint getGlId() const{
			return _glId;
		}
		const char* getFileName(){return _fileName.c_str();}
		const unsigned char* getData() const{
			return _pImgData;
		}
		int getWidth(){
			return _w;
		}
		int getHeight(){
			return _h;
		}
		int getChannelNum(){
			return _numChannels;
		}
		void reserveData();

	private:
		~TextureRes();
		TextureRes(const char* filename, bool reserveData = false, bool revertY = false);
		TextureRes(int w, int h, const char* pData, bool hasAlpha, bool reserveData = false, bool revertY = false);
		void loadPNG(const char* path, bool reserveData, bool revertY = false);

	private:
		GLuint _glId;
		std::string _fileName;
		int _numChannels;
		int _w, _h;
		unsigned char* _pImgData;

		static std::map<std::string, TextureRes*> _resMap;
		
	public:
		static void quit();
	};

} //namespace lw


#endif //__LW_POD_TEXTURE_H__