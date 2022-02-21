#include"GLScene.h"
#include"Utils.h"

#include<iostream>
#include<fstream>
#include<thread>

#define M_PI 3.1415926

GLScene::GLScene()
{
	this->initializeOpenGLFunctions();
}

void GLScene::setCamera(std::shared_ptr<Camera> p_cam)
{
	m_global_cam = p_cam;
	_setCamera_(p_cam);
}

void GLScene::setLight(std::shared_ptr<GLLight> p_light)
{
	if (m_mirror)
		m_mirror->setLight(p_light);
	for (int i = 0; i < m_ani_models.size(); i++)
		m_ani_models[i]->setLight(p_light);
	for (int i = 0; i < m_static_models.size(); i++)
		m_static_models[i]->setLight(p_light);
	for (int i = 0; i < m_simple_objs.size(); i++)
		m_simple_objs[i]->setLight(p_light);
	m_light = p_light;
}

void GLScene::prepareMirror()
{
	m_mirror_fbo->bind();
	{
		glEnable(GL_DEPTH_TEST);
		// 背景
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 设置镜子视角的投影矩阵和相机矩阵
		_setCamera_(m_mirror_cam);
		glViewport(0, 0, m_mirror->getWidth(), m_mirror->getHeight());
		paintObjects();
	}
	m_mirror_fbo->release();

	_setCamera_(m_global_cam);
	glViewport(0, 0, m_global_cam->screen_width, m_global_cam->screen_height);
}

void GLScene::paintAll()
{
	// 色彩混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_mirror->paint(m_mirror_fbo.get());
	paintObjects();
}

void GLScene::paintObjects()
{
	// 天空盒
	m_skybox->paint();

	// 动态模型
	for (int i = 0; i < m_ani_models.size(); i++)
	{
		auto pModel = m_ani_models[i].get();
		pModel->paint(pModel->m_speed_up * (float)(clock()) / CLOCKS_PER_SEC);
	}

	// 静态模型
	for (int i = 0; i < m_static_models.size(); i++)
	{
		auto pModel = m_static_models[i].get();
		pModel->paint();
	}

	// 简单几何体
	glDepthMask(GL_FALSE);
	for (int i = 0; i < m_simple_objs.size(); i++)
	{
		auto pObj = m_simple_objs[i].get();
		pObj->paint();
	}
	glDepthMask(GL_TRUE);
}

void GLScene::destory()
{
	if (m_mirror)
		m_mirror->destroy();
	if(m_skybox)
		m_skybox->destroy();
	for (int i = 0; i < m_ani_models.size(); i++)
	{
		m_ani_models[i]->destroy();
	}
	for (int i = 0; i < m_static_models.size(); i++)
	{
		m_static_models[i]->destroy();
	}
	for (int i = 0; i < m_simple_objs.size(); i++)
	{
		m_simple_objs[i]->destroy();
	}
}

std::string GLScene::getCollision()
{
	std::string colli_list = "";
	for (int i = 0; i < m_static_models.size(); i++)
	{
		if (m_static_models[i]->collisionDetect(m_global_cam->position))
			colli_list = colli_list + " " + m_static_models[i]->m_model_name;
	}
	for (int i = 0; i < m_ani_models.size(); i++)
	{
		if (m_ani_models[i]->collisionDetect(m_global_cam->position))
			colli_list = colli_list + " " + m_ani_models[i]->m_model_name;
	}
	for (int i = 0; i < m_simple_objs.size(); i++)
	{
		if (m_simple_objs[i]->collisionDetect(m_global_cam->position))
			colli_list = colli_list + " " + m_simple_objs[i]->getObjTypeName();
	}
	return colli_list;
}

void GLScene::loadSceneFromFile(const std::string& filename)
{
	std::ifstream ifs(filename);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	std::string json_str(buffer.str());

	Json::Value json_root;
	Json::CharReaderBuilder reader_builder;
	Json::CharReader* reader(reader_builder.newCharReader());
	JSONCPP_STRING errs;

	if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size(), &json_root, &errs))
	{
		std::cerr << m_class_name << "Error: Open json file " << filename << " fail!" << endl;
		std::cerr << errs << endl;
		system("pause");
		exit(-1);
	}

	// 天空盒
	assert(json_root["skybox"].size() == 6);
	std::vector<QString> skybox_surface;
	for (int i = 0; i < 6; i++)
		skybox_surface.push_back(QString(json_root["skybox"][i].asString().c_str()));
	m_skybox = std::make_unique<GLSkyBox>(skybox_surface);

	// 物体
	int obj_num = json_root["objects"].size();
	QMatrix4x4 obj_motion;
	for (int i = 0; i < obj_num; i++)
	{
		auto& obj_desc = json_root["objects"][i];
		if (obj_desc["type"] == "AnimateModel")
		{
			m_ani_models.push_back(
				std::make_unique<GLAnimateModel>(obj_desc["fromfile"].asString()));
			GLAnimateModel* pModel = m_ani_models.back().get();

			pModel->init();
			pModel->m_model_name = obj_desc["name"].asString();
			pModel->setAnimationIdx(obj_desc["animationIdx"].asInt());
			pModel->setLocToGlob(getQMatFromJson(obj_desc["motion"]));
			pModel->setColor(QVector4D(1, 1, 1, 1));
			pModel->setMaterial(GLMaterial::loadFromJson(obj_desc["material"]));
			pModel->m_speed_up = obj_desc["speedup"].asFloat();
		}
		else if (obj_desc["type"] == "StaticModel")
		{
			m_static_models.push_back(
				std::make_unique<GLModel>(obj_desc["fromfile"].asString()));
			GLModel* pModel = m_static_models.back().get();
			pModel->init();
			pModel->m_model_name = obj_desc["name"].asString();
			pModel->setLocToGlob(getQMatFromJson(obj_desc["motion"]));
			pModel->setColor(QVector4D(1, 1, 1, 1));
			pModel->setMaterial(GLMaterial::loadFromJson(obj_desc["material"]));
		}
		else if (obj_desc["type"] == "Sphere")
		{
			m_simple_objs.push_back(std::make_unique<GLSphere>());
			GLSphere* pObj = dynamic_cast<GLSphere*>(m_simple_objs.back().get());
			pObj->setRadius(obj_desc["radius"].asFloat());
			pObj->setCenterPos(getVec3FromJson(obj_desc["center"]));
			pObj->setColor(getVec4FromJson(obj_desc["color"]));
			pObj->setMaterial(GLMaterial::loadFromJson(obj_desc["material"]));
		}
		else if (obj_desc["type"] == "Mirror")
		{
			if (m_mirror)
			{
				std::cout << m_class_name << "Warning : mirror has already existed, the new one will be ignored!" << std::endl;
				continue;
			}
			// 镜面
			m_mirror = std::make_unique<GLMirror>();
			float width = obj_desc["width"].asFloat();
			float height = obj_desc["height"].asFloat();
			m_mirror->setSize(width, height);

			QMatrix4x4 motion = getQMatFromJson(obj_desc["motion"]);
			m_mirror->setLocToGlob(motion);

			// 根据motion设置像机参数
			QVector3D cam_pos(motion(0, 3), motion(1, 3), motion(2, 3));
			QVector3D front_dir(motion * QVector4D(0, 0, -1, 0));
			front_dir.normalize();
			cam_pos = cam_pos - front_dir * std::max(width, height);

			float cam_pitch = M_PI / 2 - acosf(QVector3D::dotProduct(front_dir, QVector3D(0, 1, 0)));
			float xzlen = sqrtf(front_dir.x() * front_dir.x() + front_dir.z() * front_dir.z());
			float cam_yaw = 0;
			if (xzlen >= 1e-5)
			{
				cam_yaw = acosf(front_dir.x() / xzlen);
				cam_yaw = front_dir.z() >= 0 ? cam_yaw : -cam_yaw;
			}

			m_mirror_cam = std::make_shared<Camera>(cam_pos, QVector3D(0, 1, 0), cam_yaw, cam_pitch);
			m_mirror_cam->setScreenSize(width, height);
			m_mirror_fbo = std::make_unique<QOpenGLFramebufferObject>(width, height,
				QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D, GL_RGB);

			m_mirror->setMaterial(GLMaterial::loadFromJson(obj_desc["material"]));
		}
	}
}

void GLScene::_multiThreadRT_(const int thread_num, const int thread_idx, QImage& img)
{
	for (int py = thread_idx; py < m_global_cam->screen_height; py += thread_num)
	{
		for (int px = 0; px < m_global_cam->screen_width; px++)
		{
			GLRay initRay = m_global_cam->getRay(px, py);
			QVector3D color = _traceRay_(initRay);
			for (int i = 0; i < 3; i++)
				color[i] = std::min(color[i], 1.0f);
			img.setPixel(px, py, qRgb(color[0] * 255, color[1] * 255, color[2] * 255));
		}
	}
}

std::shared_ptr<QOpenGLTexture> GLScene::rayTracingRender(int thread_num)
{
	QImage img(m_global_cam->screen_width, m_global_cam->screen_height, QImage::Format_RGB888);
	bool use_multithread = thread_num >= 2;
	if (use_multithread)
	{
		std::vector<std::thread> threads(thread_num - 1);
		for (int ti = 0; ti < thread_num - 1; ti++)
		{
			threads[ti] = std::thread(&GLScene::_multiThreadRT_, this, thread_num, ti + 1, std::ref(img));
		}
		_multiThreadRT_(thread_num, 0, img);

		for (int ti = 0; ti < thread_num - 1; ti++)
		{
			threads[ti].join();
		}
	}
	else
	{
		_multiThreadRT_(1, 0, img);
	}

	// 将图片绑定到纹理
	std::shared_ptr<QOpenGLTexture> ptex = std::make_shared<QOpenGLTexture>(img, QOpenGLTexture::GenerateMipMaps);
	ptex->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
	ptex->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
	ptex->setMinificationFilter(QOpenGLTexture::Linear);
	ptex->setMagnificationFilter(QOpenGLTexture::Linear);

	return ptex;
}

void GLScene::_setCamera_(std::shared_ptr<Camera> p_cam)
{
	if (m_skybox)
		m_skybox->setCamera(p_cam);
	if (m_mirror)
		m_mirror->setCamera(p_cam);
	for (int i = 0; i < m_ani_models.size(); i++)
		m_ani_models[i]->setCamera(p_cam);
	for (int i = 0; i < m_static_models.size(); i++)
		m_static_models[i]->setCamera(p_cam);
	for (int i = 0; i < m_simple_objs.size(); i++)
		m_simple_objs[i]->setCamera(p_cam);
}

QVector3D GLScene::_traceRay_(GLRay& ray)
{
	double eps = 1e-2;
	if (ray.m_cur_hit_times > GLRay::MAX_HIT_TIMES)
		return m_light->m_light_color;
	GLHitPoint hit = _getFirstHit_(ray);

	if (hit.m_dist < 0)
	{
		// 光线没有碰到任何几何体，返回天空盒的颜色
		if (m_skybox)
			return m_skybox->getRayColor(ray);
		else
			return m_light->m_light_color;
	}

	QVector3D outRadiance = hit.m_material->m_ka * m_light->m_light_color;

	QVector3D light_dir = (m_light->m_light_pos - hit.m_pos).normalized();
	float cos_theta = QVector3D::dotProduct(hit.m_normal, light_dir);
	if (cos_theta > 0)
	{
		// 漫反射光
		outRadiance += cos_theta * hit.m_material->m_kd * m_light->m_light_color;

		// 镜面反射光
		QVector3D inLightDir = (hit.m_pos - m_light->m_light_pos).normalized();
		QVector3D lightReflectedDir = inLightDir - hit.m_normal * QVector3D::dotProduct(hit.m_normal, inLightDir) * 2.0f;
		float spec = std::powf(std::max(QVector3D::dotProduct(light_dir, lightReflectedDir), 0.0f),
			hit.m_material->m_shininess * 256);
		outRadiance += spec * hit.m_material->m_ks * m_light->m_light_color;
	}
	outRadiance *= hit.m_color;

	if (hit.m_material->isRough)
	{
		// 对于粗糙表面,不再往下进行追踪
		return outRadiance;
	}

	outRadiance *= hit.m_material->m_Il;

	QVector3D normal = hit.m_inner_hit ? -hit.m_normal : hit.m_normal;
	float cosa = -QVector3D::dotProduct(ray.m_dir, normal);			// 入射角
	QVector3D ones(1, 1, 1);
	QVector3D F = hit.m_material->m_F0 + (ones - hit.m_material->m_F0) * powf(1 - cosa, 5);

	if (!hit.m_inner_hit)
	{
		// 物体表面镜面反射
		QVector3D reflectedDir = ray.m_dir - hit.m_normal * QVector3D::dotProduct(hit.m_normal, ray.m_dir) * 2.0f;		// 反射光线方向
		GLRay reflect_ray(hit.m_pos + eps * hit.m_normal, reflectedDir, ray.m_cur_hit_times + 1);
		outRadiance += _traceRay_(reflect_ray) * F;
	}

	if (hit.m_material->isRefractive)
	{
		assert(hit.m_material->m_ior > 0);
		float ior = hit.m_inner_hit ? 1.0f / hit.m_material->m_ior : hit.m_material->m_ior;

		// 透射光追踪
		float disc = 1 - (1 - cosa * cosa) / (ior * ior);
		if (disc >= 0)
		{
			QVector3D refractedDir = ray.m_dir / ior + normal * (cosa / ior - sqrt(disc));
			GLRay refract_ray(hit.m_pos - eps * normal, refractedDir, ray.m_cur_hit_times + 1);
			outRadiance = outRadiance + _traceRay_(refract_ray) * (ones - F);
		}
	}

	return outRadiance;
}

GLHitPoint GLScene::_getFirstHit_(GLRay& ray)
{
	GLHitPoint hit;
	hit.m_dist = -1;
	for (int i = 0; i < m_simple_objs.size(); i++)
	{
		m_simple_objs[i]->intersectPoint(ray, hit);
	}
	for (int i = 0; i < m_static_models.size(); i++)
	{
		m_static_models[i]->intersectPoint(ray, hit);
	}
	if (m_mirror)
		m_mirror->intersectPoint(ray, hit);
	return hit;
}