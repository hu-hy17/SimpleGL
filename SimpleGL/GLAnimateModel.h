#pragma once
#include"GLModel.h"

#define MAX_BONE_NUM 12		// ����������

struct BoneVertex {
	QVector3D Position;
	QVector3D Normal;
	QVector2D TexCoords;
	GLfloat BoneWeights[MAX_BONE_NUM];
	GLfloat BoneIDs[MAX_BONE_NUM];
};

class GLAnimateMesh : public GLObject
{
private:
	std::string m_class_name = "(GLAnimateMesh)";
public:
	GLAnimateMesh(const std::vector<BoneVertex>& vertices,
		const std::vector<unsigned int>& indices,
		Textures& textures);
	virtual void paint() override;
	void paint(std::vector<QMatrix4x4>& m_bone_live_offset);
	void destroy();

protected:
	int m_indices_size;		// ��������
	Textures m_textures;	// ����
};

class GLAnimateModel : public GLModel
{
private:
	std::string m_class_name = "(GLAnimateModel)";

protected:
	std::vector<QMatrix4x4> m_bone_offset;		// ������ʼʱ��6dofλ��
	std::vector<QMatrix4x4> m_bone_live_offset;	// ������̬6dofλ��
	std::map<std::string, int> m_bone_map;		// �����ڵ����Ƶ���ŵ�ӳ��
	int m_animation_idx = -1;			// ��ǰ�����ı��
	QMatrix4x4 m_global_inv_transform; 

public:
	float m_speed_up = 1.0f;

public:
	/**
	 * @brief ���캯��
	 * @param model_filename ��Ҫ��ȡ��ģ���ļ�
	*/
	GLAnimateModel(const std::string& model_filename);

	/**
	 * @brief ���ö������б��(һ��fbx�ļ��а��������������)
	 * @param idx �������б��
	*/
	void setAnimationIdx(int idx);

	virtual void paint() override;
	void paint(float cur_time);

	/**
	 * @brief ����ģ���ļ�
	*/
	virtual void init() override;
	
protected:
	/**
	 * @brief ִ�ж���
	 * @param time_in_sec ����ִ��ʱ��(��λ:s)
	*/
	void _performAnimate_(float time_in_sec);

	/**
	 * @brief ����ģ���ļ��е���������
	 * @param mesh assimp���ʽ����������ָ��
	*/
	virtual void _processMesh_(aiMesh* mesh) override;

	/**
	 * @brief ����ģ���ļ��еĹ�����
	 * @param bone assimp���ʽ�Ĺ���
	 * @param vertices 
	 * @param bone_num 
	*/
	void _processBone_(aiBone* bone, std::vector<BoneVertex>& vertices, std::vector<int>& bone_num);

	/**
	 * @brief ͨ���ݹ�ķ��ʹ�����(�˶�ѧ��)�õ���ǰ����ʱ��ÿ�������Ķ�̬6dofλ��
	 * @param anim_time ����ʱ���
	 * @param pNode ��ǰ���ʵĹ����ڵ�ָ��
	 * @param parent_transformation ���ڵ��λ��
	 * @param pAnim assimp�ṹ�Ķ�������ָ��
	*/
	void _readNodeHeirarchy_(float anim_time, const aiNode* pNode,
		const QMatrix4x4& parent_transformation, const aiAnimation* pAnim);

	void _aiMatToQMat_(QMatrix4x4& qmat, const aiMatrix4x4& aiMat);
	void _aiMatToQMat_(QMatrix4x4& qmat, const aiMatrix3x3& aiMat);
	void _aiColor4ToQVec4_(QVector4D& qvec, const aiColor4D& aiColor);

	/**
	 * @brief �Ӷ����������ҵ�ĳ�������ڵ�Ķ�������
	 * @param pAnim assimp�ṹ�Ķ�������ָ��
	 * @param node_name �ڵ�����
	 * @return ��Ӧ�ڵ�Ķ�������ָ��
	*/
	aiNodeAnim* _findNodeAnim_(const aiAnimation* pAnim, const std::string& node_name);

	/**
	 * @brief Ѱ�Ҷ�����ǰ�����ĸ�λ�����ݹؼ�֡��
	 * @param AnimationTime ����ʱ���
	 * @param pNodeAnim assimp��ʽ�Ķ�������
	 * @return �ؼ�֡���
	*/
	int _findPosition_(float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief Ѱ�Ҷ�����ǰ�����ĸ���ת���ݹؼ�֡��
	 * @param AnimationTime ����ʱ���
	 * @param pNodeAnim assimp��ʽ�Ķ�������
	 * @return �ؼ�֡���
	*/
	int _findRotation_(float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief Ѱ�Ҷ�����ǰ�����ĸ��������ݹؼ�֡��
	 * @param AnimationTime ����ʱ���
	 * @param pNodeAnim assimp��ʽ�Ķ�������
	 * @return �ؼ�֡���
	*/
	int _findScaling_(float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief ��άλ�����Բ�ֵ
	 * @param Out �������
	 * @param AnimationTime ������ǰʱ���
	 * @param pNodeAnim assimp��ʽ�Ķ�������
	*/
	void _calcInterpolatedPosition_(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief ��ά��ת���Բ�ֵ����Ԫ����ֵ��
	 * @param Out �������
	 * @param AnimationTime ������ǰʱ���
	 * @param pNodeAnim assimp��ʽ�Ķ�������
	*/
	void _calcInterpolatedRotation_(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief ��ά�������Բ�ֵ
	 * @param Out �������
	 * @param AnimationTime ������ǰʱ���
	 * @param pNodeAnim assimp��ʽ�Ķ�������
	*/
	void _calcInterpolatedScaling_(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
};