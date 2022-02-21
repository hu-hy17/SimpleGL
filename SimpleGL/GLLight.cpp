#include"GLLight.h"

GLLight::GLLight(const QVector3D& light_pos, const QVector3D& light_color)
{
	m_light_pos = light_pos;
	m_light_color = light_color;
}

void GLLight::setLightCond(QOpenGLShaderProgram& prog)
{
	// ��Դλ��
	prog.setUniformValue(san::ATTR_LIGHT_POS, m_light_pos);

	// ������ɫ
	prog.setUniformValue(san::ATTR_LIGHT_COLOR, m_light_color);

	// �۲���λ��
	prog.setUniformValue(san::ATTR_VIEW_POS, m_view_pos);
}