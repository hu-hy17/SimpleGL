#pragma once

#include<QVector3D>
#include<QVector4D>
#include<QMatrix3x3>
#include<QMatrix4x4>
#include<Eigen/Dense>
#include<json/json.h>

/**
@brief �����һ����תλ�˵���һ����תλ��֮�����ת��
@param from_rot_mat: ��ʼ��תλ��
@param to_rot_mat: Ŀ����תλ��
@param rot_x, rot_y, rot_z: �ֱ𴢴�xyz�����ת��
@return ��ת��
*/
Eigen::Vector3f getRotAngle(const Eigen::Matrix3f& from_rot_mat, const Eigen::Matrix3f& to_rot_mat);

/**
@brief �����һ��������������һ����������֮�����ת��
@param from_vec: ��ʼ��������
@param to_vec: Ŀ�귽������
@return 3x3��ת����
*/
Eigen::Matrix3f getRotMat(const Eigen::Vector3f& from_vec, const Eigen::Vector3f& to_vec);

QMatrix4x4 getQMatFromJson(const Json::Value& vals);
QVector3D getVec3FromJson(const Json::Value& vals);
QVector4D getVec4FromJson(const Json::Value& vals);