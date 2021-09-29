#include "Movable.h"
#include <iostream>
Movable::Movable()
{
	Tout = Eigen::Affine3d::Identity();
	Tin = Eigen::Affine3d::Identity();
}

Movable::Movable(const Movable& mov)
{
	Tout = mov.Tout;
	Tin = mov.Tin;
}

Eigen::Matrix4f Movable::MakeTransScale()
{
	return (Tout.matrix()*Tin.matrix()).cast<float>();
}

Eigen::Matrix4d Movable::MakeTransScaled()
{
	return (Tout.matrix() * Tin.matrix());
}

Eigen::Matrix4d Movable::MakeTransd()
{
	Eigen::Matrix4d mat = Eigen::Matrix4d::Identity();
	mat.col(3) << Tin.translation(), 1;

	return (Tout.matrix() * mat);
}

void Movable::MyTranslate(Eigen::Vector3d amt, bool preRotation)
{
	
	if(preRotation)
		Tout.pretranslate(amt);
	else
		Tout.translate(amt);
}
//angle in radians
void Movable::MyRotate(Eigen::Vector3d rotAxis, double angle)
{
	Tout.rotate(Eigen::AngleAxisd(angle, rotAxis.normalized()));
}

void Movable::MyRotate(const Eigen::Matrix3d& rot)
{
	Tout.rotate(rot);
}

void Movable::MyScale(Eigen::Vector3d amt)
{
	Tin.scale(amt);
}

void Movable::TranslateInSystem(const Eigen::Matrix3d &Mat, Eigen::Vector3d amt, bool preRotation)
{
	MyTranslate(Mat.transpose() * amt, preRotation);
}

void Movable::RotateInSystem(const Eigen::Matrix3d &Mat, Eigen::Vector3d rotAxis, double angle)
{
	MyRotate(Mat.transpose() * rotAxis, angle);
}

void Movable::RotateInSystem(const Eigen::Matrix3d& Mat, const Eigen::Matrix3d& rot)
{
	MyRotate(Mat.transpose() * rot);
}

void  Movable::RotateInSystem(const Eigen::Matrix3d& Mat, const Eigen::Quaterniond rot)
{
	MyRotate(Mat.transpose() * Eigen::Affine3d(rot).rotation().matrix());
}


void Movable::SetCenterOfRotation(Eigen::Vector3d amt)
{
	Tin.translate(-amt);
	Tout.pretranslate( amt);
}

Eigen::Vector3d Movable::GetCenterOfRotation()
{
	return -Tin.translation();
}

void Movable::ResetRotation()
{
	Tout.rotate(Tout.rotation().matrix().transpose());
}

void Movable::ResetTranslation()
{
	Tout = Eigen::Affine3d::Identity() * Tin.inverse();
}

Eigen::Matrix3d Movable::ConvertRotationToEuler()
{
	Eigen::Vector3d y = Eigen::Vector3d(0,1,0);
	Eigen::Vector3d yNew = Tout.rotation().matrix()*y;
	Eigen::Matrix3d res = Tout.rotation().matrix();
	double angle1 = acosf(std::min(1.0, (std::max(-1.0, yNew.normalized().dot(y.normalized())))));
	Eigen::Vector3d n = y.cross(yNew);

	double angle2 = acosf(std::min(1.0, (std::max(-1.0, n.normalized().dot(Eigen::Vector3d(1, 0, 0))))));
	//std::cout << "angle1 " << angle1 << " angle2 " << angle2 << std::endl;
	Tout.rotate(Tout.rotation().matrix().transpose());
	MyRotate(Eigen::Vector3d(1, 0, 0), angle1);
	if (yNew(0) < 0)
		RotateInSystem(Tout.rotation().matrix(), -y, angle2);
	else
		RotateInSystem(Tout.rotation().matrix(), y, angle2);
	res =  Tout.rotation().matrix().transpose()*res;
	return res;
}

























//void Movable::TranslateInSystem(Eigen::Matrix4d Mat, Eigen::Vector3d amt, bool preRotation)
//{
//	Eigen::Vector3d v = Mat.transpose().block<3, 3>(0, 0) * amt; //transpose instead of inverse
//	MyTranslate(v, preRotation);
//}
//
//void Movable::RotateInSystem(Eigen::Matrix4d Mat, Eigen::Vector3d rotAxis, double angle)
//{
//	Eigen::Vector3d v = Mat.transpose().block<3, 3>(0, 0) * rotAxis; //transpose instead of inverse
//	MyRotate(v.normalized(), angle);
//}
//
//
//void Movable::SetCenterOfRotation(Eigen::Vector3d amt)
//{
//	Tout.pretranslate(Tout.rotation().matrix().block<3, 3>(0, 0) * amt);
//	Tin.translate(-amt);
//}
//
//Eigen::Vector3d Movable::GetCenterOfRotation()
//{
//	return Tin.translation();
//}
