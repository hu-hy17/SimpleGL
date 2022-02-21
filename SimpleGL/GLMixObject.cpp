#include"GLMixObject.h"

#include<iostream>

using std::cout;
using std::cerr;
using std::endl;

/************************************************************************/
/* GLMixObject                                                          */
/************************************************************************/

GLMixObject::GLMixObject()
{
	m_globjects = std::vector<std::unique_ptr<GLObject>>();
}

void GLMixObject::destroy()
{
	for (auto& obj : m_globjects)
		obj->destroy();
}

void GLMixObject::setColor(const QVector4D& color)
{
	for (auto& obj : m_globjects)
		obj->setColor(color);
}

void GLMixObject::setCamera(std::shared_ptr<Camera> camera)
{
	for (auto& obj : m_globjects)
		obj->setCamera(camera);
}

void GLMixObject::setLight(std::shared_ptr<GLLight> light)
{
	for (auto& obj : m_globjects)
		obj->setLight(light);
}

void GLMixObject::setLocToGlob(const QMatrix4x4& mat)
{
	for (auto& obj : m_globjects)
		obj->setLocToGlob(mat);
}

void GLMixObject::setMaterial(std::shared_ptr<GLMaterial> material)
{
	for (auto& obj : m_globjects)
		obj->setMaterial(material);
}

/************************************************************************/
/* GLArrow                                                              */
/************************************************************************/

GLArrow::GLArrow(int steps /* = 100 */)
{
	m_globjects.push_back(std::make_unique<GLCylinder>(steps));
	m_globjects.push_back(std::make_unique<GLCone>(steps));
}

void GLArrow::paint()
{
	paint(0.1f, 1.0f, QVector3D(0, 0, 1), QVector3D(0, 0, 0));
}

void GLArrow::paint(float radius, float length, QVector3D direction, QVector3D start_pos)
{
	QVector3D cone_start_pos = start_pos + m_len_ratio * length * direction.normalized();
	GLCylinder* p_cylinder = dynamic_cast<GLCylinder*>(m_globjects[0].get());
	GLCone* p_cone = dynamic_cast<GLCone*>(m_globjects[1].get());
	if (p_cone && p_cylinder)
	{
		float cone_len = std::max((1 - m_len_ratio) * length, 10.0f);
		p_cone->paint(radius * m_radius_ratio, cone_len, direction, cone_start_pos);
		p_cylinder->paint(radius, m_len_ratio * length, direction, start_pos);
	}
	else
	{
		cout << m_class_name << "Error: cannot cast smart ptr to common ptr!" << endl;
	}
}

/************************************************************************/
/* GLCubeFrameWork                                                      */
/************************************************************************/
GLCubeFrameWork::GLCubeFrameWork()
{
	m_globjects.push_back(std::make_unique<GLCylinder>());
	m_globjects.push_back(std::make_unique<GLCube>());
	m_globjects.push_back(std::make_unique<GLSphere>());
}

void GLCubeFrameWork::setColor(QVector4D& center_color, QVector4D& edge_color, QVector4D& plane_color)
{
	m_globjects[0]->setColor(edge_color);
	m_globjects[1]->setColor(plane_color);
	m_globjects[2]->setColor(center_color);
}

void GLCubeFrameWork::paint()
{
	paint(1.0f, QMatrix4x4());
}

void GLCubeFrameWork::paint(float edge_len, const QMatrix4x4& motion)
{
	GLCylinder* p_cylinder = dynamic_cast<GLCylinder*>(m_globjects[0].get());
	GLCube* p_cube = dynamic_cast<GLCube*>(m_globjects[1].get());
	GLSphere* p_sphere = dynamic_cast<GLSphere*>(m_globjects[2].get());
	if (p_cylinder && p_cube && p_sphere)
	{
		QVector3D center_pos(motion(0, 3), motion(1, 3), motion(2, 3));

		p_sphere->paint(center_pos, edge_len * m_len_ratio / 2);

		p_cube->setLocToGlob(motion);
		p_cube->paint(edge_len, edge_len, edge_len);

		float hl = edge_len / 2;
		for (int i = 0; i < 12; i++)
		{
			QVector4D cano_start_pos(QVector3D(hl * m_cano_start_pos[i]), 1.0);
			QVector3D start_pos(motion * cano_start_pos);
			QVector3D dir(motion * m_cano_dir[i]);
			p_cylinder->paint(m_radius, edge_len, dir, start_pos);
		}
	}
	else
	{
		cout << m_class_name << "Error: cannot cast smart ptr to common ptr!" << endl;
	}

}