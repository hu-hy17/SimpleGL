#include"GLAnimateModel.h"
#include<algorithm>
#include<iostream>
#include<fstream>

using std::cerr;
using std::endl;

/************************************************************************/
/* GLAnimateMesh														*/
/************************************************************************/
GLAnimateMesh::GLAnimateMesh(
	const std::vector<BoneVertex>& vertices,
	const std::vector<unsigned int>& indices,
	Textures& textures)
{
	_compileAndLinkShader_(m_class_name, ":/shader/resources/shader/animate_mesh.vert", ":/shader/resources/shader/mesh.frag");
	QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(&vertices[0], vertices.size() * sizeof(BoneVertex));

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(&indices[0], indices.size() * sizeof(unsigned int));

	unsigned int offset = 0;
	// 坐标
	int attr = -1;
	attr = m_shader.attributeLocation(san::ATTR_POSITION);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, offset, 3, sizeof(BoneVertex));
	m_shader.enableAttributeArray(attr);
	offset += sizeof(QVector3D);

	// 法向量
	attr = m_shader.attributeLocation(san::ATTR_NORMAL);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, offset, 3, sizeof(BoneVertex));
	m_shader.enableAttributeArray(attr);
	offset += sizeof(QVector3D);

	// 纹理
	attr = m_shader.attributeLocation(san::ATTR_TEXTURE);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, offset, 2, sizeof(BoneVertex));
	m_shader.enableAttributeArray(attr);
	offset += sizeof(QVector2D);

	// 骨骼权重
	for (int i = 0; i < 3; i++)
	{
		attr = m_shader.attributeLocation(san::ATTR_BONE_WEIGHT[i]);
		m_shader.setAttributeBuffer(attr, GL_FLOAT, offset, 4, sizeof(BoneVertex));
		m_shader.enableAttributeArray(attr);
		offset += 4 * sizeof(GL_FLOAT);
	}

	// 骨骼编号
	for (int i = 0; i < 3; i++)
	{
		attr = m_shader.attributeLocation(san::ATTR_BONE_IDX[i]);
		m_shader.setAttributeBuffer(attr, GL_FLOAT, offset, 4, sizeof(BoneVertex));
		m_shader.enableAttributeArray(attr);
		offset += 4 * sizeof(GL_FLOAT);
	}

	m_indices_size = indices.size();
	m_textures = textures;
	if (m_textures.size())
	{
		m_shader.bind();
		m_shader.setUniformValue(san::ATTR_USE_TEX, true);
		m_shader.setUniformValue(san::ATTR_TEX_0, 0);
		m_shader.release();
	}

	m_vbo.release();
}

void GLAnimateMesh::paint()
{
	m_shader.bind();
	_setLightCond_();

	m_shader.setUniformValue(san::ATTR_ANIMATE, false);							// 是否进行动画
	m_shader.setUniformValue(san::ATTR_PROJ_MAT, m_camera->getProjectionMatrix());					// 投影矩阵
	m_shader.setUniformValue(san::ATTR_VIEW_MAT, m_camera->getViewMatrix());	// 摄像机矩阵
	m_shader.setUniformValue(san::ATTR_LOC_TO_GLOB_MAT, m_local_to_global);		// 局部到全局坐标转换
	{
		if (m_textures.size())
		{
			glActiveTexture(GL_TEXTURE0);
			m_textures.front()->bind();
		}

		QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);
		glDrawElements(GL_TRIANGLES, m_indices_size, GL_UNSIGNED_INT, 0);

		if (m_textures.size())
		{
			m_textures.front()->release();
		}
	}
	m_shader.release();
}

void GLAnimateMesh::paint(std::vector<QMatrix4x4>& m_bone_live_offset)
{
	if (!m_textures.size())
		return;
	m_shader.bind();
	_setLightCond_();

	// 骨骼位姿
	m_shader.setUniformValueArray(san::ATTR_BONE_ARR, &m_bone_live_offset[0], m_bone_live_offset.size());
	m_shader.setUniformValue(san::ATTR_ANIMATE, true);							// 是否进行动画
	m_shader.setUniformValue(san::ATTR_PROJ_MAT, m_camera->getProjectionMatrix());					// 投影矩阵
	m_shader.setUniformValue(san::ATTR_VIEW_MAT, m_camera->getViewMatrix());	// 摄像机矩阵
	m_shader.setUniformValue(san::ATTR_LOC_TO_GLOB_MAT, m_local_to_global);		// 局部到全局坐标转换
	{
		glActiveTexture(GL_TEXTURE0);
		m_textures.front()->bind();

		QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);
		glDrawElements(GL_TRIANGLES, m_indices_size, GL_UNSIGNED_INT, 0);

		m_textures.front()->release();
	}
	m_shader.release();
}

void GLAnimateMesh::destroy()
{
	__super::destroy();
	for (int i = 0; i < m_textures.size(); i++)
		m_textures[i]->destroy();
}

/************************************************************************/
/* GLAnimateModel														*/
/************************************************************************/
GLAnimateModel::GLAnimateModel(const std::string& model_filename) : 
	GLModel(model_filename)
{
}

void GLAnimateModel::init()
{
	_processNode_(m_scene->mRootNode);
}

void GLAnimateModel::paint()
{
	for (int i = 0; i < m_globjects.size(); i++)
	{
		m_globjects[i]->paint();
	}
}

void GLAnimateModel::paint(float cur_time)
{
	if (m_animation_idx < 0)
		paint();
	else
	{
		_performAnimate_(cur_time);
		for (int i = 0; i < m_globjects.size(); i++)
		{
			dynamic_cast<GLAnimateMesh*>(m_globjects[i].get())->paint(m_bone_live_offset);
		}
	}
}

void GLAnimateModel::setAnimationIdx(int idx)
{
	int ani_num = m_scene->mNumAnimations;
	if (idx < ani_num)
		m_animation_idx = idx;
}

void GLAnimateModel::_processMesh_(aiMesh* mesh)
{
	std::vector<BoneVertex> vertices;
	std::vector<unsigned int> indices;
	Textures textures;

	// 顶点位置,法线,坐标纹理,颜色
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		BoneVertex vertex;

		vertex.Position = QVector3D(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = QVector3D(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0])
			vertex.TexCoords = QVector2D(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vertex.TexCoords = QVector2D(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// 包围盒
	float xMax, xMin, yMax, yMin, zMax, zMin;
	xMax = yMax = zMax = INT_MIN;
	xMin = yMin = zMin = INT_MAX;
	for (BoneVertex& v : vertices)
	{
		xMax = std::max(xMax, v.Position.x());
		yMax = std::max(yMax, v.Position.y());
		zMax = std::max(zMax, v.Position.z());
		xMin = std::min(xMin, v.Position.x());
		yMin = std::min(yMin, v.Position.y());
		zMin = std::min(zMin, v.Position.z());
	}
	BoundingBox bBox;
	bBox.mMax = QVector3D(xMax, yMax, zMax);
	bBox.mMin = QVector3D(xMin, yMin, zMin);
	m_bounding_boxes.push_back(bBox);

	// 索引
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// 材质
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = m_scene->mMaterials[mesh->mMaterialIndex];
		_loadMaterialTextures_(textures, material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		_loadMaterialTextures_(textures, material,
			aiTextureType_SPECULAR, "texture_specular");
	}

	// 骨骼
	std::vector<int> bone_num(vertices.size(), 0);
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		_processBone_(mesh->mBones[i], vertices, bone_num);
	}
	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = bone_num[i]; j < MAX_BONE_NUM; j++)
			vertices[i].BoneIDs[j] = vertices[i].BoneWeights[j] = 0;
	}

	m_globjects.push_back(std::make_unique<GLAnimateMesh>(vertices, indices, textures));
}

void GLAnimateModel::_processBone_(aiBone* bone, std::vector<BoneVertex>& vertices, std::vector<int>& bone_num)
{
	std::string bone_name(bone->mName.data);
	int bone_idx;
	if (m_bone_map.find(bone_name) == m_bone_map.end())
	{
		// 加入当前骨骼
		QMatrix4x4 offset;
		_aiMatToQMat_(offset, bone->mOffsetMatrix);
		m_bone_offset.push_back(offset);
		bone_idx = m_bone_offset.size() - 1;
		m_bone_map[bone_name] = bone_idx;
	}
	else
	{
		bone_idx = m_bone_map[bone_name];
	}

	for (int i = 0; i < bone->mNumWeights; i++)
	{
		int vId = bone->mWeights[i].mVertexId;
		int nextBoneIdx = bone_num[vId];
		assert(nextBoneIdx < MAX_BONE_NUM);
		vertices[vId].BoneIDs[nextBoneIdx] = bone_idx;
		vertices[vId].BoneWeights[nextBoneIdx] = bone->mWeights[i].mWeight;
		++bone_num[vId];
	}
}

void GLAnimateModel::_performAnimate_(float time_in_sec)
{
	QMatrix4x4 mat;
	mat.setToIdentity();
	aiAnimation* p_ani = m_scene->mAnimations[m_animation_idx];
	float TicksPerSecond = p_ani->mTicksPerSecond != 0 ? p_ani->mTicksPerSecond : 25.0f;
	float TimeInTicks = time_in_sec * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, p_ani->mDuration);

	_aiMatToQMat_(m_global_inv_transform, m_scene->mRootNode->mTransformation);
	m_bone_live_offset.resize(m_bone_offset.size());
	_readNodeHeirarchy_(AnimationTime, m_scene->mRootNode, mat, p_ani);
}

void GLAnimateModel::_aiMatToQMat_(QMatrix4x4& qmat, const aiMatrix4x4& aiMat)
{
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
			qmat(r, c) = aiMat[r][c];
	}
}

void GLAnimateModel::_aiMatToQMat_(QMatrix4x4& qmat, const aiMatrix3x3& aiMat)
{
	qmat.setToIdentity();
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
			qmat(r, c) = aiMat[r][c];
	}
}

void GLAnimateModel::_aiColor4ToQVec4_(QVector4D& qvec, const aiColor4D& aiColor)
{
	qvec.setX(aiColor.r);
	qvec.setY(aiColor.g);
	qvec.setZ(aiColor.b);
	qvec.setW(aiColor.a);
}

aiNodeAnim* GLAnimateModel::_findNodeAnim_(const aiAnimation* pAnim, const std::string& node_name)
{
	for (int i = 0; i < pAnim->mNumChannels; i++)
	{
		std::string tmpName(pAnim->mChannels[i]->mNodeName.C_Str());
		if (tmpName == node_name)
			return pAnim->mChannels[i];
	}
	return nullptr;
}

void GLAnimateModel::_readNodeHeirarchy_(float anim_time, const aiNode* pNode,
	const QMatrix4x4& parent_transformation, const aiAnimation* pAnim)
{
	std::string NodeName(pNode->mName.data);
	QMatrix4x4 NodeTransformation;
	_aiMatToQMat_(NodeTransformation, pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = _findNodeAnim_(pAnim, NodeName);
	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		_calcInterpolatedScaling_(Scaling, anim_time, pNodeAnim);
		QMatrix4x4 ScalingM;
		ScalingM.scale(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		_calcInterpolatedRotation_(RotationQ, anim_time, pNodeAnim);
		QMatrix4x4 RotationM;
		_aiMatToQMat_(RotationM, RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		_calcInterpolatedPosition_(Translation, anim_time, pNodeAnim);
		QMatrix4x4 TranslationM;
		TranslationM.setToIdentity();
		TranslationM(0, 3) = Translation.x;
		TranslationM(1, 3) = Translation.y;
		TranslationM(2, 3) = Translation.z;

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	QMatrix4x4 GlobalTransformation = parent_transformation * NodeTransformation;
	if (m_bone_map.find(NodeName) != m_bone_map.end()) {
		int BoneIndex = m_bone_map[NodeName];
		m_bone_live_offset[BoneIndex] = m_global_inv_transform * GlobalTransformation *
			m_bone_offset[BoneIndex];
	}
	for (uint i = 0; i < pNode->mNumChildren; i++) {
		_readNodeHeirarchy_(anim_time, pNode->mChildren[i], GlobalTransformation, pAnim);
	}
}

int GLAnimateModel::_findPosition_(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}
	assert(0);
	return 0;
}

int GLAnimateModel::_findRotation_(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);
	for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	assert(0);
	return 0;
}

int GLAnimateModel::_findScaling_(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);
	for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}
	assert(0);
	return 0;
}

void GLAnimateModel::_calcInterpolatedPosition_(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = _findPosition_(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void GLAnimateModel::_calcInterpolatedRotation_(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = _findRotation_(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void GLAnimateModel::_calcInterpolatedScaling_(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = _findScaling_(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}