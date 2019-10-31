#pragma once
#include <Eigen/core>
#include <Eigen/Geometry>


class Movable
{
public:
	Movable();
	Eigen::Matrix4f MakeTrans();
	void MyTranslate(Eigen::Vector3f amt);
	void MyRotate(Eigen::Vector3f rotAxis,float angle);
	void MyScale(Eigen::Vector3f amt);
private:
	Eigen::Transform<float,3,Eigen::Affine> T;

};

