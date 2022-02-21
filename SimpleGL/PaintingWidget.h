#pragma once

#include<QOpenGLWidget>
#include<QOpenGLShader>
#include<QOpenGLShaderProgram>
#include<QDebug>
#include<QOpenGLFunctions>
#include<QOpenGLVertexArrayObject>
#include<QOpenGLFramebufferObject>
#include<QOpenGLBuffer>

#include<fstream>

#include"GLScene.h"

class PaintingWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
private:
	const std::string m_class_name = "(PaintingWidget)";

	// �����ļ�
	std::string m_scene_filename;

	// ����
	std::unique_ptr<GLScene> m_scene;

	// ���
	std::shared_ptr<Camera> m_camera;
	bool mBtnLeftPressed = false;
	QPoint mLastPos;

	// ����
	std::shared_ptr<GLLight> m_light;

	// ֡����
	std::unique_ptr<GLFilter> m_filter;
	std::unique_ptr<QOpenGLFramebufferObject> m_fbo;

	// ��ʱ
	clock_t m_last_update_time;

	bool m_use_rt = false;
	int m_rt_tnum = 4;

	std::ofstream m_ofs_fps;

public:
	PaintingWidget(QWidget* parent = nullptr);
	~PaintingWidget() Q_DECL_OVERRIDE;
	void setSceneFile(const std::string& filename);
	void setUseRT(bool use, int thread_num);

protected:
	virtual void initializeGL() Q_DECL_OVERRIDE;
	virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	virtual void paintGL() Q_DECL_OVERRIDE;

	void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

public slots:
	void setFilterInverse();
	void setFilterNone();

signals:
	void onCollision(const QString& target_name);
	void showFPS(int fps);
};