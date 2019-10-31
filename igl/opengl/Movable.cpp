#include "Movable.h"

Movable::Movable()
{
	T = Eigen::Transform<float, 3, Eigen::Affine>::Identity();
}

Eigen::Matrix4f Movable::MakeTrans()
{
	return T.matrix();
}

void Movable::MyTranslate(Eigen::Vector3f amt)
{
	T.translate(amt);
}
//angle in radians
void Movable::MyRotate(Eigen::Vector3f rotAxis, float angle)
{
	T.rotate(Eigen::AngleAxisf(angle, rotAxis));
}

void Movable::MyScale(Eigen::Vector3f amt)
{
	T.scale(amt);
}