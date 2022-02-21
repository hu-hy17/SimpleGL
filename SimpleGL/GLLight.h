#pragma once

#include<QOpenGLShaderProgram>
#include<QVector3D>

#include"Defs.h"

class GLLight
{
public:
	QVector3D m_light_pos;		// 光源位置
	QVector3D m_light_color;	// 光的颜色
	QVector3D m_view_pos;		// 观察者位置

public:
	GLLight(const QVector3D& light_pos, const QVector3D& light_color = QVector3D(1.0f, 1.0f, 1.0f));

	/**
	 * @brief 向着色器程序中添加光照参数
	 * @param prog QT格式的着色器程序
	*/
	void setLightCond(QOpenGLShaderProgram& prog);
};