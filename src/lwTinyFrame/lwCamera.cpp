#include "stdafx.h"
#include "lwTinyFrame/lwCamera.h"
#include "lwTinyFrame/lwApp.h"

namespace lw{

	Camera* Camera::_spCurrentCamera = NULL;

	Camera::Camera(){
		perspective(65.0f, 3.0f/2.0f, 1, 1000);
		lookat(10, 10, 10, 0, 0, 0, 0, 1, 0);
	}

	void Camera::lookat(
		GLfloat eyex,
		GLfloat eyey,
		GLfloat eyez,
		GLfloat centerx,
		GLfloat centery,
		GLfloat centerz,
		GLfloat upx,
		GLfloat upy,
		GLfloat upz){
			cml::matrix_look_at_RH(_viewMatrix, eyex, eyey, eyez,
				centerx, centery, centerz,
				upx, upy, upz);
	}

	void Camera::perspective(
		GLfloat fovy, 
		GLfloat aspect, 
		GLfloat zNear, 
		GLfloat zFar){
			fovy = fovy*(float)M_PI/180.f;
			cml::matrix_perspective_yfov_RH(_projMatrix, fovy, aspect, zNear, zFar, cml::z_clip_neg_one);

#ifdef __APPLE__
		appleAdj();
#endif
	}

	void Camera::ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat znear, GLfloat zfar){
		cml::matrix_orthographic_RH(_projMatrix, left, right, bottom, top, znear, zfar, cml::z_clip_neg_one);

#ifdef __APPLE__
		appleAdj();
#endif
	}

#ifdef __APPLE__
	void Camera::appleAdj(){
		float rotDegree = 0;
		const App::Orientation orient = App::s().getConfig().orientation;
		switch (orient) {
		case App::ORIENTATION_LEFT:
			rotDegree = M_PI_2;
			break;
		case App::ORIENTATION_RIGHT:
			rotDegree = -M_PI_2;
			break;
		default:
			break;
		}
		cml::Matrix44 mRot;
		cml::Vector3 v3;
		v3[0] = 0; v3[1] = 0; v3[2] = 1;
		cml::matrix_rotation_axis_angle(mRot, v3, rotDegree);
		_rawProjMatrix = _projMatrix;
		_projMatrix = mRot * _projMatrix;
	}
#endif

} //namespace lw