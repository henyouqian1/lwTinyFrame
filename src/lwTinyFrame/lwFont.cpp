#include "stdafx.h"
#include "lwTinyFrame/lwFont.h"
#include "lwTinyFrame/lwRes.h"
#include "lwTinyFrame/lwFileSys.h"
#include "lwTinyFrame/lwSprite.h"

namespace lw{

	class FontRes: public Res{
	public:
		static FontRes* create(const char* fileName);
		static void quit();
		
		int getH(){
			return _commonInfo.lineHeight;
		}
		const char* getFileName(){
			return _fileName.c_str();
		}

		struct CommonInfo{
			unsigned short lineHeight;
			unsigned short base;
			unsigned short texturW;
			unsigned short texturH;
			unsigned short numPages;
			char bitField;
			unsigned char alphaChnl;
			unsigned char redChnl;
			unsigned char greenChnl;
			unsigned char blueChnl;
		};
		struct CharInfo{
			unsigned int id;
			unsigned short x;
			unsigned short y;
			unsigned short w;
			unsigned short h;
			short xoffset;
			short yoffset;
			short xadvance;
			unsigned char page;
			unsigned char chnl;
		};
		const std::map<wchar_t, CharInfo>& getCharInfoMap(){
			return _charInfoMap;
		}
		const CommonInfo& getCommonInfo(){
			return _commonInfo;
		}
		const std::vector<Sprite*> getSprites(){
			return _sprites;
		}

	private:
		FontRes(const char* fileName, bool& ok);
		~FontRes();

		std::string _fileName;
		CommonInfo _commonInfo;
		
		std::map<wchar_t, CharInfo> _charInfoMap;
		std::vector<Sprite*> _sprites;

		static std::map<std::string, FontRes*> _sResMap;
	};

	std::map<std::string, FontRes*> FontRes::_sResMap;

	FontRes* FontRes::create(const char* fileName){
		std::map<std::string, FontRes*>::iterator it = _sResMap.find(fileName);
		if ( it != _sResMap.end() ){
			it->second->addRef();
			return it->second;
		}
		bool ok = false;
		FontRes* pRes = new FontRes(fileName, ok);
		lwassert(pRes);
		if ( !ok ){
			lwerror("new FontRes error");
			delete pRes;
			return NULL;
		}
		_sResMap[fileName] = pRes;
		
		return pRes;
	}

	void FontRes::quit(){
		if ( !_sResMap.empty() ){
			lwerror("Font leak!");
			std::map<std::string, FontRes*>::iterator it = _sResMap.begin();
			std::map<std::string, FontRes*>::iterator itEnd = _sResMap.end();
			for ( ; it != itEnd; ++it ){
				lwerror("Font leak: " << it->second->getFileName());
			}
		}
	}

	FontRes::FontRes(const char* fileName, bool& ok){
		ok = false;
		lwassert(fileName);
		_fileName = fileName;

		FILE* pf = fopen(_f(fileName), "rb");
		if ( pf == NULL ){
			lwerror("fopen failed: fileName=" << fileName);
			return;
		}

		//"BMF" and version
		char bmfAndVersion[4];
		fread(bmfAndVersion, sizeof(bmfAndVersion), 1, pf);
		if ( bmfAndVersion[0] != 'B' && bmfAndVersion[1] != 'M' && bmfAndVersion[2] != 'F' ){
			lwerror("not BMF head");
			fclose(pf);
		}
		if ( bmfAndVersion[3] != 3 ){
			lwerror("version 3 needed: this file version=" << (int)(bmfAndVersion[3]));
			fclose(pf);
		}

		//Block type 1: info
		char blockType = 0;
		int blockSize = 0;
		fread(&blockType, sizeof(blockType), 1, pf);
		lwassert(blockType == 1);
		fread(&blockSize, sizeof(blockSize), 1, pf);
		fseek(pf, blockSize, SEEK_CUR);

		//Block type 2: common
		fread(&blockType, sizeof(blockType), 1, pf);
		lwassert(blockType == 2);
		fread(&blockSize, sizeof(blockSize), 1, pf);
		fread(&_commonInfo, blockSize, 1, pf);

		//Block type 3: pages
		fread(&blockType, sizeof(blockType), 1, pf);
		lwassert(blockType == 3);
		fread(&blockSize, sizeof(blockSize), 1, pf);
		char* buf = new char[blockSize];
		fread(buf, blockSize, 1, pf);
		char* p = buf;
		int strSize = 0;
		while ( p < buf + blockSize ){
			strSize = (int)strlen(p);
			Sprite* pSprite = Sprite::create(p);
			if ( pSprite == NULL ){
				lwerror("font create sprite failed: fileName=" << p);
				delete [] buf;
				fclose(pf);
				return;
			}
			_sprites.push_back(pSprite);
			p += strSize + 1;
		}
		delete [] buf;

		//Block type 4: chars
		fread(&blockType, sizeof(blockType), 1, pf);
		lwassert(blockType == 4);
		fread(&blockSize, sizeof(blockSize), 1, pf);
		int numChars = blockSize/20;
		CharInfo cf;
		for ( int i = 0; i < numChars; ++i ){
			fread(&cf, sizeof(cf), 1, pf);
			wchar_t wc = cf.id;
			_charInfoMap[wc] = cf;
		}

		fclose(pf);
		ok = true;
	}

	FontRes::~FontRes(){
		{
			std::vector<Sprite*>::iterator it = _sprites.begin();
			std::vector<Sprite*>::iterator itEnd = _sprites.end();
			for ( ; it != itEnd; ++it ){
				delete *it;
			}
		}
		{
			std::map<std::string, FontRes*>::iterator it = _sResMap.find(_fileName);
			if ( it != _sResMap.end() ){
				_sResMap.erase(it);
			}
		}
	}

	Font* Font::create(const char* fileName){
		FontRes* pRes = FontRes::create(fileName);
		if ( pRes == NULL ){
			return NULL;
		}
		Font* pFont = new Font(pRes);
		lwassert(pFont);
		return pFont;
	}

	Font::Font(FontRes* pRes):_pRes(pRes), _needUpdate(true)
	,_posX(0.f), _posY(0.f), _align(ALIGN_TOP_LEFT), _scaleX(1.f), _scaleY(1.f)
	,_rotate(0.f), _color(COLOR_WHITE), _width(0.f), _height(0.f){
	}

	Font::~Font(){
		_pRes->release();
	}

	void Font::update(){
		_needUpdate = false;

		_width = 0.f;
		_height = 0.f;
		_linesOffset.clear();
		
		const std::map<wchar_t, FontRes::CharInfo>& charInfoMap = _pRes->getCharInfoMap();
		const FontRes::CommonInfo& comInfo = _pRes->getCommonInfo();
		size_t len = _text.length();
		const wchar_t* p = _text.c_str();

		float x = 0;
		int alignH = _align % 3;
		while ( 1 ){
			if ( *p == '\n' || p == _text.c_str()+len ){
				x = x*_scaleX;
				switch (alignH)
				{
				case 0:
					_linesOffset.push_back(0);
					break;
				case 1:
					_linesOffset.push_back((-x)*.5f);
					break;
				case 2:
					_linesOffset.push_back(-x);
					break;
				}
				_width = max(_width, x);
				_height += comInfo.lineHeight;
				x = 0;
				if ( p == _text.c_str()+len ){
					break;
				}
			}else{
				std::map<wchar_t, FontRes::CharInfo>::const_iterator it = charInfoMap.find(*p);
				if ( it == charInfoMap.end() ){
					it = charInfoMap.find(' ');
					if ( it == charInfoMap.end() ){
						x += 10.f;
						++p;
						continue;
					}
				}
				const FontRes::CharInfo& charInfo = it->second;
				x += charInfo.xadvance;
			}
			++p;
		}
		int alignV = _align / 3;
		switch (alignV)
		{
		case 0:
			_y0 = _posY;
			break;
		case 1:
			_y0 = _posY-.5f*_height*_scaleY;
			break;
		case 2:
			_y0 = _posY-_height*_scaleY;
			break;
		}
	}

	void Font::getSize(float& w, float& h){
		if ( _needUpdate ){
			update();
		}
		w = _width;
		h = _height;
	}

	void Font::collect(){
		if ( _needUpdate ){
			update();
		}
		const wchar_t* text = _text.c_str();
		size_t len = _text.size();
		const wchar_t* p = text;
		float currX = _posX;
		if ( !_linesOffset.empty() ){
			currX += _linesOffset[0];
		}
		float currY = _y0;

		int currLine = 1;
		const FontRes::CommonInfo& comInfo = _pRes->getCommonInfo();
		const std::map<wchar_t, FontRes::CharInfo>& charInfoMap = _pRes->getCharInfoMap();
		const std::vector<Sprite*> sprites = _pRes->getSprites();
		while ( p < text+len ){
			if ( *p == '\n' ){
				currY += comInfo.lineHeight*_scaleY;
				currX = _posX;
				if ( currLine < (int)_linesOffset.size() ){
					currX += _linesOffset[currLine];
				}
				++currLine;
			}else{
				std::map<wchar_t, FontRes::CharInfo>::const_iterator it = charInfoMap.find(*p);
				if ( it == charInfoMap.end() ){
					it = charInfoMap.find(' ');
					if ( it == charInfoMap.end() ){
						currX += 10;
						++p;
						continue;
					}
				}
				const FontRes::CharInfo& charInfo = it->second;
				lwassert(charInfo.page < sprites.size());
				Sprite* pSprite = sprites[charInfo.page];
				pSprite->setUV(charInfo.x, charInfo.y, charInfo.w, charInfo.h);
				if ( _rotate == 0 ){
					pSprite->setPos((float)currX+charInfo.xoffset, (float)currY+charInfo.yoffset*_scaleY);
					pSprite->setScale(_scaleX, _scaleY);
					pSprite->setColor(_color);
					pSprite->setRotate(0);
					pSprite->collect();
				}else{
					cml::Vector2 v2;
					v2[0] = (float)currX+charInfo.xoffset-_posX;
					v2[1] = (float)currY+charInfo.yoffset-_posY;
					v2 = cml::rotate_vector_2D(v2, _rotate);
					pSprite->setPos(_posX+v2[0], _posY+v2[1]);
					pSprite->setScale(_scaleX, _scaleY);
					pSprite->setRotate(_rotate);
					pSprite->setColor(_color);
					pSprite->collect();
				}
				currX += charInfo.xadvance*_scaleX;
			}
			++p;
		}
	}

	void Font::quit(){
		FontRes::quit();
	}

	int Font::getH(){
		return _pRes->getH();
	}

} //namespace lw