#include "tutorial/IK/IK.h"

IK::IK()
{
	scaleFactor = Eigen::Vector3d(1, 0.5, 1);

}

void IK::Init(const std::string &config)
{
	std::string item_name;
	std::ifstream nameFileout;

	nameFileout.open(config);
	if (!nameFileout.is_open())
	{
		std::cout << "Can't open file "<<config << std::endl;
	}
	else
	{
		nameFileout >> item_name;
		std::cout << "openning " << item_name << std::endl;
		load_mesh_from_file(item_name);
		
		SetParent(0, -1);
		data().show_overlay_depth = false;
		data().show_texture = true;
		data().show_lines = false;
		data().face_based = false;
		data().point_size = 10;
		data().line_width = 3;
		data().MyScale(scaleFactor);
		double s = (1 + scaleFactor[1])/2.0;
		data().MyTranslate(Eigen::Vector3d(0, cylinderLength/2.0, 0), true);
		data().SetCenterOfRotation(Eigen::Vector3d(0, -cylinderLength / 2.0, 0));

		data().add_points(data().GetCenterOfRotation().transpose() *(1.0 / scaleFactor[1]), Eigen::RowVector3d(0, 0, 1));
		data().set_face_based(!data().face_based);

		for(int i=1;i<linksNum;i++)
		{
			append_mesh(false);
			data().set_mesh(data_list[0].V, data_list[0].F);
			data().set_uv(data_list[0].V_uv*1.5, data_list[0].F_uv);
			SetParent(i, i - 1);
			data().show_overlay_depth = false;
			data().show_texture = true;
			data().show_lines = false;
			data().face_based = false;
			data().point_size = 10;
			data().line_width = 3;
			data().MyScale(scaleFactor);
	
			data().MyTranslate(Eigen::Vector3d(0, cylinderLength*s, 0),true);
			data().SetCenterOfRotation(Eigen::Vector3d(0, -cylinderLength / 2.0 , 0));
			
			data().add_points(data().GetCenterOfRotation().transpose()*(1.0/scaleFactor[1]) , Eigen::RowVector3d(0, 0, 1));
			
			AddAxis();
			
		}
		nameFileout >> item_name;
		std::cout << "openning " << item_name << std::endl;
		load_mesh_from_file(item_name);

		SetParent(linksNum,-1);
		data().show_overlay_depth = false;
		//data().show_texture = true;
		data().point_size = 10;
		data().MyTranslate(Eigen::Vector3d(5, 0, 0),true);
		nameFileout.close();
		MyTranslate(Eigen::Vector3d(0, 0, -12.0), true);
		
		data().set_colors(Eigen::RowVector3d(0.9, 0.1, 0.1));
	//	for (int i = 1; i < data_list.size(); i++)
	//		parents[i] = i - 1;
		lastLink = linksNum - 1;
		firstLink = 0;
		dest = linksNum;
		tipPosition = GetTipPositionInSystem(lastLink);
		std::cout << "tip pos " << tipPosition.transpose();
		destPosition = (data_list[dest].MakeTransd()*Eigen::Vector4d(0,0,0,1)).head(3);
	}
	
}

void IK::SetAxis(double size)
{
}

Eigen::Vector3d IK::GetTipPositionInSystem(int link)
{
	Eigen::Vector4d vecToTip = Eigen::Vector4d(0,  cylinderLength, 0, 0);
	int i = link;
	
	for (; parents[i] > -1; i = parents[i])
	{
		vecToTip = data_list[i].MakeTransd() * vecToTip + Eigen::Vector4d(0, cylinderLength, 0, 0);
	}
	Eigen::Vector4d origin =  data_list[i].MakeTransd() * Eigen::Vector4d(0, -cylinderLength* scaleFactor[1] /2.0, 0, 1);
	return origin.head(3) + (data_list[i].MakeTransd() * vecToTip).head(3)* scaleFactor[1];
}

Eigen::Vector3d IK::GetJointPositionInSystem(int link)
{
	Eigen::Vector4d vecToJoint = Eigen::Vector4d(0, 0, 0, 0);
	
	int i = link;
	for (; parents[i] > -1; i = parents[i])
	{
		vecToJoint = data_list[i].MakeTransd() * vecToJoint + Eigen::Vector4d(0, cylinderLength, 0, 0);
	}
	Eigen::Vector4d origin = data_list[i].MakeTransd() * Eigen::Vector4d(0, -cylinderLength* scaleFactor[1] / 2.0, 0, 1);
	return origin.head(3) + (data_list[i].MakeTransd() * vecToJoint).head(3)* scaleFactor[1];
}


void IK::CalcStep()
{
	int i = lastLink;
	for(; i > -1; i = parents[i])
	{
		Eigen::Vector3d jointPos = GetJointPositionInSystem(i);
		Eigen::Vector3d vec1 = (destPosition - jointPos);
		Eigen::Vector3d vec2 = (tipPosition - jointPos);
		double angle1 = acosf(std::min(1.0,(std::max(-1.0,vec1.normalized().dot(vec2.normalized())))));
		Eigen::Vector3d n = vec2.cross(vec1);
		Eigen::Vector3d tmp = n.cross(vec1);
		double angle2 = acosf(std::min(1.0, (std::max(-1.0, n.normalized().dot(Eigen::Vector3d(1,0,0))))));

		//std::cout << "angle " << angle << std::endl;
		Eigen::Matrix4d mat = Eigen::Matrix4d::Identity();
		for (int j = i; j > -1; j = parents[j])
			mat = mat * data_list[j].MakeTransd();
		data_list[i].RotateInSystem(mat.block<3, 3>(0, 0), n, angle1 / 20.0);
		tipPosition = GetTipPositionInSystem(lastLink);
	}
	
}

void IK::CalcStepQuaternions()
{
	int i = lastLink;
	for (; i > -1; i = parents[i])
	{
		Eigen::Vector3d jointPos = GetJointPositionInSystem(i);
		Eigen::Vector3d vec1 = (destPosition - jointPos);
		Eigen::Vector3d vec2 = (tipPosition - jointPos);
		Eigen::Quaterniond quat = Eigen::Quaterniond::FromTwoVectors(vec1,vec2);
		//std::cout << "quaternion " << quat << std::endl;
		
		Eigen::Matrix4d mat = Eigen::Matrix4d::Identity();
		for (int j = i; j > -1; j = parents[j])
			mat = mat * data_list[j].MakeTransd();
		data_list[i].RotateInSystem(mat.block<3, 3>(0, 0), quat);
		tipPosition = GetTipPositionInSystem(lastLink);
	}
}

void IK::MakeChange()
{
	CalcStep();
	std::cout << "distance " << GetDistance() << std::endl;
}

void IK::Animate()
{
	if (isActive)
	{
		if ((GetJointPositionInSystem(0) - destPosition).norm() <= linksNum * cylinderLength * scaleFactor[1])
		{
			MakeChange();
			if (GetDistance() < delta)
			{
				EulerAngles();
				SetAnimation();
			}
		}
		else
		{
			std::cout << "can not reach!\n";
			SetAnimation();
		}
		//Eigen::Vector3d tmp1 = GetTipPositionInSystem(lastLink);
		//Eigen::Vector3d tmp2 = (data_list[linksNum].MakeTransd() * Eigen::Vector4d(0, 0, 0, 1)).head(3);
		//data_list[linksNum].MyTranslate(tmp1-tmp2,true);

		//std::cout <<"tip  "<< tmp1.transpose() << std::endl;
		//std::cout << "sphere  "<<tmp2.transpose() << std::endl;
		//SetAnimation();
	}
	
}

void IK::WhenTranslate()
{
	if (selected_data_index == dest)
		destPosition = (data().MakeTransd() * Eigen::Vector4d(0, 0, 0, 1)).head(3);
	else
		tipPosition = GetTipPositionInSystem(lastLink);
}

void IK::ResetArm()
{
	for (int i = lastLink; i > -1; i = parents[i])
	{
		data_list[i].ResetRotation();
	}
}

void IK::EulerAngles()
{
	Eigen::Matrix3d rot = Eigen::Matrix3d::Identity();
	int i = firstLink;
	for (; i<children.size() && !children[i].empty(); i = children[i][0])
	{
		data_list[i].RotateInSystem(data_list[i].GetRotation(),rot* data_list[i].GetRotation());
		//data_list[i].MyRotate(rot);
		rot = data_list[i].ConvertRotationToEuler();
	}
	if (i == lastLink)
	{
		data_list[i].RotateInSystem(data_list[i].GetRotation(), rot * data_list[i].GetRotation());
		data_list[i].ConvertRotationToEuler();
	}
	//std::cout << "rot mat:\n" << rot << std::endl;
	//std::cout << "det mat:\n" << rot.determinant() << std::endl;

}


void IK::SetParent(int childIndx, int parentIndx)
{
	if (parents.size() <= childIndx)
		parents.push_back(parentIndx);
	else
		parents[childIndx] = parentIndx;
	int s = children.size();
	if (s <= parentIndx)
	{
		std::vector<int> tmp;
		tmp.push_back(childIndx);
		children.push_back(tmp);
	}
	else if(parentIndx>-1)
	{
		children[parentIndx].push_back(childIndx);
	}
}

Eigen::Vector3d IK::GetCameraPosition()
{
	return GetTipPositionInSystem(lastLink);
}

Eigen::Vector3d IK::GetCameraForward()
{
	return Eigen::Vector3d(0,0,-1);
}

Eigen::Vector3d IK::GetCameraUp()
{
	return Eigen::Vector3d(0,1,0);
}

IK::~IK()
{
	
}

void IK::AddAxis()
{
	int savedIndx = selected_data_index;
	selected_data_index = parents[savedIndx];
	data().add_edges(Eigen::RowVector3d(-cylinderLength, cylinderLength / 2.0, 0), Eigen::RowVector3d(cylinderLength, cylinderLength / 2.0, 0), Eigen::RowVector3d(1, 0, 0));
	data().add_edges(Eigen::RowVector3d(0, -cylinderLength + cylinderLength / 2.0, 0), Eigen::RowVector3d(0, cylinderLength + cylinderLength / 2.0, 0), Eigen::RowVector3d(0, 1, 0));
	data().add_edges(Eigen::RowVector3d(0, cylinderLength / 2.0, -cylinderLength), Eigen::RowVector3d(0, cylinderLength / 2.0, cylinderLength), Eigen::RowVector3d(0, 0, 1));
	selected_data_index = savedIndx;
}



