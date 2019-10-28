#include "Movable.h"

Movable::Movable()
{
	T0 = Eigen::Matrix4f::Identity();
	R = Eigen::Matrix4f::Identity();
	T1 = Eigen::Matrix4f::Identity();
}

Eigen::Matrix4f Movable::makeTrans()
{
	return T1 * R * T0;
}
