#pragma once
#include<json/json.h>

#include<memory>
#include<string>
#include<set>

#include"Camera.h"
#include"GLObject.h"
#include"GLFilter.h"
#include"GLModel.h"
#include"GLAnimateModel.h"

class GLScene : QOpenGLFunctions
{
private:
	const std::string m_class_name = "(GLScene)";
	std::shared_ptr<Camera> m_global_cam;
	std::shared_ptr<GLLight> m_light;

public:
	// 天空盒
	std::unique_ptr<GLSkyBox> m_skybox;

	// 几何体
	std::vector<std::unique_ptr<GLObject>> m_simple_objs;
	std::vector<std::unique_ptr<GLAnimateModel>> m_ani_models;
	std::vector<std::unique_ptr<GLModel>> m_static_models;

	// 镜面
	std::unique_ptr<GLMirror> m_mirror;
	std::unique_ptr<QOpenGLFramebufferObject> m_mirror_fbo;
	std::shared_ptr<Camera> m_mirror_cam;

public:
	GLScene();

	/**
	 * @brief 将摄像机视角切换到镜子处，渲染场景中其它物体并储存到帧缓冲中，作为镜子内显示的画面
	*/
	void prepareMirror();
	void setCamera(std::shared_ptr<Camera> p_cam);
	void setLight(std::shared_ptr<GLLight> p_light);

	/**
	 * @brief 绘制场景中所有几何体
	*/
	void paintAll();

	/**
	 * @brief 绘制场景中除了镜子之外的所有几何体
	*/
	void paintObjects();

	/**
	 * @brief 从json格式的文件中加载场景配置
	 * @param filename json配置
	*/
	void loadSceneFromFile(const std::string& filename);

	/**
	 * @brief 获取碰撞信息
	 * @return 碰撞描述
	*/
	std::string getCollision();
	void destory();

	/**
	 * @brief 利用cpu进行光线追踪，将结果储存为纹理利用帧缓冲渲染
	 * @param thread_num 线程数量
	 * @return 储存在纹理中的光线追踪结果
	*/
	std::shared_ptr<QOpenGLTexture> rayTracingRender(int thread_num);

private:
	void _setCamera_(std::shared_ptr<Camera> p_cam);

	/**
	 * @brief 追踪单根光线直到碰到粗糙物体或者天空盒或者折射/反射次数达到上限
	 * @param ray 光线
	 * @return 光线对应像素点处的最终颜色值
	*/
	QVector3D _traceRay_(GLRay& ray);

	/**
	 * @brief 多线程光线追踪时每个线程调用的函数
	 * @param thread_num 总线程数
	 * @param thread_idx 当前线程编号
	 * @param img 储存结果的图像
	*/
	void _multiThreadRT_(const int thread_num, const int thread_idx, QImage& img);

	/**
	@brief 获取光线与场景中物体的第一个交点
	@param ray : 光线
	*/
	GLHitPoint _getFirstHit_(GLRay& ray);
};