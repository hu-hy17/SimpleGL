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
	// ��պ�
	std::unique_ptr<GLSkyBox> m_skybox;

	// ������
	std::vector<std::unique_ptr<GLObject>> m_simple_objs;
	std::vector<std::unique_ptr<GLAnimateModel>> m_ani_models;
	std::vector<std::unique_ptr<GLModel>> m_static_models;

	// ����
	std::unique_ptr<GLMirror> m_mirror;
	std::unique_ptr<QOpenGLFramebufferObject> m_mirror_fbo;
	std::shared_ptr<Camera> m_mirror_cam;

public:
	GLScene();

	/**
	 * @brief ��������ӽ��л������Ӵ�����Ⱦ�������������岢���浽֡�����У���Ϊ��������ʾ�Ļ���
	*/
	void prepareMirror();
	void setCamera(std::shared_ptr<Camera> p_cam);
	void setLight(std::shared_ptr<GLLight> p_light);

	/**
	 * @brief ���Ƴ��������м�����
	*/
	void paintAll();

	/**
	 * @brief ���Ƴ����г��˾���֮������м�����
	*/
	void paintObjects();

	/**
	 * @brief ��json��ʽ���ļ��м��س�������
	 * @param filename json����
	*/
	void loadSceneFromFile(const std::string& filename);

	/**
	 * @brief ��ȡ��ײ��Ϣ
	 * @return ��ײ����
	*/
	std::string getCollision();
	void destory();

	/**
	 * @brief ����cpu���й���׷�٣����������Ϊ��������֡������Ⱦ
	 * @param thread_num �߳�����
	 * @return �����������еĹ���׷�ٽ��
	*/
	std::shared_ptr<QOpenGLTexture> rayTracingRender(int thread_num);

private:
	void _setCamera_(std::shared_ptr<Camera> p_cam);

	/**
	 * @brief ׷�ٵ�������ֱ�������ֲ����������պл�������/��������ﵽ����
	 * @param ray ����
	 * @return ���߶�Ӧ���ص㴦��������ɫֵ
	*/
	QVector3D _traceRay_(GLRay& ray);

	/**
	 * @brief ���̹߳���׷��ʱÿ���̵߳��õĺ���
	 * @param thread_num ���߳���
	 * @param thread_idx ��ǰ�̱߳��
	 * @param img ��������ͼ��
	*/
	void _multiThreadRT_(const int thread_num, const int thread_idx, QImage& img);

	/**
	@brief ��ȡ�����볡��������ĵ�һ������
	@param ray : ����
	*/
	GLHitPoint _getFirstHit_(GLRay& ray);
};