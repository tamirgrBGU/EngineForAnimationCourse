#pragma once
#include <Eigen/core>
#include <Eigen/Geometry>

class Movable
{
public:
	Movable();
	Eigen::Matrix4f makeTrans();
private:
	Eigen::Matrix4f T1, R, T0;

};

