#pragma once

#include<QVector3D>
#include<json/json.h>

#include<memory>
#include<string>
#include<map>

class GLRay 
{
public:
	const static int MAX_HIT_TIMES = 5;		// ÿ����������ڼ������������ͷ�����ٴ�
	QVector3D m_start_pos;			// �������
	QVector3D m_dir;				// ��������
	int m_cur_hit_times;			// ��ǰ�����Ѿ�������߷����˶��ٴ�

public:
	GLRay(QVector3D start_pos, QVector3D dir);
	GLRay(QVector3D start_pos, QVector3D dir, int hit_times);
};

class GLMaterial
{
public:
	enum MATERIAL_TYPE 
	{
		MAT_JADE,				// ���
		MAT_OBSIDIAN,			// ����ʯ
		MAT_PEARL,				// ����
		MAT_BRASS,				// ��ͭ
		MAT_BRONZE,				// ��ͭ
		MAT_COPPER,				// ͭ
		MAT_WHITE_PLASTIC,		// ����
		MAT_SILVER,				// ����
		MAT_DEFAULT				// Ĭ��
	};

	QVector3D m_ka, m_kd, m_ks;		// �������գ������䣬���淴��ϵ��
	float m_shininess;				// ���淴��ǿ��
	float m_Il;						// ������������ɫռ��(�����ڷǴֲڼ�����)
	QVector3D m_F0;					// �����
	float m_ior;					// ������
	bool isRough;					// �Ƿ�ֲ�
	bool isRefractive;				// �Ƿ��������

public:
	GLMaterial(int type);
	GLMaterial(MATERIAL_TYPE type);
	GLMaterial(const std::string& mat_name);

	/**
	 * @brief ��json�����ļ��м��ز��ʣ����δ����������ior��͸������δ����Il��Ϊ�ֲڲ���
	 * @param val json����
	 * @return �������ݵ�ָ��
	*/
	static std::shared_ptr<GLMaterial> loadFromJson(Json::Value& val);

	/**
	@brief ���ò���Ϊ�������(������)
	@param Il : ������������ɫռ��
	*/
	void setSmooth(float Il);

	/**
	@brief ���ò���Ϊ͸������
	@param Il : ������������ɫռ��
	@param ior : ������
	@param F0 : �����
	*/
	void setSmooth(float Il, float ior, QVector3D F0);
};

class GLHitPoint
{
public:
	float m_dist;			// ���㵽���߳�����ľ���
	QVector3D m_pos;		// ������ά����
	QVector3D m_normal;		// ���㷨����
	QVector3D m_color;		// ���㴦����������ɫ(���û����������Ļ�Ĭ��Ϊ1,1,1)
	std::shared_ptr<GLMaterial> m_material;
	bool m_inner_hit;		// �Ƿ����ڲ�����(���ڳ�������ĳ���������ڲ��Ĺ��ߣ����ܼ����������뼸�����ڲ�������ķ���,��������Ҫȡ����)
};