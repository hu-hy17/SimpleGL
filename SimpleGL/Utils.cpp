#include"Utils.h"
#include<cmath>

Eigen::Vector3f getRotAngle(const Eigen::Matrix3f& from_rot_mat, const Eigen::Matrix3f& to_rot_mat)
{
	Eigen::Matrix3f tmp = from_rot_mat.inverse();
	Eigen::Matrix3f rot_mat = tmp * to_rot_mat;
	return rot_mat.eulerAngles(0, 1, 2);
}

Eigen::Matrix3f getRotMat(const Eigen::Vector3f& from_vec, const Eigen::Vector3f& to_vec)
{
	Eigen::Quaternionf quat;
	return quat.setFromTwoVectors(from_vec, to_vec).toRotationMatrix();
}

QMatrix4x4 getQMatFromJson(const Json::Value& vals)
{
	assert(vals.size() == 16);
	QMatrix4x4 qmat;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			qmat(i, j) = vals[4 * i + j].asFloat();
	}
	return qmat;
}

QVector3D getVec3FromJson(const Json::Value& vals)
{
	assert(vals.size() == 3);
	QVector3D ret;
	for (int i = 0; i < 3; i++)
		ret[i] = vals[i].asFloat();
	return ret;
}

QVector4D getVec4FromJson(const Json::Value& vals)
{
	assert(vals.size() == 4);
	QVector4D ret;
	for (int i = 0; i < 4; i++)
		ret[i] = vals[i].asFloat();
	return ret;
}