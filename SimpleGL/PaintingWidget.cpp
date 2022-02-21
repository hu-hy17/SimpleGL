#include"PaintingWidget.h"
#include"StopWatch.h"

#include<vector>
#include<string>
#include<iostream>
#include<chrono>

#include<QFile>
#include<QVector>

#include<Eigen/Dense>

#define _USE_MATH_DEFINES
#include <math.h>

using std::cout;
using std::endl;

PaintingWidget::PaintingWidget(QWidget* parent) : QOpenGLWidget(parent)
{
	// 光照
	float ambient_strength = 0.4;
	float diffuse_strength = 0.8;
	float specular_strength = 0.5;
	QVector3D light_pos(0.0f, 150.0f, -150.0f);
	m_light = std::make_shared<GLLight>(light_pos, QVector3D(1.0f, 1.0f, 1.0f));

	// 相机
	m_camera = std::make_shared<Camera>(QVector3D(0.0f, 0.0f, -300.0f), QVector3D(0, 1, 0), M_PI / 2, 0);
	m_camera->setScreenSize(width(), height());
	mBtnLeftPressed = false;

	// 抗锯齿
	QSurfaceFormat surfaceFormat;
	surfaceFormat.setSamples(4);		//多重采样
	setFormat(surfaceFormat);
}

PaintingWidget::~PaintingWidget()
{
	makeCurrent();
	if(m_scene)
		m_scene->destory();
	if(m_filter)
		m_filter->destroy();
	doneCurrent();
}

void PaintingWidget::initializeGL()
{
	this->initializeOpenGLFunctions();

	// 深度检测
	glEnable(GL_DEPTH_TEST);

	// 帧缓冲
	m_filter = std::make_unique<GLFilter>();
	m_fbo = std::make_unique<QOpenGLFramebufferObject>(width(), height(),
		QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D, GL_RGB);

	// 场景设置
	m_scene = std::make_unique<GLScene>();
	m_scene->loadSceneFromFile(m_scene_filename);
	m_scene->setCamera(m_camera);
	m_scene->setLight(m_light);

	if (m_use_rt)
		m_ofs_fps.open("../fps_rt_" + std::to_string(m_rt_tnum) + ".txt");
	else
		m_ofs_fps.open("../fps_opengl.txt");

	m_last_update_time = clock();
}

void PaintingWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
	m_fbo = std::make_unique<QOpenGLFramebufferObject>(width(), height(),
		QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D, GL_RGB);
	m_camera->setScreenSize(w, h);
}

void PaintingWidget::paintGL() {
	// 更新相机
	m_camera->processInput(0.1f);

	// 碰撞检测
	std::string colli_list = m_scene->getCollision();
	emit(onCollision(QString::fromStdString(colli_list)));
	
	if (!m_use_rt)
	{
		m_scene->prepareMirror();
		// 在帧缓冲中绘制
		m_fbo->bind();
		{
			glEnable(GL_DEPTH_TEST);
			// 背景
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_scene->paintAll();
		}
		m_fbo->release();

		// 在默认缓冲中绘制
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, m_fbo->texture());
		m_filter->paint();
	}
	else
	{
		// 光线追踪
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto pTex = m_scene->rayTracingRender(m_rt_tnum);
		pTex->bind();
		m_filter->paint();
		pTex->release();
	}

	update();

	clock_t now = clock();
	float span = 1.0f * (now - m_last_update_time) / CLOCKS_PER_SEC;
	m_last_update_time = now;

	m_ofs_fps << int(1.0f / span) << " ";

	emit showFPS(int(1.0f / span));
}

/************************************************************************/
/* 数据加载和更新
/************************************************************************/
void PaintingWidget::setSceneFile(const std::string& filename)
{
	m_scene_filename = filename;
}

void PaintingWidget::setUseRT(bool use, int thread_num)
{
	m_use_rt = use;
	m_rt_tnum = thread_num;
}

/************************************************************************/
/* 按钮事件响应
/************************************************************************/
void PaintingWidget::keyPressEvent(QKeyEvent* event)
{
	int key = event->key();
	if (key >= 0 && key < 1024)
		m_camera->keys[key] = true;
	switch (key)
	{
	}
}

void PaintingWidget::keyReleaseEvent(QKeyEvent* event)
{
	int key = event->key();
	if (key >= 0 && key < 1024)
		m_camera->keys[key] = false;
}

void PaintingWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		mBtnLeftPressed = true;
		mLastPos = event->pos();
	}
}

void PaintingWidget::mouseReleaseEvent(QMouseEvent* event)
{
	Q_UNUSED(event);

	mBtnLeftPressed = false;
}

void PaintingWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (mBtnLeftPressed) {
		int xpos = event->pos().x();
		int ypos = event->pos().y();

		int xoffset = xpos - mLastPos.x();
		int yoffset = mLastPos.y() - ypos;
		mLastPos = event->pos();
		m_camera->processMouseMovement(xoffset, yoffset);
	}
}

void PaintingWidget::wheelEvent(QWheelEvent* event)
{
	QPoint offset = event->angleDelta();
	m_camera->processMouseScroll(offset.y() / 20.0f);
}

/************************************************************************/
/* 信号槽
/************************************************************************/

void PaintingWidget::setFilterInverse()
{
	m_filter->setFilter(GLFilter::FILTER::FILTER_INVERSE);
}

void PaintingWidget::setFilterNone()
{
	m_filter->setFilter(GLFilter::FILTER::NONE);
}