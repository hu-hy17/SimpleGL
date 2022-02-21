#pragma once
#include"GLObject.h"

class GLMixObject
{
protected:
	const std::string m_class_name = "(GLMixObject)";
	std::vector<std::unique_ptr<GLObject>> m_globjects;

public:
	GLMixObject();
	void destroy();
	/**
	@brief 设置几何体的rgba颜色
	@param color: 颜色参数,依次为Red,Green,Blue,Alpha
	*/
	void setColor(const QVector4D& color);

	/**
	@brief 设置局部坐标系到世界坐标系的转移矩阵
	@param mat: 转移矩阵
	*/
	virtual void setLocToGlob(const QMatrix4x4& mat);

	/**
	@brief 设置相机
	@param camera: 相机类的指针
	*/
	void setCamera(std::shared_ptr<Camera> camera);

	/**
	@brief 设置光照
	@param light: 光照类的指针
	*/
	void setLight(std::shared_ptr<GLLight> light);

	/**
	@brief 设置材质
	@param material : 材质类的指针
	*/
	void setMaterial(std::shared_ptr<GLMaterial> material);

	virtual void paint() = 0;
};

class GLArrow : public GLMixObject
{
protected:
	const std::string m_class_name = "(GLArrow)";
private:
	const float m_len_ratio = 0.9f;
	const float m_radius_ratio = 3.0f;
public:
	GLArrow(int steps = 100);
	void paint();
	void paint(float radius, float length, QVector3D direction, QVector3D start_pos);
};

class GLCubeFrameWork : public GLMixObject
{
protected:
	const std::string m_class_name = "(GLCubeFrameWork)";

private:
	const QVector4D m_cano_start_pos[12]{
		{ -1, -1, -1, 1 },
		{ -1, -1, -1, 1 },
		{ -1, -1, -1, 1 },
		{ -1, -1,  1, 1 },
		{ -1, -1,  1, 1 },
		{  1, -1,  1, 1 },
		{  1, -1,  1, 1 },
		{ -1,  1,  1, 1 },
		{ -1,  1,  1, 1 },
		{  1,  1, -1, 1 },
		{  1,  1, -1, 1 },
		{  1,  1, -1, 1 },
	};
	const QVector4D m_cano_dir[12]{
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0,-1, 0 },
		{ 1, 0, 0, 0 },
		{ 0, 0,-1, 0 },
		{ -1, 0, 0, 0 },
		{ 0,-1, 0, 0 },
		{ 0, 0, 1, 0 }
	};
	const float m_len_ratio = 0.3f;
	const float m_radius = 1.0f;

public:
	GLCubeFrameWork();
	void setColor(QVector4D& center_color, QVector4D& edge_color, QVector4D& plane_color);
	void paint();
	void paint(float edge_len, const QMatrix4x4& motion);
};