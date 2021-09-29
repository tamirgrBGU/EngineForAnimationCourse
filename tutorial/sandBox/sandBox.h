#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "igl/aabb.h"

class SandBox : public igl::opengl::glfw::Viewer
{
public:
	SandBox();
	void Init(const std::string& config);
	void Simplification();
	void SetQueue(Eigen::MatrixXd &V, Eigen::MatrixXi &F);
	~SandBox();
	bool CheckCollision(int indx1, int indx2,const Eigen::Matrix4d &mat1,const Eigen::Matrix4d &mat2);
	void ResetScene(bool onStart);
	void ShowTree();
	void ResetBoxes();
	void DrawBestAxis(bool toRotate);
	double doubleVariable;
private:
	// Prepare array-based edge data structures and priority queue
	std::vector<Eigen::VectorXi> EMAP; 
	std::vector<Eigen::MatrixXi> E, EF, EI;
	typedef std::set<std::pair<double, int> > PriorityQueue;
	std::vector<PriorityQueue*> Q;
	std::vector<std::vector<PriorityQueue::iterator>> Qit;
	std::vector<Eigen::MatrixXd> C;
	std::vector<int> num_collapsed;
	std::vector<igl::AABB<Eigen::MatrixXd, 3>*> trees;
	std::vector< igl::AABB<Eigen::MatrixXd, 3>*> subTrees;
	std::vector<Eigen::Vector3d> velocities;
	
	
	void Animate();
};

