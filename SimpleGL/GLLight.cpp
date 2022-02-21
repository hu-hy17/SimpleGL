#include"GLLight.h"

GLLight::GLLight(const QVector3D& light_pos, const QVector3D& light_color)
{
	m_light_pos = light_pos;
	m_light_color = light_color;
}

void GLLight::setLightCond(QOpenGLShaderProgram& prog)
{
	// 光源位置
	prog.setUniformValue(san::ATTR_LIGHT_POS, m_light_pos);

	// 光照颜色
	prog.setUniformValue(san::ATTR_LIGHT_COLOR, m_light_color);

	// 观察者位置
	prog.setUniformValue(san::ATTR_VIEW_POS, m_view_pos);
}