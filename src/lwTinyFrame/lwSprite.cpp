#include "stdafx.h"
#include "lwTinyFrame/lwSprite.h"
#include "lwTinyFrame/lwApp.h"
#include "lwTinyFrame/lwFileSys.h"
#include "lwTinyFrame/lwTextureRes.h"

namespace lw{
	
	Sprite* Sprite::create(const char* texFileName, bool reserveData){
		lwassert(texFileName);
		bool ok;
		Sprite* p = new Sprite(texFileName, reserveData, ok);
		if ( p && !ok ){
			lwerror("Sprite construct failed");
			delete p;
			p = NULL;
		}
		return p;
	}

	Sprite::Sprite(const char* texFileName, bool reserveData, bool &ok)
		:_u1(0.f), _v1(0.f), _u2(1.f), _v2(1.f)
		,_ancX(0.f), _ancY(0.f), _color(COLOR_WHITE)
		,_posX(0.f), _posY(0.f), _scaleX(1.f), _scaleY(1.f), _rotate(0.f)
		,_width(1), _height(1), _matrixNeedUpdate(false){
			_pTextureRes = TextureRes::create(texFileName, reserveData);
			ok = _pTextureRes != NULL;
			_texW = _pTextureRes->getWidth();
			_texH = _pTextureRes->getHeight();
			_glId = _pTextureRes->getGlId();
			_hasAlpha = _pTextureRes->getChannelNum() == 4;
			setUV(0, 0, -1, -1);
			_matrix.identity();
	}

	Sprite::~Sprite(){
		if ( _pTextureRes ){
			_pTextureRes->release();
		}
	}

	void Sprite::setUV(int u, int v, int w, int h){
		_u1 = (float)u/(float)_texW;
		_v1 = (float)v/(float)_texH;
		if ( w <= 0 ){
			_width = _texW;
		}else{
			_width = w;
		}
		_u2 = (float)(u+_width)/(float)_texW;

		if ( h <= 0 ){
			_height = _texH;
		}else{
			_height = h;
		}
		_v2 = (float)(v+_height)/(float)_texH;

		_v1 = 1.f - _v1;
		_v2 = 1.f - _v2;
	}

	namespace{
		std::vector<Sprite::Vertex> g_vertexVec;
		GLuint g_currTextureId = 0;
		Color g_currColor = COLOR_WHITE;
		const float Z_BEGIN = -1000.f;
		const float Z_END = 1000.f;
	}

	void Sprite::collect(){
		if ( _matrixNeedUpdate ){
			updateMatrix();
		}

		if ( _glId != g_currTextureId || _color != g_currColor ){
			if ( g_currTextureId != 0 ){
				flush();
			}
			g_currTextureId = _glId;
			g_currColor = _color;
		}

		float posX1 = -_ancX;
		float posY1 = -_ancY;
		float posX2 = posX1 + (float)_width;
		float posY2 = posY1 + (float)_height;

		cml::Vector3 p1;
		p1[0] = posX1; p1[1] = posY1; p1[2] = 0;
		p1 = cml::transform_point(_matrix, p1);
		cml::Vector3 p2;
		p2[0] = posX1; p2[1] = posY2; p2[2] = 0;
		p2 = cml::transform_point(_matrix, p2);
		cml::Vector3 p3;
		p3[0] = posX2; p3[1] = posY1; p3[2] = 0;
		p3 = cml::transform_point(_matrix, p3);
		cml::Vector3 p4;
		p4[0] = posX2; p4[1] = posY2; p4[2] = 0;
		p4 = cml::transform_point(_matrix, p4);

		float minx = min(p1[0], p2[0]);
		minx = min(minx, p3[0]);
		minx = min(minx, p4[0]);
		float miny = min(p1[1], p2[1]);
		miny = min(miny, p3[1]);
		miny = min(miny, p4[1]);
		float maxx = max(p1[0], p2[0]);
		maxx = max(maxx, p3[0]);
		maxx = max(maxx, p4[0]);
		float maxy = max(p1[1], p2[1]);
		maxy = max(maxy, p3[1]);
		maxy = max(maxy, p4[1]);

		const App::Config& conf = App::s().getConfig();
		if ( minx >= conf.width || maxx <= 0.f 
		|| miny >= conf.height || maxy <= 0.f ){
			return;
		}
		
		//1
		Vertex v;
		v.x = p1[0];
		v.y = -p1[1];
		v.z = 0.f;
		v.u = _u1;
		v.v = 1.f -_v1;
		g_vertexVec.push_back(v);

		//2
		v.x = p2[0];
		v.y = -p2[1];
		v.z = 0.f;
		v.u = _u1;
		v.v = 1.f -_v2;
		g_vertexVec.push_back(v);

		//3
		v.x = p3[0];
		v.y = -p3[1];
		v.z = 0.f;
		v.u = _u2;
		v.v = 1.f -_v1;
		g_vertexVec.push_back(v);

		//3
		v.x = p3[0];
		v.y = -p3[1];
		v.z = 0.f;
		v.u = _u2;
		v.v = 1.f -_v1;
		g_vertexVec.push_back(v);

		//2
		v.x = p2[0];
		v.y = -p2[1];
		v.z = 0.f;
		v.u = _u1;
		v.v = 1.f -_v2;
		g_vertexVec.push_back(v);

		//4
		v.x = p4[0];
		v.y = -p4[1];
		v.z = 0.f;
		v.u = _u2;
		v.v = 1.f -_v2;
		g_vertexVec.push_back(v);
	}

	void Sprite::flush(BlendMode bm){
		if ( g_vertexVec.empty() ){
			return;
		}

		glDisable(GL_LIGHTING);
		glDepthMask(GL_FALSE);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		const App::Config conf = App::s().getConfig();

#ifdef __APPLE__
		float rotDegree = 0;
		const App::Orientation orient = conf.orientation;
		switch (orient) {
			case App::ORIENTATION_LEFT:
				rotDegree = 90;
				break;
			case App::ORIENTATION_RIGHT:
				rotDegree = -90;
				break;
            case App::ORIENTATION_DOWN:
                rotDegree = 180;
                break;
			default:
				break;
		}
		glRotatef(rotDegree, 0, 0, 1);
#endif
#ifdef WIN32
		int dOrient = (conf.orientation-conf.orientation0);
		float rotDegree = -dOrient * 90.f;
		glRotatef(rotDegree, 0, 0, 1);
#endif

		cml::Matrix44 m;
		float w = (float)conf.width;
		float h = (float)conf.height;
		cml::matrix_orthographic_RH(m, 0.f, w, -h, 0.f, Z_BEGIN, Z_END, cml::z_clip_neg_one);
		glMultMatrixf(m.data());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);
		
		if ( bm == BLEND_NONE ){
			glDisable(GL_BLEND);
		}else{
			glEnable(GL_BLEND);
			if ( bm == BLEND_OVERLAY ){
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}else if ( bm == BLEND_ADD ){
				glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
			}
		}
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(g_vertexVec[0]));
		char* p = (char*)&(g_vertexVec[0]);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), p+(sizeof(float)*3));

		glColor4f(g_currColor.r/255.f, g_currColor.g/255.f, g_currColor.b/255.f, g_currColor.a/255.f);
		glBindTexture(GL_TEXTURE_2D, g_currTextureId);
		lwassert(glGetError()==GL_NO_ERROR);

		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)g_vertexVec.size());

		g_vertexVec.clear();
		glDepthMask(GL_TRUE);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_BLEND);
		g_currTextureId = 0;
	}

	void Sprite::setFilter(GLfixed minFilter, GLfixed magFilter){
		glBindTexture(GL_TEXTURE_2D, _pTextureRes->getGlId());
		glTexParameterx( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
		glTexParameterx( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	const cml::Matrix44& Sprite::getMatrix(){
		if ( _matrixNeedUpdate ){
			updateMatrix();
		}
		return _matrix;
	}
	
	void Sprite::updateMatrix(){
		_matrixNeedUpdate = false;
		cml::Matrix44 m;
		cml::matrix_translation(_matrix, _posX, _posY, 0.f);
		if ( _rotate != 0.f ){
			cml::matrix_rotation_world_z(m, _rotate);
			_matrix *= m;
		}
		if ( _scaleX != 1.f || _scaleY != 1.f ){
			cml::matrix_scale(m, _scaleX, _scaleY, 1.f);
			_matrix *= m;
		}
	}

	Sprite9* Sprite9::create(const char* fileName, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3){
		lwassert(fileName);
		bool ok = false;
		Sprite9* pSprite9 = new Sprite9(fileName, uvX, uvY, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3, ok);
		lwassert(pSprite9);
		if ( !ok ){
			delete pSprite9;
			return NULL;
		}
		return pSprite9;
	}
	Sprite9::Sprite9(const char* fileName, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, bool& ok):_needUpdate(true){
		_pSprite = Sprite::create(fileName);
		if ( _pSprite == NULL ){
			lwerror("Sprite::create failed: file=" <<fileName);
			return;
		}

		_x[0] = 0.f;
		_y[0] = 0.f;

		_uvW[0] = uvW1;
		_uvW[1] = uvW2;
		_uvW[2] = uvW3;

		_uvH[0] = uvH1;
		_uvH[1] = uvH2;
		_uvH[2] = uvH3;

		_uvX[0] = uvX;
		_uvX[1] = _uvX[0] + uvW1;
		_uvX[2] = _uvX[1] + uvW2;

		_uvY[0] = uvY;
		_uvY[1] = _uvY[0] + uvH1;
		_uvY[2] = _uvY[1] + uvH2;

		_width = (float)(_uvW[0] + _uvW[1] + _uvW[2]);
		_height = (float)(_uvH[0] + _uvH[1] + _uvH[2]);

		ok = true;
	}
	Sprite9::~Sprite9(){
		if ( _pSprite ){
			delete _pSprite;
		}
	}
	void Sprite9::setPos(float x, float y){
		_x[0] = x;
		_y[0] = y;
		_needUpdate = true;
	}
	void Sprite9::setSize(float w, float h){
		_width = w;
		_height = h;
		_needUpdate = true;
	}
	void Sprite9::setColor(const lw::Color& color){
		_pSprite->setColor(color);
	}
	void Sprite9::update(){
		float midW = max(0.f, _width - _uvW[0] - _uvW[2]);
		float midH = max(0.f, _height - _uvH[0] - _uvH[2]);
		_x[1] = _x[0] + _uvW[0];
		_x[2] = _x[1] + midW;
		_y[1] = _y[0] + _uvH[0];
		_y[2] = _y[1] + midH;

		_w[0] = (float)_uvW[0];
		_w[1] = midW;
		_w[2] = (float)_uvW[2];
		_h[0] = (float)_uvH[0];
		_h[1] = midH;
		_h[2] = (float)_uvH[2];
	}
	void Sprite9::collect(){
		if ( _needUpdate ){
			update();
			_needUpdate = false;
		}
		for ( int i = 0; i < 3; ++i ){
			for ( int j = 0; j < 3; ++j ){
				_pSprite->setUV(_uvX[i], _uvY[j], _uvW[i], _uvH[j]);
				_pSprite->setPos(_x[i], _y[j]);
				_pSprite->setSize(_w[i], _h[j]);
				_pSprite->collect();
			}
		}
	}
	void Sprite9::setFilter(GLfixed minFilter, GLfixed magFilter){
		_pSprite->setFilter(minFilter, magFilter);
	}

} //namespace lw