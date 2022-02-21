#include"GLObject.h"
#include"Utils.h"

#include<iostream>

using std::cout;
using std::cerr;
using std::endl;

#define M_PI 3.1415926

/************************************************************************/
/* GLObject                                                             */
/************************************************************************/
GLObject::GLObject()
{
	m_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	m_ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	this->initializeOpenGLFunctions();
}

GLObject::~GLObject()
{
	if (m_camera)
		m_camera.reset();
	if (m_light)
		m_light.reset();
}

void GLObject::destroy()
{
	m_vbo.destroy();
	m_ebo.destroy();
	m_vao.destroy();
}

bool GLObject::intersectPoint(const GLRay& ray, GLHitPoint& hitpoint)
{
	return false;
}

void GLObject::setColor(const QVector4D& color)
{
	m_color = color;
}

void GLObject::setLocToGlob(const QMatrix4x4& mat)
{
	m_local_to_global = mat;
}

void GLObject::setLight(std::shared_ptr<GLLight> light)
{
	if (m_light != nullptr)
		m_light.reset();
	if (!light)
	{
		m_open_light = false;
		return;
	}
	m_light = light;
	m_open_light = true;
}

void GLObject::setCamera(std::shared_ptr<Camera> camera)
{
	if (m_camera != nullptr)
		m_camera.reset();
	m_camera = camera;
}

void GLObject::setMaterial(std::shared_ptr<GLMaterial> material)
{
	if (m_material != nullptr)
		m_material.reset();
	m_material = material;
}

std::shared_ptr<GLMaterial> GLObject::getMaterial()
{
	return m_material;
}

bool GLObject::collisionDetect(const QVector3D& pos)
{
	return false;
}

std::string GLObject::getObjTypeName()
{
	return "SimpleObject";
}

void GLObject::_compileAndLinkShader_(
	const std::string& class_name,
	const std::string& vert_shader,
	const std::string& frag_shader)
{
	// 着色器编译和链接
	bool success = m_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, vert_shader.c_str());
	if (!success)
	{
		cerr << class_name << "Error: ShaderProgram addShaderFromSourceFile failed!" << endl;
		goto fail;
	}

	success = m_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, frag_shader.c_str());
	if (!success)
	{
		cerr << class_name << "Error: ShaderProgram addShaderFromSourceFile failed!" << endl;
		goto fail;
	}

	success = m_shader.link();
	if (!success)
	{
		cerr << class_name << "Error: shaderProgram link failed!" << endl;
		goto fail;
	}

	return;

fail:
	system("pause");
	exit(-1);
}

void GLObject::_setLightCond_()
{
	// 设置是否开启光照,几何体颜色,观察者位置
	m_shader.setUniformValue(san::ATTR_OPEN_LIGHT, m_open_light);
	m_shader.setUniformValue(san::ATTR_OBJ_COLOR, QVector3D(m_color));
	m_shader.setUniformValue(san::ATTR_OBJ_COLOR_ALPHA, m_color[3]);
	m_shader.setUniformValue(san::ATTR_VIEW_POS, m_camera->position);

	if (!m_open_light)
		return;

	// 设置材质参数
	m_shader.setUniformValue(san::ATTR_AMBIENT_STR, m_material->m_ka);
	m_shader.setUniformValue(san::ATTR_DIFFUSE_STR, m_material->m_kd);
	m_shader.setUniformValue(san::ATTR_SPECULAR_STR, m_material->m_ks);
	m_shader.setUniformValue(san::ATTR_SHININESS, m_material->m_shininess * 128.0f);

	// 设置光源参数
	m_light->setLightCond(m_shader);
}

/************************************************************************/
/* GLCube                                                               */
/************************************************************************/

GLCube::GLCube()
{
	_compileAndLinkShader_(m_class_name, ":shader/resources/shader/cube.vert", ":shader/resources/shader/simpleObj.frag");

	const float normVal = 1 / sqrtf(3);
	// VAO，VBO数据部分
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	// 0
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	// 1
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	// 2
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,	// 3

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	// 4
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	// 5
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	// 6
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,	// 7

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	// 8
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	// 9
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,	// 10
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,	// 11

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	// 12
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	// 13
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,	// 14
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,	// 15

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	// 16
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,	// 17
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	// 18
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,	// 19

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	// 20
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	// 21
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,	// 22
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f	// 23
	};
	unsigned int indices[] = {  // note that we start from 0!
		 0,  1,  2,	  0,  2,  3,
		 4,  5,  6,	  4,  6,  7,
		 8,  9, 10,	  8, 10, 11,
		12, 13, 14,	 12, 14, 15,
		16, 17, 18,	 16, 18, 19,
		20, 21, 22,	 20, 22, 23,
	};

	QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(vertices, sizeof(vertices));

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(indices, sizeof(indices));

	// 坐标
	int attr = -1;
	attr = m_shader.attributeLocation(san::ATTR_POSITION);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
	m_shader.enableAttributeArray(attr);

	// 法向量
	attr = m_shader.attributeLocation(san::ATTR_NORMAL);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
	m_shader.enableAttributeArray(attr);
	m_vbo.release();
}

void GLCube::paint()
{
	paint(1.0f, 1.0f, 1.0f);
}

void GLCube::paint(float len_x, float len_y, float len_z)
{
	m_shader.bind();
	_setLightCond_();

	// 边长
	m_shader.setUniformValue(san::ATTR_LENGTH_X, len_x);
	m_shader.setUniformValue(san::ATTR_LENGTH_Y, len_y);
	m_shader.setUniformValue(san::ATTR_LENGTH_Z, len_z);

	m_shader.setUniformValue(san::ATTR_PROJ_MAT, m_camera->getProjectionMatrix());	// 投影矩阵
	m_shader.setUniformValue(san::ATTR_VIEW_MAT, m_camera->getViewMatrix());		// 摄像机矩阵
	m_shader.setUniformValue(san::ATTR_LOC_TO_GLOB_MAT, m_local_to_global);			// 局部到全局坐标转换
	{
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);
		glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, 0);
	}
	m_shader.release();
}

/************************************************************************/
/* GLPoint                                                              */
/************************************************************************/

GLPoint::GLPoint() :
	GLSphere(10)
{ }

/************************************************************************/
/* GLSphere                                                             */
/************************************************************************/

GLSphere::GLSphere(int steps)
{
	_compileAndLinkShader_(m_class_name, ":shader/resources/shader/sphere.vert", ":shader/resources/shader/simpleObj.frag");

	// 顶点与三角形数据
	QVector<GLfloat> vertices = QVector<GLfloat>();
	std::vector<unsigned int> indices = std::vector<unsigned int>();

	float radius = 1.0f;
	float dTheta = 2 * M_PI / steps;
	float dDelta = M_PI / 2 / steps;

	for (float delt = 0; delt < M_PI; delt += dDelta)
	{
		for (float theta = 0; theta < 2 * M_PI; theta += dTheta)
		{
			unsigned int idx_base = vertices.size() / 6;
			// 顶点
			vertices.push_back(radius * sin(delt) * cos(theta));
			vertices.push_back(radius * cos(delt));
			vertices.push_back(radius * sin(delt) * sin(theta));
			vertices << sin(delt) * cos(theta) << cos(delt) << sin(delt) * sin(theta);

			vertices.push_back(radius * sin(delt + dDelta) * cos(theta));
			vertices.push_back(radius * cos(delt + dDelta));
			vertices.push_back(radius * sin(delt + dDelta) * sin(theta));
			vertices << sin(delt + dDelta) * cos(theta) << cos(delt + dDelta) << sin(delt + dDelta) * sin(theta);

			vertices.push_back(radius * sin(delt) * cos(theta + dTheta));
			vertices.push_back(radius * cos(delt));
			vertices.push_back(radius * sin(delt) * sin(theta + dTheta));
			vertices << sin(delt) * cos(theta + dTheta) << cos(delt) << sin(delt) * sin(theta + dTheta);

			vertices.push_back(radius * sin(delt + dDelta) * cos(theta + dTheta));
			vertices.push_back(radius * cos(delt + dDelta));
			vertices.push_back(radius * sin(delt + dDelta) * sin(theta + dTheta));
			vertices << sin(delt + dDelta) * cos(theta + dTheta) << cos(delt + dDelta) << sin(delt + dDelta) * sin(theta + dTheta);

			// 三角形
			std::vector<unsigned int> triangle_1{ idx_base, idx_base + 1, idx_base + 3 };
			std::vector<unsigned int> triangle_2{ idx_base, idx_base + 2, idx_base + 3 };

			indices.insert(indices.end(), triangle_1.begin(), triangle_1.end());
			indices.insert(indices.end(), triangle_2.begin(), triangle_2.end());
		}
	}

	m_indices_size = indices.size();

	// VBO, EBO
	QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(&(*vertices.begin()), vertices.size() * sizeof(GLfloat));

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(&(*indices.begin()), indices.size() * sizeof(unsigned int));

	// 坐标
	int attr = -1;
	attr = m_shader.attributeLocation(san::ATTR_POSITION);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
	m_shader.enableAttributeArray(attr);

	// 法向量
	attr = m_shader.attributeLocation(san::ATTR_NORMAL);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, sizeof(GL_FLOAT) * 3, 3, sizeof(GLfloat) * 6);
	m_shader.enableAttributeArray(attr);

	m_vbo.release();
}

void GLSphere::setRadius(float r)
{
	m_radius = r;
}

void GLSphere::setCenterPos(const QVector3D& pos)
{
	m_center_pos = pos;
}

void GLSphere::paint()
{
	paint(m_center_pos, m_radius);
}

void GLSphere::paint(const QVector3D& pos, float radius /* = 1.0f */)
{
	m_shader.bind();
	_setLightCond_();

	// 设置位置和半径
	m_local_to_global(0, 3) = pos.x();
	m_local_to_global(1, 3) = pos.y();
	m_local_to_global(2, 3) = pos.z();
	m_shader.setUniformValue(san::ATTR_RADIUS, radius);

	m_shader.setUniformValue(san::ATTR_PROJ_MAT, m_camera->getProjectionMatrix());					// 投影矩阵
	m_shader.setUniformValue(san::ATTR_VIEW_MAT, m_camera->getViewMatrix());	// 摄像机矩阵
	m_shader.setUniformValue(san::ATTR_LOC_TO_GLOB_MAT, m_local_to_global);		// 局部到全局坐标转换
	{
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);
		glDrawElements(GL_TRIANGLES, m_indices_size, GL_UNSIGNED_INT, 0);
	}
	m_shader.release();
}

bool GLSphere::intersectPoint(const GLRay& ray, GLHitPoint& hitpoint)
{
	float dist1 = (m_center_pos - ray.m_start_pos).length();							// 光线起点与球心距离平方
	float dist2 = QVector3D::dotProduct(m_center_pos - ray.m_start_pos, ray.m_dir);		// 光线起点到光线离球心最近点的距离
	if (dist1 >= m_radius && dist2 <= 0)
	{
		// 光线起点在球外且球在光线背面
		return false;
	}
	float hlen = m_radius * m_radius - dist1 * dist1 + dist2 * dist2;		// 半弦长平方
	if (hlen < 0)
		return false;

	hlen = sqrtf(hlen);

	// 求出光线起点到交点位置的两个解，并留下最小的正数解
	float t1 = dist2 - hlen;
	float t2 = dist2 + hlen;
	float final_dist = t1 <= 0 ? t2 : t1;

	if (hitpoint.m_dist < 0 || hitpoint.m_dist > final_dist)
	{
		// 更新交点
		hitpoint.m_dist = final_dist;
		hitpoint.m_pos = ray.m_start_pos + final_dist * ray.m_dir;
		hitpoint.m_normal = (hitpoint.m_pos - m_center_pos).normalized();
		hitpoint.m_material = m_material;
		hitpoint.m_inner_hit = dist1 <= m_radius;
		hitpoint.m_color = QVector3D(m_color);
	}

	return true;
}

bool GLSphere::collisionDetect(const QVector3D& pos)
{
	return (pos - m_center_pos).length() <= m_radius;
}

std::string GLSphere::getObjTypeName()
{
	return "Sphere";
}

/************************************************************************/
/* GLCylinder                                                           */
/************************************************************************/

GLCylinder::GLCylinder(int steps /* = 100 */)
{
	_compileAndLinkShader_(m_class_name, ":shader/resources/shader/cylinder.vert", ":shader/resources/shader/simpleObj.frag");

	// 顶点与三角形数据
	QVector<GLfloat> vertices = QVector<GLfloat>();
	QVector<unsigned int> indices = QVector<unsigned int>();

	float radius = 1.0f;
	float length = 1.0f;

	float dTheta = 2 * M_PI / steps;

	// 设置上表面中心和下表面中心
	int bot_center_idx = 0;
	int top_center_idx = 1;
	vertices << 0 << 0 << 0 << 0 << 0 << -1;
	vertices << 0 << 0 << length << 0 << 0 << 1;

	for (float theta = 0; theta < 2 * M_PI; theta += dTheta)
	{
		unsigned int idx_base = vertices.size() / 6;
		// 上下表面三角形顶点
		vertices << radius * cos(theta) << radius * sin(theta) << 0;
		vertices << 0 << 0 << -1;
		vertices << radius * cos(theta + dTheta) << radius * sin(theta + dTheta) << 0;
		vertices << 0 << 0 << -1;

		vertices << radius * cos(theta) << radius * sin(theta) << length;
		vertices << 0 << 0 << 1;
		vertices << radius * cos(theta + dTheta) << radius * sin(theta + dTheta) << length;
		vertices << 0 << 0 << 1;

		// 侧面三角形顶点
		vertices << radius * cos(theta) << radius * sin(theta) << 0;
		vertices << cos(theta) << sin(theta) << 0;
		vertices << radius * cos(theta + dTheta) << radius * sin(theta + dTheta) << 0;
		vertices << cos(theta + dTheta) << sin(theta + dTheta) << 0;

		vertices << radius * cos(theta) << radius * sin(theta) << length;
		vertices << cos(theta) << sin(theta) << 0;
		vertices << radius * cos(theta + dTheta) << radius * sin(theta + dTheta) << length;
		vertices << cos(theta + dTheta) << sin(theta + dTheta) << 0;

		// 三角形下标
		indices << 0 << idx_base << idx_base + 1;
		indices << 1 << idx_base + 2 << idx_base + 3;
		indices << idx_base + 4 << idx_base + 5 << idx_base + 6;
		indices << idx_base + 5 << idx_base + 6 << idx_base + 7;
	}

	m_indices_size = indices.size();
	// VBO, EBO
	QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(&(*vertices.begin()), vertices.size() * sizeof(GLfloat));

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(&(*indices.begin()), indices.size() * sizeof(unsigned int));

	// 坐标
	int attr = -1;
	attr = m_shader.attributeLocation(san::ATTR_POSITION);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
	m_shader.enableAttributeArray(attr);

	// 法向量
	attr = m_shader.attributeLocation(san::ATTR_NORMAL);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, sizeof(GL_FLOAT) * 3, 3, sizeof(GLfloat) * 6);
	m_shader.enableAttributeArray(attr);

	m_vbo.release();
}

void GLCylinder::paint()
{
	paint(1.0f, 1.0f, QVector3D(0, 0, 1), QVector3D(0, 0, 0));
}

void GLCylinder::paint(float radius, float length, QVector3D direction, QVector3D start_pos)
{
	m_shader.bind();
	_setLightCond_();

	// 设置起始位置和方向
	m_local_to_global(0, 3) = start_pos.x();
	m_local_to_global(1, 3) = start_pos.y();
	m_local_to_global(2, 3) = start_pos.z();

	Eigen::Matrix3f rot_mat = getRotMat(Eigen::Vector3f(0, 0, 1.0f),
		Eigen::Vector3f(direction.x(), direction.y(), direction.z()));
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
			m_local_to_global(r, c) = rot_mat(r, c);
	}

	// 设置半径和长度
	m_shader.setUniformValue(san::ATTR_RADIUS, radius);
	m_shader.setUniformValue(san::ATTR_LENGTH, length);

	m_shader.setUniformValue(san::ATTR_PROJ_MAT, m_camera->getProjectionMatrix());					// 投影矩阵
	m_shader.setUniformValue(san::ATTR_VIEW_MAT, m_camera->getViewMatrix());	// 摄像机矩阵
	m_shader.setUniformValue(san::ATTR_LOC_TO_GLOB_MAT, m_local_to_global);		// 局部到全局坐标转换
	{
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);
		glDrawElements(GL_TRIANGLES, m_indices_size, GL_UNSIGNED_INT, 0);
	}
	m_shader.release();
}

/************************************************************************/
/* GLCone                                                               */
/************************************************************************/

GLCone::GLCone(int steps /* = 100 */)
{
	_compileAndLinkShader_(m_class_name, ":shader/resources/shader/cylinder.vert", ":shader/resources/shader/simpleObj.frag");

	// 顶点与三角形数据
	QVector<GLfloat> vertices = QVector<GLfloat>();
	QVector<unsigned int> indices = QVector<unsigned int>();

	float radius = 1.0f;
	float length = 1.0f;

	float dTheta = 2 * M_PI / steps;

	// 设置上表面中心和下表面中心
	int bot_center_idx = 0;
	int top_center_idx = 1;
	vertices << 0 << 0 << 0 << 0 << 0 << -1;
	vertices << 0 << 0 << length << 0 << 0 << 1;

	for (float theta = 0; theta < 2 * M_PI; theta += dTheta)
	{
		unsigned int idx_base = vertices.size() / 6;
		// 下表面三角形顶点
		vertices << radius * cos(theta) << radius * sin(theta) << 0;
		vertices << 0 << 0 << -1;
		vertices << radius * cos(theta + dTheta) << radius * sin(theta + dTheta) << 0;
		vertices << 0 << 0 << -1;

		// 侧面三角形顶点
		vertices << radius * cos(theta) << radius * sin(theta) << 0;
		vertices << cos(theta) << sin(theta) << 0;
		vertices << radius * cos(theta + dTheta) << radius * sin(theta + dTheta) << 0;
		vertices << cos(theta + dTheta) << sin(theta + dTheta) << 0;

		// 三角形下标
		indices << 0 << idx_base << idx_base + 1;
		indices << 1 << idx_base + 2 << idx_base + 3;
	}

	m_indices_size = indices.size();
	// VBO, EBO
	QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(&(*vertices.begin()), vertices.size() * sizeof(GLfloat));

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(&(*indices.begin()), indices.size() * sizeof(unsigned int));

	// 坐标
	int attr = -1;
	attr = m_shader.attributeLocation(san::ATTR_POSITION);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
	m_shader.enableAttributeArray(attr);

	// 法向量
	attr = m_shader.attributeLocation(san::ATTR_NORMAL);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, sizeof(GL_FLOAT) * 3, 3, sizeof(GLfloat) * 6);
	m_shader.enableAttributeArray(attr);

	m_vbo.release();
}

void GLCone::paint()
{
	paint(1.0f, 1.0f, QVector3D(0, 0, 1), QVector3D(0, 0, 0));
}

void GLCone::paint(float radius, float length, QVector3D direction, QVector3D start_pos)
{
	m_shader.bind();
	_setLightCond_();

	// 设置起始位置和方向
	m_local_to_global(0, 3) = start_pos.x();
	m_local_to_global(1, 3) = start_pos.y();
	m_local_to_global(2, 3) = start_pos.z();

	Eigen::Matrix3f rot_mat = getRotMat(Eigen::Vector3f(0, 0, 1.0f),
		Eigen::Vector3f(direction.x(), direction.y(), direction.z()));
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
			m_local_to_global(r, c) = rot_mat(r, c);
	}

	// 设置半径和长度
	m_shader.setUniformValue(san::ATTR_RADIUS, radius);
	m_shader.setUniformValue(san::ATTR_LENGTH, length);

	m_shader.setUniformValue(san::ATTR_PROJ_MAT, m_camera->getProjectionMatrix());					// 投影矩阵
	m_shader.setUniformValue(san::ATTR_VIEW_MAT, m_camera->getViewMatrix());	// 摄像机矩阵
	m_shader.setUniformValue(san::ATTR_LOC_TO_GLOB_MAT, m_local_to_global);		// 局部到全局坐标转换
	{
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);
		glDrawElements(GL_TRIANGLES, m_indices_size, GL_UNSIGNED_INT, 0);
	}
	m_shader.release();
}

/************************************************************************/
/* GLMirror                                                             */
/************************************************************************/
GLMirror::GLMirror()
{
	_compileAndLinkShader_(m_class_name, ":shader/resources/shader/plane.vert", ":shader/resources/shader/plane.frag");

	// VAO，VBO数据部分
	GLfloat vertices[] = {
		// pos				  texCoord
		-0.5f, -0.5f,  0.0f,  0.0f,  0.0f,	// 0
		 0.5f, -0.5f,  0.0f,  1.0f,  0.0f,	// 1
		 0.5f,  0.5f,  0.0f,  1.0f,  1.0f, 	// 2
		-0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 	// 3
	};
	unsigned int indices[] = {
		 0,  1,  2,	  0,  2,  3,
	};

	QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(vertices, sizeof(vertices));

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(indices, sizeof(indices));

	// 坐标
	int attr = -1;
	attr = m_shader.attributeLocation(san::ATTR_POSITION);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 5);
	m_shader.enableAttributeArray(attr);

	// 纹理坐标
	attr = m_shader.attributeLocation(san::ATTR_TEXTURE);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 3, 2, sizeof(GLfloat) * 5);
	m_shader.enableAttributeArray(attr);
	m_vbo.release();
}

void GLMirror::setSize(float width, float height)
{
	m_width = width;
	m_height = height;
}

void GLMirror::paint()
{
	
}

void GLMirror::paint(const QOpenGLFramebufferObject* fbo)
{
	m_shader.bind();

	QMatrix4x4 l2g = m_local_to_global;
	l2g.scale(m_width, m_height, 1);

	m_shader.setUniformValue(san::ATTR_PROJ_MAT, m_camera->getProjectionMatrix());					// 投影矩阵
	m_shader.setUniformValue(san::ATTR_VIEW_MAT, m_camera->getViewMatrix());	// 摄像机矩阵
	m_shader.setUniformValue(san::ATTR_LOC_TO_GLOB_MAT, l2g);					// 局部到全局坐标转换
	{
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);
		glBindTexture(GL_TEXTURE_2D, fbo->texture());
		glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
	}

	m_shader.release();
}

float GLMirror::getWidth()
{
	return m_width;
}

float GLMirror::getHeight()
{
	return m_height;
}

bool GLMirror::intersectPoint(const GLRay& ray, GLHitPoint& hitpoint)
{
	// 将光线转换到模型局部坐标系下
	QMatrix4x4 global_to_local = m_local_to_global.inverted();
	QVector4D start_pos_m = global_to_local * QVector4D(ray.m_start_pos, 1.0);
	QVector4D light_dir_m = global_to_local * QVector4D(ray.m_dir, 0);

	if (light_dir_m.z() == 0)
	{
		// 光线方向与面平行
		return false;
	}

	float dist = -start_pos_m.z() / light_dir_m.z();
	if (dist <= 0)
		return false;

	// 判断光线与z=0面的交点是否在镜面范围内
	QVector4D cross_point = start_pos_m + dist * light_dir_m;
	float X = fabs(cross_point.x());
	float Y = fabs(cross_point.y());
	if (X > 0.5 * m_width || Y > 0.5 * m_height)
		return false;

	dist /= m_local_to_global(3, 3);
	QVector4D normal_l(0, 0, 1, 0);
	if (start_pos_m.z() < 0)
		normal_l.setZ(-normal_l.z());

	if (hitpoint.m_dist < 0 || hitpoint.m_dist > dist)
	{
		// 更新交点
		hitpoint.m_dist = dist;
		hitpoint.m_pos = ray.m_start_pos + dist * ray.m_dir;
		hitpoint.m_normal = QVector3D(m_local_to_global * normal_l);
		hitpoint.m_material = m_material;
		hitpoint.m_inner_hit = false;
		hitpoint.m_color = QVector3D(1, 1, 1);		// 镜面的颜色可以是任意的
	}
}

/************************************************************************/
/* GLSkyBox                                                             */
/************************************************************************/

GLSkyBox::GLSkyBox(const std::vector<QString>& img_filenames)
{
	_compileAndLinkShader_(m_class_name, ":shader/resources/shader/skybox.vert", ":shader/resources/shader/skybox.frag");

	// VAO，VBO数据部分
	GLfloat vertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(vertices, sizeof(vertices));

	// 坐标
	int attr = -1;
	attr = m_shader.attributeLocation(san::ATTR_POSITION);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 3);
	m_shader.enableAttributeArray(attr);

	// 纹理
	assert(img_filenames.size() == 6);
	for (int i = 0; i < 6; i++)
	{
		m_imgs[i] = QImage(img_filenames[i]).convertToFormat(QImage::Format_RGB888);
	}

	m_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);
	m_texture->create();
	m_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	m_texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    m_texture->setMipLevels(0);
    m_texture->setLayers(0);
    m_texture->setSize(m_imgs[0].width(), m_imgs[0].height(), m_imgs[0].depth());
    m_texture->setFormat(QOpenGLTexture::RGB8_UNorm);
    m_texture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::UInt8);
    for (int i = 0; i < 6; ++i) {
        m_texture->setData(0, 0, static_cast<QOpenGLTexture::CubeMapFace>(QOpenGLTexture::CubeMapPositiveX + i), 
			QOpenGLTexture::RGB, QOpenGLTexture::UInt8, m_imgs[i].bits());
    }
    m_texture->generateMipMaps();
}

void GLSkyBox::paint()
{
	glDepthFunc(GL_LEQUAL);
	m_shader.bind();

	QMatrix4x4 cam_mat = m_camera->getViewMatrix();
	cam_mat(0, 3) = cam_mat(1, 3) = cam_mat(2, 3) = 0;

	m_shader.setUniformValue(san::ATTR_PROJ_MAT, m_camera->getProjectionMatrix());					// 投影矩阵
	m_shader.setUniformValue(san::ATTR_VIEW_MAT, cam_mat);	// 摄像机矩阵
	m_shader.setUniformValue(san::ATTR_LOC_TO_GLOB_MAT, m_local_to_global);		// 局部到全局坐标转换

	{
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);
		glActiveTexture(GL_TEXTURE0);
		m_texture->bind();
		glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
		m_texture->release();
	}

	m_shader.release();
	glDepthFunc(GL_LESS);
}

QVector3D GLSkyBox::getRayColor(const GLRay& ray)
{
	QVector3D dir = ray.m_dir.normalized();
	QVector3D absDir(fabs(dir.x()), fabs(dir.y()), fabs(dir.z()));
	float maxEle = -1;
	int maxIdx = -1;
	for (int i = 0; i < 3; i++)
	{
		if (absDir[i] > maxEle)
		{
			maxEle = absDir[i];
			maxIdx = i;
		}
	}

	int texPlaneIdx = 2 * maxIdx + (dir[maxIdx] >= 0 ? 0 : 1);		// 光线与天空盒交点所在的面
	auto& img = m_imgs[texPlaneIdx];

	// 计算像素坐标
	double fx, fy;
	if (maxIdx == 0)
	{
		// 视角沿x轴方向
		fx = -dir[2] / dir[0];
		fy = -dir[1] / absDir[0];
	}
	else if (maxIdx == 1)
	{
		// 视角沿y轴方向
		fx = dir[0] / absDir[1];
		fy = dir[2] / dir[1];
	}
	else if (maxIdx == 2)
	{
		// 视角沿z轴方向
		fx = dir[0] / dir[2];
		fy = -dir[1] / absDir[2];
	}
	fx = (fx + 1) / 2.0f;
	fy = (fy + 1) / 2.0f;

	int px = fx * img.width();
	int py = fy * img.height();

	px = px >= img.width() ? img.width() - 1 : px;
	py = py >= img.height() ? img.height() - 1 : py;
	
	QColor color = QColor(img.pixel(px, py));
	return QVector3D(color.redF(), color.greenF(), color.blueF());
}