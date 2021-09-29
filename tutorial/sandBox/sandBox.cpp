#include "tutorial/sandBox/sandBox.h"
#include "igl/edge_flaps.h"
#include "igl/collapse_edge.h"
#include "Eigen/dense"
#include <functional>

void QuadricCost(
	const int e,
	const Eigen::MatrixXd& V,
	const Eigen::MatrixXi& /*F*/,
	const Eigen::MatrixXi& E,
	const Eigen::VectorXi& /*EMAP*/,
	const Eigen::MatrixXi& /*EF*/,
	const Eigen::MatrixXi& /*EI*/,
	double& cost,
	Eigen::RowVectorXd& p)
{

	cost = (V.row(E(e, 0)) - V.row(E(e, 1))).norm();
	p = 0.5 * (V.row(E(e, 0)) + V.row(E(e, 1)));
}

void AddBox(igl::opengl::ViewerData& data, const Eigen::AlignedBox<double, 3> & box, const Eigen::RowVector3d& color)
{
	data.add_edges(box.corner(box.BottomLeftCeil).transpose(), box.corner(box.BottomRightCeil).transpose(), color);
	data.add_edges(box.corner(box.BottomLeftCeil).transpose(), box.corner(box.BottomLeftFloor).transpose(), color);
	data.add_edges(box.corner(box.BottomLeftCeil).transpose(), box.corner(box.TopLeftCeil).transpose(), color);

	data.add_edges(box.corner(box.TopRightCeil).transpose(), box.corner(box.TopRightFloor).transpose(), color);
	data.add_edges(box.corner(box.TopRightCeil).transpose(), box.corner(box.BottomRightCeil).transpose(), color);
	data.add_edges(box.corner(box.TopRightCeil).transpose(), box.corner(box.TopLeftCeil).transpose(), color);

	data.add_edges(box.corner(box.BottomRightFloor).transpose(), box.corner(box.BottomRightCeil).transpose(), color);
	data.add_edges(box.corner(box.BottomRightFloor).transpose(), box.corner(box.BottomLeftFloor).transpose(), color);
	data.add_edges(box.corner(box.BottomRightFloor).transpose(), box.corner(box.TopRightFloor).transpose(), color);

	data.add_edges(box.corner(box.TopLeftFloor).transpose(), box.corner(box.TopRightFloor).transpose(), color);
	data.add_edges(box.corner(box.TopLeftFloor).transpose(), box.corner(box.BottomLeftFloor).transpose(), color);
	data.add_edges(box.corner(box.TopLeftFloor).transpose(), box.corner(box.TopLeftCeil).transpose(), color);

}

SandBox::SandBox()
{
	

}

void SandBox::Init(const std::string &config)
{
	std::string item_name;
	std::ifstream nameFileout;
	doubleVariable = 0;
	nameFileout.open(config);
	if (!nameFileout.is_open())
	{
		std::cout << "Can't open file "<<config << std::endl;
	}
	else
	{
		double num = 0;
		while (nameFileout >> item_name)
		{
			std::cout << "openning " << item_name << std::endl;
			load_mesh_from_file(item_name);
			SetQueue(data().V, data().F);
			parents.push_back(-1);
			data().add_points(Eigen::RowVector3d(0, 0, 0), Eigen::RowVector3d(0, 0, 1));
			data().show_overlay_depth = false;
			data().point_size = 10;
			data().line_width = 2;
			data().set_visible(false, 1);
			trees.push_back(new igl::AABB<Eigen::MatrixXd, 3>());
			trees.back()->init(data().V, data().F);
			Eigen::AlignedBox<double, 3> box = trees.back()->m_box;
			
			std::cout <<"center "<< box.center() << std::endl;
			std::cout << "size "<<box.sizes() << std::endl;

			data().add_points(box.center().transpose(), Eigen::RowVector3d(0, 0, 0));
			AddBox(data(),box, Eigen::RowVector3d(0, 1, 0));
			if(num==0)
				DrawBestAxis(false);
			else
				DrawBestAxis(true);
			num++;
			/*igl::AABB<Eigen::MatrixXd, 3> *tree = trees.back();
			if (!tree->is_leaf())
			{
				for (int i = 1; !tree->m_left->is_leaf();i++)
				{
					tree = tree->m_left;
					std::cout << "level " << i << std::endl;
				}

				AddBox(tree->m_box, Eigen::RowVector3d(0, 1, 0));
			}*/
			
		}
		nameFileout.close();
	}
	MyTranslate(Eigen::Vector3d(0, 0, -1), true);
	
	data().set_colors(Eigen::RowVector3d(0.9, 0.1, 0.1));
	ResetScene(true);
	//parents[2] = 1;
	
	//selected_data_index = 1;
//	int f =2;
//	Eigen::Vector3i face = data().F.row(f);
//std::cout << "faces cube\n" <<  << std::endl;
//	std::cout << "a b c d\n" << data().F_normals.row(f).normalized() <<" "<< -data().F_normals.row(f).normalized() * data().V.row(face(0)).transpose() << std::endl;

	//std::cout << "edges cube\n" << E[1] << std::endl;
	//std::cout << "vertices cube\n" << data().V << std::endl;
	//std::cout << "emap cube\n" << EMAP[1] << std::endl;
	//std::cout << "edge face cube\n" << EF[1] << std::endl;
	//std::cout << "edge face cube\n" << C[1] << std::endl;

	
}

void SandBox::ShowTree()
{
	AddBox(data(), trees.back()->m_right->m_box, Eigen::RowVector3d(1, 0,1));
	AddBox(data(), trees.back()->m_left->m_box, Eigen::RowVector3d(0, 1, 1));
	AddBox(data(), trees.back()->m_left->m_right->m_box, Eigen::RowVector3d(1, 1, 1));
	AddBox(data(), trees.back()->m_left->m_left->m_box, Eigen::RowVector3d(1, 1, 0));
	AddBox(data(), trees.back()->m_left->m_right->m_left->m_box, Eigen::RowVector3d(0, 0, 1));
	AddBox(data(), trees.back()->m_left->m_right->m_right->m_box, Eigen::RowVector3d(0, 0, 0));
}

void SandBox::Simplification()
{	
	Eigen::MatrixXd V = data().V;
	Eigen::MatrixXi F = data().F;

	int i = selected_data_index;
	if (i>=0 && !Q[i]->empty())
	{
		bool something_collapsed = false;
		// collapse edge
		const int max_iter = std::ceil(0.05 * Q[i]->size());

		for (int j = 0; j < max_iter; j++)
		{
			if (!igl::collapse_edge(
				QuadricCost, V, F, E[i], EMAP[i], EF[i], EI[i], *Q[i], Qit[i], C[i]))
			{
				break;
			}
			something_collapsed = true;
			num_collapsed[i]++;
		}
		std::cout << "collapsed " <<num_collapsed[i]<<" edges"<< std::endl;
		if (something_collapsed)
		{
			data().clear();
			data().set_mesh(V, F);
			data().set_face_based(true);
		}
	}
	else
		std::cout << "empty" << std::endl;
	data().set_colors(Eigen::RowVector3d(0.9, 0.1, 0.1));
}

void SandBox::SetQueue(Eigen::MatrixXd& V, Eigen::MatrixXi &F)
{
	Eigen::MatrixXi Etmp,EFtmp, EItmp;
	Eigen::VectorXi EMAPtmp;
	PriorityQueue *Qtmp = new PriorityQueue();
	std::vector<PriorityQueue::iterator> QitTmp;
	igl::edge_flaps(F, Etmp, EMAPtmp, EFtmp, EItmp);
	QitTmp.resize(Etmp.rows());

	C.push_back(Eigen::MatrixXd(Etmp.rows(), V.cols()));
	Eigen::VectorXd costs(Etmp.rows());
	Qtmp->clear();
	for (int e = 0; e < Etmp.rows(); e++)
	{

		double cost = e;
		Eigen::RowVectorXd p(1, 3);
		QuadricCost(e, V, F, Etmp, EMAPtmp, EFtmp, EItmp, cost, p);
		C[selected_data_index].row(e) = p;
		QitTmp[e] = Qtmp->insert(std::pair<double, int>(cost, e)).first;
	}

	EMAP.push_back(EMAPtmp);
	E.push_back(Etmp);
	EF.push_back(EFtmp);
	EI.push_back(EItmp);
	Q.push_back(Qtmp);
	Qit.push_back(QitTmp);
	num_collapsed.push_back(0);
}

SandBox::~SandBox()
{
	if (Q.size() > 0)
	{
		for (PriorityQueue* q : Q)
			delete q;
	}
}

bool SandBox::CheckCollision(int indx1,int indx2, const Eigen::Matrix4d& mat1, const Eigen::Matrix4d& mat2)
{
	bool isCollide = false;
	Eigen::Matrix3d rotA = mat1.block<3, 3>(0, 0);
	Eigen::Matrix3d rotB = mat2.block<3, 3>(0, 0);
	Eigen::Matrix3d rotC = rotA.transpose() * rotB;
	Eigen::Matrix3d rotCabs = rotC.cwiseAbs();
	std::function<bool( igl::AABB<Eigen::MatrixXd, 3>*, igl::AABB<Eigen::MatrixXd, 3>* )> recCheck;
	
	igl::AABB<Eigen::MatrixXd, 3> * tree1 = trees[indx1];
	igl::AABB<Eigen::MatrixXd, 3> * tree2 = trees[indx2];

	igl::opengl::ViewerData *data1 = &data_list[indx1];
	igl::opengl::ViewerData *data2 = &data_list[indx2];

    recCheck = [ data1,data2,mat1,mat2, rotA, rotB, rotC,rotCabs,&recCheck](igl::AABB<Eigen::MatrixXd, 3> * tree1, igl::AABB<Eigen::MatrixXd, 3> * tree2){

		Eigen::AlignedBox<double, 3> box1 = tree1->m_box;
		Eigen::AlignedBox<double, 3> box2 = tree2->m_box;

		Eigen::Vector4d center1(0, 0, 0, 1);
		center1 << box1.center(), 1;
		center1 = mat1 * center1;
		Eigen::Vector4d center2(0, 0, 0, 1);
		center2 << box2.center(), 1;
		center2 = mat2 * center2;
		Eigen::Vector3d sizes1 = box1.sizes()/2.0;
		Eigen::Vector3d sizes2 = box2.sizes()/2.0;
		Eigen::Vector3d vDiff = (center2 - center1).head(3);

		double R0, R1, R;
		bool separatable = false;
		//first 3 conditions
		for (size_t i = 0; i < 3 && !separatable; i++)
		{
			R0 = sizes1(i);
			R1 = rotCabs.row(i).dot(sizes2);
			R = abs(rotA.col(i).dot(vDiff));
			separatable = (R > R0 + R1);
		}

		//second 3 conditions
		for (size_t i = 0; i < 3 && !separatable; i++)
		{
			R1 = sizes2(i);
			R0 = rotCabs.col(i).dot(sizes1);
			R = abs(rotB.col(i).dot(vDiff));
			separatable = (R > R0 + R1);
		}
		//TODO: Finish 9 last conditions
		//for (size_t i = 0; i < 3 && !separatable; i++)
		//{
		//	int indx1 = (i + 1) % 3, indx2 = (2 - i) % 3;
		//	Eigen::Vector3d tmp1 = sizes1(indx1) * rotCabs.row(indx2) + sizes1(indx2) * rotCabs.row(indx1);
		//	
		//	for (size_t j = 0; j < 3 && !separatable; j++)
		//	{
		//		R0 = 0;
		//		R1 = 0;
		//		R  = 0;
		//		for (size_t k = 0; k < 3; k++)
		//		{
		//			if (k != i)
		//			{
		//				R0 += sizes1(k)*;
		//			}
		//		}
		//		R1 = sizes2(i);
		//		R0 = rotCabs.col(i).dot(sizes1);
		//		R = abs(rotB.col(i).dot(vDiff));
		//		separatable = (R > R0 + R1);
		//	}
		//}

		if (separatable || (tree1->is_leaf() && tree2->is_leaf()))
		{
			if (!separatable)
			{
				AddBox(*data1,tree1->m_box, Eigen::Vector3d(1, 1, 1));
				AddBox(*data2,tree2->m_box, Eigen::Vector3d(1, 1, 1));

			}
			return separatable;
		}
		if (tree1->is_leaf())
		{
			separatable = recCheck(tree1,tree2->m_left) && recCheck(tree1, tree2->m_right);
		}
		else 
			if (tree2->is_leaf())
			{
				separatable = recCheck(tree1->m_left, tree2) && recCheck(tree1->m_right, tree2);
			}
			else
			{
				separatable = recCheck(tree1->m_right, tree2->m_left) && recCheck(tree1->m_right, tree2->m_right) && recCheck(tree1->m_left, tree2->m_left) && recCheck(tree1->m_left, tree2->m_right);

			}
		return separatable;
	};
	isCollide = !recCheck(tree1,tree2);
	return isCollide;
}

void SandBox::ResetScene(bool onStart)
{
	
	for (size_t i = 0; i < data_list.size(); i++)
	{
		if (!onStart)
		{
			data_list[i].ResetTranslation();
			data_list[i].set_edges(Eigen::MatrixXd(0,3), Eigen::MatrixXi(0,2), Eigen::MatrixXd(0,3));
			AddBox(data_list[i], trees[i]->m_box, Eigen::Vector3d(0, 1, 0));
		}
		else
		{
			velocities.push_back(Eigen::Vector3d(-0.02 * (i - 0.5), 0, 0));
			
			
		}
		data_list[i].MyTranslate(Eigen::Vector3d(i - 0.5, 0, 0), true);
		
	}
}

void SandBox::ResetBoxes()
{
	if(isActive)
	for (size_t i = 0; i < data_list.size(); i++)
	{

		data_list[i].set_edges(Eigen::MatrixXd(0, 3), Eigen::MatrixXi(0, 2), Eigen::MatrixXd(0, 3));
		AddBox(data_list[i], trees[i]->m_box, Eigen::Vector3d(0, 1, 0));
	}
}

void SandBox::DrawBestAxis(bool toRotate)
{
	//Eigen::MatrixXd S = data().V.transpose() * data().V;
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(data().V, Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::RowVector3d v1 = svd.matrixV().transpose().row(0).normalized();
	Eigen::RowVector3d v2 = svd.matrixV().transpose().row(1).normalized();
	Eigen::RowVector3d v3 = svd.matrixV().transpose().row(2).normalized();
	Eigen::RowVector3d center = trees.back()->m_box.center().transpose();
	data().add_edges(center - v1/2.0, center + v1/2.0, Eigen::RowVector3d(1, 0, 0));
	data().add_edges(center - v2/2.0, center + v2/2.0, Eigen::RowVector3d(0, 1, 0));
	data().add_edges(center - v3/2.0, center + v3/2.0, Eigen::RowVector3d(0, 0, 1));
	if (toRotate)
		data().MyRotate(svd.matrixV());
}


void SandBox::Animate()
{
	if (isActive)
	{
		
		data().MyTranslate( velocities[selected_data_index], true);
		for (size_t i = 0; i < data_list.size(); i++)
		{
			for (size_t j = i+1; j < data_list.size(); j++)
			{
				if (CheckCollision(i, j, data_list[i].MakeTransScaled(), data_list[j].MakeTransScaled()))
					SetAnimation();
			}
		}
		
	}
}

//void SandBox::QuadricCost(
//	const int e,
//	double& cost,
//	Eigen::RowVectorXd& p)
//{
//	cost = (V.row(E(e, 0)) - V.row(E(e, 1))).norm();
//	p = 0.5 * (V.row(E(e, 0)) +  V.row(E(e, 1)));
//}

