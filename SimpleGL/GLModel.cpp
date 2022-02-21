#include"GLModel.h"
#include"Utils.h"

#include<iostream>
#include<fstream>
#include<climits>
#include<algorithm>

#include<QImage>

using std::cerr;
using std::endl;

/************************************************************************/
/* GLMesh																*/
/************************************************************************/
GLMesh::GLMesh(const std::vector<Vertex>& vertices,
	const std::vector<unsigned int>& indices,
	Textures& textures)
{
	_compileAndLinkShader_(m_class_name, ":shader/resources/shader/mesh.vert", ":shader/resources/shader/mesh.frag");
	QOpenGLVertexArrayObject::Binder vaoBind(&m_vao);

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(&vertices[0], vertices.size() * sizeof(Vertex));

	m_ebo.create();
	m_ebo.bind();
	m_ebo.allocate(&indices[0], indices.size() * sizeof(unsigned int));

	// 坐标
	int attr = -1;
	attr = m_shader.attributeLocation(san::ATTR_POSITION);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(Vertex));
	m_shader.enableAttributeArray(attr);

	// 法向量
	attr = m_shader.attributeLocation(san::ATTR_NORMAL);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(Vertex));
	m_shader.enableAttributeArray(attr);

	// 纹理
	attr = m_shader.attributeLocation(san::ATTR_TEXTURE);
	m_shader.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 6, 2, sizeof(Vertex));
	m_shader.enableAttributeArray(attr);

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

void GLMesh::paint()
{
	if (m_textures.size() == 0)
		return;
	m_shader.bind();
	_setLightCond_();

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

void GLMesh::destroy()
{
	__super::destroy();
	for (int i = 0; i < m_textures.size(); i++)
		m_textures[i]->destroy();
}

/************************************************************************/
/* GLModel																*/
/************************************************************************/
GLModel::GLModel(const std::string& model_filename)
{
	const char* extension = strrchr(model_filename.c_str(), '.');
	m_dir = model_filename.substr(0, model_filename.find_last_of('/'));
	if (!extension) {
		cerr << m_class_name << "Please provide a file with a valid extension." << endl;
		goto fail;
	}

	if (AI_FALSE == aiIsExtensionSupported(extension)) {
		cerr << m_class_name << "The specified model file extension is currently "
			"unsupported in Assimp " << endl;
		goto fail;
	}

	m_scene = aiImportFile(model_filename.c_str(),
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices);

	if (!m_scene)
	{
		cerr << m_class_name << "Fail to parse model file!" << endl;
		goto fail;
	}

	return;
fail:
	system("pause");
	exit(-1);
}

void GLModel::init()
{
	_processNode_(m_scene->mRootNode);
}

void GLModel::paint()
{
	for (int i = 0; i < m_globjects.size(); i++)
	{
		m_globjects[i]->paint();
	}
}

void GLModel::setLocToGlob(const QMatrix4x4& mat)
{
	__super::setLocToGlob(mat);
	m_model_mat = mat;
}

bool GLModel::collisionDetect(const QVector3D& pos)
{
	QVector4D hPos(pos, 1.0);
	hPos = m_model_mat.inverted() * hPos;
	QVector3D gPos(hPos.x(), hPos.y(), hPos.z());
	gPos /= hPos.w();

	bool ret = false;
	for (BoundingBox& box : m_bounding_boxes)
	{
		bool contain = true;
		contain = contain && (gPos.x() >= box.mMin.x() && gPos.x() <= box.mMax.x());
		contain = contain && (gPos.y() >= box.mMin.y() && gPos.y() <= box.mMax.y());
		contain = contain && (gPos.z() >= box.mMin.z() && gPos.z() <= box.mMax.z());
		ret = ret || contain;
	}
	return ret;
}

bool GLModel::intersectPoint(const GLRay& ray, GLHitPoint& hitpoint)
{
	// 将光线转换到模型局部坐标系下
	QMatrix4x4 global_to_local = m_model_mat.inverted();
	QVector4D start_pos_m = global_to_local * QVector4D(ray.m_start_pos, 1.0);
	QVector4D light_dir_m = global_to_local * QVector4D(ray.m_dir, 0);

	int m_mesh_num = m_aabb_trees.size();
	Eigen::Vector3f start_pos;
	Eigen::Vector3f direction;
	igl::Hit hit;
	for (int d = 0; d < 3; d++)
	{
		start_pos[d] = start_pos_m[d] / start_pos_m[3];
		direction[d] = light_dir_m[d];
	}

	int mesh_idx = -1;
	float min_dist = 1e9;
	QVector3D normal_m;
	QVector3D point_color;
	
	// 对于模型中的每个mesh求解与光线的交点，得到法向和位置
	for (int i = 1; i < m_mesh_num; i++)
	{
		auto& tree = m_aabb_trees[i];
		bool isHit = tree.intersect_ray(m_mesh_vertices[i], m_mesh_indices[i], start_pos, direction, hit);
		if (isHit && hit.t < min_dist)
		{
			mesh_idx = i;
			min_dist = hit.t;
			normal_m = _getMeshNormal_(i, hit.id);
			Eigen::Vector3f hitpos = start_pos + min_dist * direction;
			point_color = _getMeshColor_(i, hit.id, QVector3D(hitpos.x(), hitpos.y(), hitpos.z()));
		}
	}

	if (mesh_idx == -1)
		return false;

	// 将求解得到的距离，法向转换到全局坐标系
	min_dist /= m_model_mat(3, 3);
	QVector3D normal_g(m_model_mat * QVector4D(normal_m, 0));
	
	if (hitpoint.m_dist < 0 || hitpoint.m_dist > min_dist)
	{
		hitpoint.m_dist = min_dist;
		hitpoint.m_pos = ray.m_start_pos + min_dist * ray.m_dir;
		hitpoint.m_normal = normal_g;
		hitpoint.m_material = m_globjects[mesh_idx]->getMaterial();
		hitpoint.m_inner_hit = false;
		hitpoint.m_color = point_color;
	}

	return true;
}

void GLModel::_processNode_(aiNode* node)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_scene->mMeshes[node->mMeshes[i]];
		_processMesh_(mesh);
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		_processNode_(node->mChildren[i]);
	}
}

void GLModel::_processMesh_(aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Textures textures;

	// 顶点位置,法线,坐标纹理
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

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
	for (Vertex& v : vertices)
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

	// 生成libigl格式的mesh数据
	Eigen::MatrixXf igl_vertices;
	Eigen::MatrixXf igl_normals;
	Eigen::MatrixXf igl_tex_coor;
	Eigen::MatrixXi igl_indices;

	int v_num = vertices.size();
	int i_num = indices.size();
	assert(i_num % 3 == 0 && i_num < 2e9);
	igl_vertices.resize(v_num, 3);
	igl_normals.resize(v_num, 3);
	igl_tex_coor.resize(v_num, 2);
	igl_indices.resize(i_num / 3, 3);
	
	for (int r = 0; r < v_num; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			igl_vertices(r, c) = vertices[r].Position[c];
			igl_normals(r, c) = vertices[r].Normal[c];
		}
		for (int c = 0; c < 2; c++)
			igl_tex_coor(r, c) = vertices[r].TexCoords[c];
	}
	for (int r = 0; r < i_num / 3; r++)
	{
		for (int c = 0; c < 3; c++)
			igl_indices(r, c) = indices[3 * r + c];
	}

	m_mesh_vertices.push_back(igl_vertices);
	m_mesh_normals.push_back(igl_normals);
	m_mesh_indices.push_back(igl_indices);
	m_mesh_tex_coors.push_back(igl_tex_coor);

	// 构建AABB树
	igl::AABB<Eigen::MatrixXf, 3> tree;
	tree.init(igl_vertices, igl_indices);
	m_aabb_trees.push_back(tree);

	// 材质
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = m_scene->mMaterials[mesh->mMaterialIndex];
		auto pImg = _loadMaterialTextures_(textures, material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		_loadMaterialTextures_(textures, material,
			aiTextureType_SPECULAR, "texture_specular");
		m_mesh_tex.push_back(pImg);
	}
	else
	{
		m_mesh_tex.push_back(nullptr);
	}

	m_globjects.push_back(std::make_unique<GLMesh>(vertices, indices, textures));
}

std::shared_ptr<QImage> GLModel::_loadMaterialTextures_(Textures& textures, 
	aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::shared_ptr<QImage> ret = nullptr;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string filename = m_dir + '/' + std::string(str.C_Str());
		QString qfilename = QString::fromUtf8(filename.data());
		QImage org_img(qfilename);
		auto pImg = std::make_shared<QImage>(org_img.mirrored(false, true));
		textures.push_back(std::make_shared<QOpenGLTexture>(*pImg, QOpenGLTexture::GenerateMipMaps));
		if (!textures.back()->isCreated())
		{
			qDebug() << "Failed to load texture";
			textures.pop_back();
		}
		else
		{
			ret = pImg;
			textures.back()->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
			textures.back()->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
			textures.back()->setMinificationFilter(QOpenGLTexture::Linear);
			textures.back()->setMagnificationFilter(QOpenGLTexture::Linear);
		}
	}
	return ret;
}

QVector3D GLModel::_getMeshNormal_(int mesh_id, int ele_id)
{
	QVector3D ret(0, 0, 0);
	for (int d = 0; d < 3; d++)
	{
		int v_id = m_mesh_indices[mesh_id](ele_id, d);
		ret[0] += m_mesh_normals[mesh_id](v_id, 0);
		ret[1] += m_mesh_normals[mesh_id](v_id, 1);
		ret[2] += m_mesh_normals[mesh_id](v_id, 2);
	}
	return ret.normalized();
}

QVector3D GLModel::_getMeshColor_(int mesh_id, int ele_id, QVector3D point)
{
	if (mesh_id >= m_mesh_tex.size() || m_mesh_tex[mesh_id] == nullptr)
	{
		return QVector3D(1, 1, 1);
	}
	auto pImg = m_mesh_tex[mesh_id];

	// 计算交点与三角形三个顶点分别围成的面积
	QVector3D triangle_points[3];
	float area[3];

	for (int r = 0; r < 3; r++)
	{
		int v_id = m_mesh_indices[mesh_id](ele_id, r);
		for (int c = 0; c < 3; c++)
			triangle_points[r][c] = m_mesh_vertices[mesh_id](v_id, c);
	}

	area[0] = fabs(QVector3D::crossProduct(triangle_points[1] - point, triangle_points[2] - point).length());
	area[1] = fabs(QVector3D::crossProduct(triangle_points[0] - point, triangle_points[2] - point).length());
	area[2] = fabs(QVector3D::crossProduct(triangle_points[0] - point, triangle_points[1] - point).length());
	float area_sum = area[0] + area[1] + area[2];
	area[0] /= area_sum;
	area[1] /= area_sum;
	area[2] /= area_sum;

	// 利用面积进行纹理坐标加权
	float texX = 0; 
	float texY = 0;
	for (int r = 0; r < 3; r++)
	{
		int v_id = m_mesh_indices[mesh_id](ele_id, r);
		texX += area[r] * m_mesh_tex_coors[mesh_id](v_id, 0);
		texY += area[r] * m_mesh_tex_coors[mesh_id](v_id, 1);
	}

	texX *= pImg->width();
	texY *= pImg->height();

	int px = std::min((int)texX, pImg->width() - 1);
	int py = std::min((int)texY, pImg->height() - 1);

	assert(px >= 0 && py >= 0);

	QColor color = QColor(pImg->pixel(px, py));
	return QVector3D(color.redF(), color.greenF(), color.blueF());
}