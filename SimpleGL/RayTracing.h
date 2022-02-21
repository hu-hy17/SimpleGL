#pragma once

#include<QVector3D>
#include<json/json.h>

#include<memory>
#include<string>
#include<map>

class GLRay 
{
public:
	const static int MAX_HIT_TIMES = 5;		// 每根光束最多在几何体表面折射和反射多少次
	QVector3D m_start_pos;			// 光束起点
	QVector3D m_dir;				// 光束方向
	int m_cur_hit_times;			// 当前光束已经折射或者反射了多少次

public:
	GLRay(QVector3D start_pos, QVector3D dir);
	GLRay(QVector3D start_pos, QVector3D dir, int hit_times);
};

class GLMaterial
{
public:
	enum MATERIAL_TYPE 
	{
		MAT_JADE,				// 翡翠
		MAT_OBSIDIAN,			// 黑曜石
		MAT_PEARL,				// 珍珠
		MAT_BRASS,				// 黄铜
		MAT_BRONZE,				// 青铜
		MAT_COPPER,				// 铜
		MAT_WHITE_PLASTIC,		// 塑料
		MAT_SILVER,				// 白银
		MAT_DEFAULT				// 默认
	};

	QVector3D m_ka, m_kd, m_ks;		// 环境光照，漫反射，镜面反射系数
	float m_shininess;				// 镜面反射强度
	float m_Il;						// 几何体自身颜色占比(仅对于非粗糙几何体)
	QVector3D m_F0;					// 反射比
	float m_ior;					// 折射率
	bool isRough;					// 是否粗糙
	bool isRefractive;				// 是否产生折射

public:
	GLMaterial(int type);
	GLMaterial(MATERIAL_TYPE type);
	GLMaterial(const std::string& mat_name);

	/**
	 * @brief 从json配置文件中加载材质，如果未设置折射率ior则不透明，若未设置Il则为粗糙材质
	 * @param val json数据
	 * @return 材质数据的指针
	*/
	static std::shared_ptr<GLMaterial> loadFromJson(Json::Value& val);

	/**
	@brief 设置材质为镜面材质(不折射)
	@param Il : 几何体自身颜色占比
	*/
	void setSmooth(float Il);

	/**
	@brief 设置材质为透明材质
	@param Il : 几何体自身颜色占比
	@param ior : 折射率
	@param F0 : 反射比
	*/
	void setSmooth(float Il, float ior, QVector3D F0);
};

class GLHitPoint
{
public:
	float m_dist;			// 交点到光线出发点的距离
	QVector3D m_pos;		// 交点三维坐标
	QVector3D m_normal;		// 交点法向量
	QVector3D m_color;		// 交点处物体自身颜色(如果没有设置纹理的话默认为1,1,1)
	std::shared_ptr<GLMaterial> m_material;
	bool m_inner_hit;		// 是否是内部交点(对于出发点在某个几何体内部的光线，不能继续计算其与几何体内部其它面的反射,且折射率要取倒数)
};