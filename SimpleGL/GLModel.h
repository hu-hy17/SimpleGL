#pragma once
#include<map>

#include<assimp/cimport.h>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<igl/AABB.h>

#include<Eigen/Dense>

#include"GLObject.h"
#include"GLMixObject.h"

typedef std::vector<std::shared_ptr<QOpenGLTexture>> Textures;

struct Vertex {
	QVector3D Position;
	QVector3D Normal;
	QVector2D TexCoords;
};

struct BoundingBox {
	QVector3D mMax;
	QVector3D mMin;
};

class GLMesh : public GLObject 
{
public:
	GLMesh(const std::vector<Vertex>& vertices, 
		const std::vector<unsigned int>& indices, 
		Textures& textures);
	virtual void destroy() override;
	virtual void paint() override;

protected:
	int m_indices_size;
	Textures m_textures;
};

class GLModel : public GLMixObject
{
protected:
	const std::string m_class_name = "(GLModel)";
	const aiScene* m_scene;		// assimp 格式场景数据
	std::string m_dir;			// 模型文件路径
	std::vector<BoundingBox> m_bounding_boxes;	// 模型包围盒
	std::vector<igl::AABB<Eigen::MatrixXf, 3>> m_aabb_trees;	// aabb树，用于快速求交
	std::vector<Eigen::MatrixXf> m_mesh_vertices;	// 模型网格顶点
	std::vector<Eigen::MatrixXf> m_mesh_normals;	// 模型网格法向量
	std::vector<Eigen::MatrixXf> m_mesh_tex_coors;	// 模型网格纹理坐标
	std::vector<Eigen::MatrixXi> m_mesh_indices;	// 模型网格三角形索引
	std::vector<std::shared_ptr<QImage>> m_mesh_tex;	// 模型纹理
	QMatrix4x4 m_model_mat;							// 模型6dof位姿

public:
	std::string m_model_name;

public:
	GLModel(const std::string& model_filename);
	virtual void paint() override;

	/**
	 * @brief 读取模型文件
	*/
	virtual void init();

	/**
	 * @brief 简易碰撞检测，判断给定点是否在模型AABB包围盒内部
	 * @param pos 三维点坐标
	 * @return 给定坐标是否在模型内部
	*/
	bool collisionDetect(const QVector3D& pos);

	/**
	 * @brief 光线求交，详见基类GLObject同名函数
	*/
	virtual bool intersectPoint(const GLRay& ray, GLHitPoint& hitpoint);

	/**
	 * @brief 设置局部到全局坐标系转换矩阵
	 * @param mat 转换矩阵
	*/
	virtual void setLocToGlob(const QMatrix4x4& mat) override;

protected:
	/**
	 * @brief 读取模型文件中的纹理数据，用QT的纹理类储存，同时返回QT图片类用于光追计算
	 * @param textures 用于保存纹理
	 * @param mat assimp材质数据
	 * @param type 纹理类型
	 * @param typeName 无用
	 * @return 储存为图片的纹理数据
	*/
	std::shared_ptr<QImage> _loadMaterialTextures_(Textures& textures,
		aiMaterial* mat, aiTextureType type, std::string typeName);

	/**
	 * @brief 获取网格的法向量信息
	 * @param mesh_id 网格编号
	 * @param ele_id 三角形编号
	 * @return 三角形的法向量
	*/
	QVector3D _getMeshNormal_(int mesh_id, int ele_id);

	/**
	 * @brief 获取网格的纹理信息
	 * @param mesh_id 网格编号
	 * @param ele_id 三角形编号
	 * @param point 三维坐标
	 * @return 坐标处相应纹理的颜色值
	*/
	QVector3D _getMeshColor_(int mesh_id, int ele_id, QVector3D point);

	/**
	 * @brief 递归解析assimp格式模型数据
	 * @param node 节点指针
	*/
	virtual void _processNode_(aiNode* node);

	/**
	 * @brief 解析assimp格式的网格数据
	 * @param mesh 网格数据指针
	*/
	virtual void _processMesh_(aiMesh* mesh);
};