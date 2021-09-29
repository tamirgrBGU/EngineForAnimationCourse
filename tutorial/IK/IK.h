#pragma once
#include "igl/opengl/glfw/Viewer.h"


class IK : public igl::opengl::glfw::Viewer
{
public:
	IK();
	void Init(const std::string& config);
	void SetAxis(double size);
	Eigen::Vector3d GetTipPositionInSystem(int link);
	Eigen::Vector3d GetJointPositionInSystem(int link);
	inline double GetDistance() { return (destPosition - tipPosition).norm();}
	void CalcStep();
	void CalcStepQuaternions();
	void MakeChange();
	void Animate();
	void WhenTranslate();
	void ResetArm();
	void EulerAngles();
	inline int GetFirstLink() { return firstLink; }
	void SetParent(int childIndx,int parentIndx);
	Eigen::Vector3d GetCameraPosition();
	Eigen::Vector3d GetCameraForward();
	Eigen::Vector3d GetCameraUp();

	
	~IK();
	//void SandBox::QuadricCost(
	//	const int e,
	//	double& cost,
	//	Eigen::RowVectorXd& p);
private:
	const double cylinderLength = 1.6;
	const double delta = 0.15;
	const int linksNum = 10;
	int lastLink,firstLink,dest;
	std::vector<std::vector<int>> children;
	Eigen::Vector3d tipPosition;
	Eigen::Vector3d destPosition;
	Eigen::Vector3d scaleFactor;
	void AddAxis();
};

