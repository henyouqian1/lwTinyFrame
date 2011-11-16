#include "stdafx.h"
#include "lwTinyFrame/lwTextureRes.h"
#include "lwTinyFrame/lwFileSys.h"
#include "soil/SOIL.h"

namespace lw{

	std::map<std::string, TextureRes*> TextureRes::_resMap;

	TextureRes::TextureRes(const char* fileName, bool reserveData, bool revertY) : _glId(-1), _pImgData(NULL){
		lwassert(fileName);
		_fileName = fileName;

		size_t len = strlen(fileName);
		if ( len < 4 ){
			lwerror("texture file name too short: filepath = " << _f(fileName));
			_glId = -1;
			return;
		}
		_f fpath(fileName);
		if ( fileName[len-4] == '.' && fileName[len-3] == 'p' 
		&& fileName[len-2] == 'n' && fileName[len-1] == 'g'){
			if ( fpath.isValid() ){
				loadPNG(fpath, reserveData);
			}else{
				lwerror("texture is not exist: " << fileName);
				return;
			}
		}else{
			lwerror("only support PNG file: " << fileName);
			return;
		}
	}

	TextureRes::TextureRes(int w, int h, const char* pData, bool hasAlpha, bool reserveData, bool revertY) : _glId(-1), _pImgData(NULL){
		lwassert(pData);
		_w = w;
		_h = h;
		_numChannels = hasAlpha?4:3;
		int sz = w*h*_numChannels;
		_pImgData = new unsigned char[sz];
		memcpy(_pImgData, pData, sz);
		_glId = SOIL_internal_create_OGL_texture(_pImgData, w, h, _numChannels,
			SOIL_CREATE_NEW_ID, revertY ? SOIL_FLAG_INVERT_Y:0,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE);

		if ( !reserveData ){
			delete [] _pImgData;
			_pImgData = NULL;
		}

		if ( _glId == 0 ){
			lwerror("Failed to create texture");
			_glId = -1;
		}
	}

	void TextureRes::loadPNG(const char* path, bool reserveData, bool revertY){
		_pImgData = SOIL_load_image(path, &_w, &_h, &_numChannels, SOIL_LOAD_AUTO);
		_numChannels == 4 ? GL_RGBA:GL_RGB;
		_glId = SOIL_internal_create_OGL_texture(_pImgData, _w, _h, _numChannels,
			SOIL_CREATE_NEW_ID, revertY ? SOIL_FLAG_INVERT_Y:0,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE);
		
		if ( !reserveData ){
			SOIL_free_image_data(_pImgData);
			_pImgData = NULL;
		}
		
		if ( _glId == 0 ){
			lwerror("Failed to load texture: path=" << path);
			_glId = -1;
		}
	}

	TextureRes::~TextureRes(){
		if ( _glId != -1 ){
			glDeleteTextures(1, &_glId);
			if ( !_fileName.empty() ){
				std::map<std::string, TextureRes*>::iterator it = _resMap.find(_fileName.c_str());
				if ( it != _resMap.end() ){
					_resMap.erase(it);
				}else{
					lwerror("texture is not found: filename=" << _fileName.c_str());
				}
			}
			if ( _pImgData ){
				SOIL_free_image_data(_pImgData);
			}
		}
	}
	TextureRes* TextureRes::create(const char* fileName, bool reserveData, bool revertY){
		lwassert(fileName);

		std::string strFileName = fileName;

		std::map<std::string, TextureRes*>::iterator it = _resMap.find(strFileName);
		if ( it == _resMap.end() ){
			TextureRes* p = new TextureRes(fileName, reserveData, revertY);
			if ( p && p->_glId == -1 ){
				delete p;
				return NULL;
			}else{
				_resMap[strFileName] = p;
				return p;
			}
		}else{
			it->second->addRef();
			if ( reserveData ){
				it->second->reserveData();
			}
			return it->second;
		}
	}

	TextureRes* TextureRes::create(int w, int h, const char* pData, bool hasAlpha, bool reserveData, bool revertY){
		lwassert(pData);
		TextureRes* p = new TextureRes(w, h, pData, hasAlpha, reserveData, revertY);
		if ( p && p->_glId == -1 ){
			delete p;
			return NULL;
		}else{
			return p;
		}
	}

	void TextureRes::quit(){
		while( !_resMap.empty() ){
			lwwarning("Texture resource leak: file=" << _resMap.begin()->first.c_str());
			delete (_resMap.begin()->second);
		}
		_resMap.clear();
	}

	void TextureRes::reserveData(){
		if ( _pImgData == NULL ){
			_f fpath(_fileName.c_str());
			int numChannels;
			int w, h;
			_pImgData = SOIL_load_image(fpath, &w, &h, &numChannels, SOIL_LOAD_AUTO);
		}
	}

} //namespace lw