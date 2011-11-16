#ifndef __LW_CAMERA_H__
#define __LW_CAMERA_H__

namespace lw{

	class Camera{
	public:
		Camera();
		static Camera* getCurrent(){
			lwassert(_spCurrentCamera);
			return _spCurrentCamera;
		}

		const cml::Matrix44& getView() const {
			return _viewMatrix;
		}
		const cml::Matrix44& getProj() const {
			return _projMatrix;
		}
		
		void lookat(GLfloat eyex, GLfloat eyey, GLfloat eyez,
					GLfloat centerx, GLfloat centery, GLfloat centerz,
					GLfloat upx, GLfloat upy, GLfloat upz);
		void setViewMatrix(cml::Matrix44& viewMatrix){
			_viewMatrix = viewMatrix;
		}
		void perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);
		void ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat znear, GLfloat zfar);
		
		void setCurrent(){
			_spCurrentCamera = this;
		}
		
	private:
		cml::Matrix44 _viewMatrix;
		cml::Matrix44 _projMatrix;

		static Camera* _spCurrentCamera;

#ifdef __APPLE__
	public:
		const cml::Matrix44& getRawProj() const {
			return _rawProjMatrix;
		}
		
	private:
		void appleAdj();
		cml::Matrix44 _rawProjMatrix;
#endif

	};

} //namespace lw



#endif //__LW_CAMERA_H__