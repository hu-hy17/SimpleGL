#pragma once
#pragma once

#include<string>
#include<memory>

#include<QOpenGLShader>
#include<QOpenGLShaderProgram>
#include<QOpenGLVertexArrayObject>
#include<QOpenGLFramebufferObject>
#include<QOpenGLBuffer>
#include<QOpenGLFunctions>
#include<QOpenGLTexture>

#include<QVector>
#include<QMatrix4x4>
#include<QImage>
#include<QString>

#include"Defs.h"
#include"GLLight.h"
#include"RayTracing.h"
#include"Camera.h"

class GLObject : protected QOpenGLFunctions
{
protected:
	const std::string m_class_name = "(GLObject)";
	QVector4D m_color;					// 颜色
	QOpenGLShaderProgram m_shader;		// 着色器
	QOpenGLBuffer m_vbo, m_ebo;
	QOpenGLVertexArrayObject m_vao;
	QMatrix4x4 m_local_to_global;		// 局部坐标系到全局坐标系转换矩阵

	bool m_open_light = false;
	std::shared_ptr<GLLight> m_light;	// 光照
	std::shared_ptr<Camera> m_camera;	// 摄像机
	std::shared_ptr<GLMaterial> m_material;		// 材质

protected:
	/**
	@brief 编译和链接顶点着色器和片段着色器
	@param class_name: 调用该函数的派生类的名称
	@param vert_shader:顶点着色器文件名称
	@param frag_shader:片段着色器文件名称
	*/
	void _compileAndLinkShader_(
		const std::string& class_name,
		const std::string& vert_shader,
		const std::string& frag_shader);
	/**
	@brief 将光照环境以及物体颜色设置到着色器中
	*/
	void _setLightCond_();

public:
	GLObject();
	~GLObject();

	/**
	@brief 销毁vbo,ebo,vao,请保证调用该函数后不再使用该对象
	*/
	virtual void destroy();

	/**
	@brief 设置几何体的rgba颜色
	@param color: 颜色参数,依次为Red,Green,Blue,Alpha
	*/
	void setColor(const QVector4D& color);

	/**
	@brief 设置局部坐标系到世界坐标系的转移矩阵
	@param mat: 转移矩阵
	*/
	void setLocToGlob(const QMatrix4x4& mat);

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
	*/
	void setMaterial(std::shared_ptr<GLMaterial> material);

	/**
	@brief 获取材质
	*/
	std::shared_ptr<GLMaterial> getMaterial();

	/**
	@brief 绘制一个默认的几何体
	*/
	virtual void paint() = 0;

	/**
	@brief 求解一个给定光束与几何体的交点
	@param ray : 光线
	@param hitpoint : 保存交点
	@return 光线与当前几何体是否相交，如果派生类没有实现该方法，返回false
	*/
	virtual bool intersectPoint(const GLRay& ray, GLHitPoint& hitpoint);

	/**
	@brief 碰撞检测
	*/
	virtual bool collisionDetect(const QVector3D& pos);

	/**
	@brief 获取几何体类型名称
	*/
	virtual std::string getObjTypeName();
};

class GLCube : public GLObject
{
protected:
	const std::string m_class_name = "(GLCube)";

public:
	GLCube();
	void paint();
	void paint(float len_x, float len_y, float len_z);
};

class GLSphere : public GLObject
{
protected:
	const std::string m_class_name = "(GLSphere)";
	int m_indices_size;
	float m_radius = 1.0f;
	QVector3D m_center_pos = QVector3D(0, 0, 0);

public:
	GLSphere(int steps = 100);
	void paint();
	void paint(const QVector3D& pos, float radius);
	void setRadius(float r);
	void setCenterPos(const QVector3D& pos);

	/**
	 * @brief 求交函数，详见基类GLObject同名函数
	*/
	bool intersectPoint(const GLRay& ray, GLHitPoint& hitpoint);
	bool collisionDetect(const QVector3D& pos);
	std::string getObjTypeName();
};

class GLPoint : public GLSphere
{
protected:
	const std::string m_class_name = "(GLPoint)";
public:
	GLPoint();
};

class GLCylinder : public GLObject
{
protected:
	const std::string m_class_name = "(GLCylinder)";
	int m_indices_size;

public:
	GLCylinder(int steps = 100);
	void paint();
	void paint(float radius, float length, QVector3D direction, QVector3D start_pos);
};

class GLCone : public GLObject
{
	// TODO: 锥体的法向量计算有问题
protected:
	const std::string m_class_name = "(GLCone)";
	int m_indices_size;
public:
	GLCone(int steps = 100);
	void paint();
	void paint(float radius, float length, QVector3D direction, QVector3D start_pos);
};

class GLMirror : public GLObject
{
protected:
	const std::string m_class_name = "(GLMirror)";
	float m_width = 0;
	float m_height = 0;
public:
	GLMirror();
	void setSize(float width, float height);
	void paint();
	void paint(const QOpenGLFramebufferObject* fbo);
	float getWidth();
	float getHeight();
	bool intersectPoint(const GLRay& ray, GLHitPoint& hitpoint);
};

class GLSkyBox : public GLObject
{
protected:
	const std::string m_class_name = "(GLSkyBox)";
private:
	QImage m_imgs[6];
	std::unique_ptr<QOpenGLTexture> m_texture;
public:
	GLSkyBox(const std::vector<QString>& img_filenames);
	void paint();

	/**
	@brief 获取光线与天空盒交点对应的颜色值
	@param ray : 光线
	@return 光线与天空盒交点处的颜色值
	*/
	QVector3D getRayColor(const GLRay& ray);
};