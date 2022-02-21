#pragma once

#include<QOpenGLShaderProgram>
#include<QVector3D>

#include"Defs.h"

class GLLight
{
public:
	QVector3D m_light_pos;		// ��Դλ��
	QVector3D m_light_color;	// �����ɫ
	QVector3D m_view_pos;		// �۲���λ��

public:
	GLLight(const QVector3D& light_pos, const QVector3D& light_color = QVector3D(1.0f, 1.0f, 1.0f));

	/**
	 * @brief ����ɫ����������ӹ��ղ���
	 * @param prog QT��ʽ����ɫ������
	*/
	void setLightCond(QOpenGLShaderProgram& prog);
};