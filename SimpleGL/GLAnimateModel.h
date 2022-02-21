#pragma once
#include"GLModel.h"

#define MAX_BONE_NUM 12		// 最大骨骼数量

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
	int m_indices_size;		// 顶点数量
	Textures m_textures;	// 纹理
};

class GLAnimateModel : public GLModel
{
private:
	std::string m_class_name = "(GLAnimateModel)";

protected:
	std::vector<QMatrix4x4> m_bone_offset;		// 骨骼初始时的6dof位姿
	std::vector<QMatrix4x4> m_bone_live_offset;	// 骨骼动态6dof位姿
	std::map<std::string, int> m_bone_map;		// 骨骼节点名称到编号的映射
	int m_animation_idx = -1;			// 当前动画的编号
	QMatrix4x4 m_global_inv_transform; 

public:
	float m_speed_up = 1.0f;

public:
	/**
	 * @brief 构造函数
	 * @param model_filename 需要读取的模型文件
	*/
	GLAnimateModel(const std::string& model_filename);

	/**
	 * @brief 设置动作序列编号(一个fbx文件中包含多个动作序列)
	 * @param idx 动作序列编号
	*/
	void setAnimationIdx(int idx);

	virtual void paint() override;
	void paint(float cur_time);

	/**
	 * @brief 解析模型文件
	*/
	virtual void init() override;
	
protected:
	/**
	 * @brief 执行动画
	 * @param time_in_sec 动画执行时长(单位:s)
	*/
	void _performAnimate_(float time_in_sec);

	/**
	 * @brief 解析模型文件中的网格数据
	 * @param mesh assimp库格式的网格数据指针
	*/
	virtual void _processMesh_(aiMesh* mesh) override;

	/**
	 * @brief 解析模型文件中的骨骼绑定
	 * @param bone assimp库格式的骨骼
	 * @param vertices 
	 * @param bone_num 
	*/
	void _processBone_(aiBone* bone, std::vector<BoneVertex>& vertices, std::vector<int>& bone_num);

	/**
	 * @brief 通过递归的访问骨骼树(运动学链)得到当前动画时刻每个骨骼的动态6dof位姿
	 * @param anim_time 动画时间戳
	 * @param pNode 当前访问的骨骼节点指针
	 * @param parent_transformation 父节点的位姿
	 * @param pAnim assimp结构的动画数据指针
	*/
	void _readNodeHeirarchy_(float anim_time, const aiNode* pNode,
		const QMatrix4x4& parent_transformation, const aiAnimation* pAnim);

	void _aiMatToQMat_(QMatrix4x4& qmat, const aiMatrix4x4& aiMat);
	void _aiMatToQMat_(QMatrix4x4& qmat, const aiMatrix3x3& aiMat);
	void _aiColor4ToQVec4_(QVector4D& qvec, const aiColor4D& aiColor);

	/**
	 * @brief 从动画数据中找到某个骨骼节点的动画数据
	 * @param pAnim assimp结构的动画数据指针
	 * @param node_name 节点名称
	 * @return 相应节点的动画数据指针
	*/
	aiNodeAnim* _findNodeAnim_(const aiAnimation* pAnim, const std::string& node_name);

	/**
	 * @brief 寻找动画当前处在哪个位置数据关键帧处
	 * @param AnimationTime 动画时间戳
	 * @param pNodeAnim assimp格式的动画数据
	 * @return 关键帧编号
	*/
	int _findPosition_(float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief 寻找动画当前处在哪个旋转数据关键帧处
	 * @param AnimationTime 动画时间戳
	 * @param pNodeAnim assimp格式的动画数据
	 * @return 关键帧编号
	*/
	int _findRotation_(float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief 寻找动画当前处在哪个放缩数据关键帧处
	 * @param AnimationTime 动画时间戳
	 * @param pNodeAnim assimp格式的动画数据
	 * @return 关键帧编号
	*/
	int _findScaling_(float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief 三维位置线性插值
	 * @param Out 结果储存
	 * @param AnimationTime 动画当前时间戳
	 * @param pNodeAnim assimp格式的动画数据
	*/
	void _calcInterpolatedPosition_(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief 三维旋转线性插值（四元数插值）
	 * @param Out 结果储存
	 * @param AnimationTime 动画当前时间戳
	 * @param pNodeAnim assimp格式的动画数据
	*/
	void _calcInterpolatedRotation_(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	/**
	 * @brief 三维放缩线性插值
	 * @param Out 结果储存
	 * @param AnimationTime 动画当前时间戳
	 * @param pNodeAnim assimp格式的动画数据
	*/
	void _calcInterpolatedScaling_(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
};