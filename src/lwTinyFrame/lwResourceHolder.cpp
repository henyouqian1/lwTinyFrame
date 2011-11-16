#include "stdafx.h"
#include "lwTinyFrame/lwResourceHolder.h"
#include "lwTinyFrame/lwSprite.h"
#include "lwTinyFrame/lwFont.h"

namespace lw{

	ResourceHolder::~ResourceHolder(){
		clear();
	}

	void ResourceHolder::clear(){
		{
			std::vector<lw::Sprite*>::iterator it = _textures.begin();
			std::vector<lw::Sprite*>::iterator itEnd = _textures.end();
			for ( ; it != itEnd; ++it ){
				delete *it;
			}
			_textures.clear();
		}
		{
			std::vector<lw::Font*>::iterator it = _fonts.begin();
			std::vector<lw::Font*>::iterator itEnd = _fonts.end();
			for ( ; it != itEnd; ++it ){
				delete *it;
			}
			_fonts.clear();
		}
	}

	void ResourceHolder::addTexture(const char* fileName){
		lw::Sprite* p = lw::Sprite::create(fileName);
		lwassert(p);
		_textures.push_back(p);
	}

	void ResourceHolder::addFont(const char* fileName){
		lw::Font* p = lw::Font::create(fileName);
		lwassert(p);
		_fonts.push_back(p);
	}

} //namespace lw