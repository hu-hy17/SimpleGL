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
	QVector4D m_color;					// ��ɫ
	QOpenGLShaderProgram m_shader;		// ��ɫ��
	QOpenGLBuffer m_vbo, m_ebo;
	QOpenGLVertexArrayObject m_vao;
	QMatrix4x4 m_local_to_global;		// �ֲ�����ϵ��ȫ������ϵת������

	bool m_open_light = false;
	std::shared_ptr<GLLight> m_light;	// ����
	std::shared_ptr<Camera> m_camera;	// �����
	std::shared_ptr<GLMaterial> m_material;		// ����

protected:
	/**
	@brief ��������Ӷ�����ɫ����Ƭ����ɫ��
	@param class_name: ���øú����������������
	@param vert_shader:������ɫ���ļ�����
	@param frag_shader:Ƭ����ɫ���ļ�����
	*/
	void _compileAndLinkShader_(
		const std::string& class_name,
		const std::string& vert_shader,
		const std::string& frag_shader);
	/**
	@brief �����ջ����Լ�������ɫ���õ���ɫ����
	*/
	void _setLightCond_();

public:
	GLObject();
	~GLObject();

	/**
	@brief ����vbo,ebo,vao,�뱣֤���øú�������ʹ�øö���
	*/
	virtual void destroy();

	/**
	@brief ���ü������rgba��ɫ
	@param color: ��ɫ����,����ΪRed,Green,Blue,Alpha
	*/
	void setColor(const QVector4D& color);

	/**
	@brief ���þֲ�����ϵ����������ϵ��ת�ƾ���
	@param mat: ת�ƾ���
	*/
	void setLocToGlob(const QMatrix4x4& mat);

	/**
	@brief �������
	@param camera: ������ָ��
	*/
	void setCamera(std::shared_ptr<Camera> camera);

	/**
	@brief ���ù���
	@param light: �������ָ��
	*/
	void setLight(std::shared_ptr<GLLight> light);

	/**
	@brief ���ò���
	*/
	void setMaterial(std::shared_ptr<GLMaterial> material);

	/**
	@brief ��ȡ����
	*/
	std::shared_ptr<GLMaterial> getMaterial();

	/**
	@brief ����һ��Ĭ�ϵļ�����
	*/
	virtual void paint() = 0;

	/**
	@brief ���һ�����������뼸����Ľ���
	@param ray : ����
	@param hitpoint : ���潻��
	@return �����뵱ǰ�������Ƿ��ཻ�����������û��ʵ�ָ÷���������false
	*/
	virtual bool intersectPoint(const GLRay& ray, GLHitPoint& hitpoint);

	/**
	@brief ��ײ���
	*/
	virtual bool collisionDetect(const QVector3D& pos);

	/**
	@brief ��ȡ��������������
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
	 * @brief �󽻺������������GLObjectͬ������
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
	// TODO: ׶��ķ���������������
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
	@brief ��ȡ��������պн����Ӧ����ɫֵ
	@param ray : ����
	@return ��������պн��㴦����ɫֵ
	*/
	QVector3D getRayColor(const GLRay& ray);
};