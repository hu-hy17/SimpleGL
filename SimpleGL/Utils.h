#pragma once

#include<QVector3D>
#include<QVector4D>
#include<QMatrix3x3>
#include<QMatrix4x4>
#include<Eigen/Dense>
#include<json/json.h>

/**
@brief 计算从一个旋转位姿到另一个旋转位姿之间的旋转角
@param from_rot_mat: 初始旋转位姿
@param to_rot_mat: 目标旋转位姿
@param rot_x, rot_y, rot_z: 分别储存xyz轴的旋转角
@return 旋转角
*/
Eigen::Vector3f getRotAngle(const Eigen::Matrix3f& from_rot_mat, const Eigen::Matrix3f& to_rot_mat);

/**
@brief 计算从一个方向向量到另一个方向向量之间的旋转角
@param from_vec: 初始方向向量
@param to_vec: 目标方向向量
@return 3x3旋转矩阵
*/
Eigen::Matrix3f getRotMat(const Eigen::Vector3f& from_vec, const Eigen::Vector3f& to_vec);

QMatrix4x4 getQMatFromJson(const Json::Value& vals);
QVector3D getVec3FromJson(const Json::Value& vals);
QVector4D getVec4FromJson(const Json::Value& vals);