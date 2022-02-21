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
	const aiScene* m_scene;		// assimp ��ʽ��������
	std::string m_dir;			// ģ���ļ�·��
	std::vector<BoundingBox> m_bounding_boxes;	// ģ�Ͱ�Χ��
	std::vector<igl::AABB<Eigen::MatrixXf, 3>> m_aabb_trees;	// aabb�������ڿ�����
	std::vector<Eigen::MatrixXf> m_mesh_vertices;	// ģ�����񶥵�
	std::vector<Eigen::MatrixXf> m_mesh_normals;	// ģ����������
	std::vector<Eigen::MatrixXf> m_mesh_tex_coors;	// ģ��������������
	std::vector<Eigen::MatrixXi> m_mesh_indices;	// ģ����������������
	std::vector<std::shared_ptr<QImage>> m_mesh_tex;	// ģ������
	QMatrix4x4 m_model_mat;							// ģ��6dofλ��

public:
	std::string m_model_name;

public:
	GLModel(const std::string& model_filename);
	virtual void paint() override;

	/**
	 * @brief ��ȡģ���ļ�
	*/
	virtual void init();

	/**
	 * @brief ������ײ��⣬�жϸ������Ƿ���ģ��AABB��Χ���ڲ�
	 * @param pos ��ά������
	 * @return ���������Ƿ���ģ���ڲ�
	*/
	bool collisionDetect(const QVector3D& pos);

	/**
	 * @brief �����󽻣��������GLObjectͬ������
	*/
	virtual bool intersectPoint(const GLRay& ray, GLHitPoint& hitpoint);

	/**
	 * @brief ���þֲ���ȫ������ϵת������
	 * @param mat ת������
	*/
	virtual void setLocToGlob(const QMatrix4x4& mat) override;

protected:
	/**
	 * @brief ��ȡģ���ļ��е��������ݣ���QT�������ഢ�棬ͬʱ����QTͼƬ�����ڹ�׷����
	 * @param textures ���ڱ�������
	 * @param mat assimp��������
	 * @param type ��������
	 * @param typeName ����
	 * @return ����ΪͼƬ����������
	*/
	std::shared_ptr<QImage> _loadMaterialTextures_(Textures& textures,
		aiMaterial* mat, aiTextureType type, std::string typeName);

	/**
	 * @brief ��ȡ����ķ�������Ϣ
	 * @param mesh_id ������
	 * @param ele_id �����α��
	 * @return �����εķ�����
	*/
	QVector3D _getMeshNormal_(int mesh_id, int ele_id);

	/**
	 * @brief ��ȡ�����������Ϣ
	 * @param mesh_id ������
	 * @param ele_id �����α��
	 * @param point ��ά����
	 * @return ���괦��Ӧ�������ɫֵ
	*/
	QVector3D _getMeshColor_(int mesh_id, int ele_id, QVector3D point);

	/**
	 * @brief �ݹ����assimp��ʽģ������
	 * @param node �ڵ�ָ��
	*/
	virtual void _processNode_(aiNode* node);

	/**
	 * @brief ����assimp��ʽ����������
	 * @param mesh ��������ָ��
	*/
	virtual void _processMesh_(aiMesh* mesh);
};