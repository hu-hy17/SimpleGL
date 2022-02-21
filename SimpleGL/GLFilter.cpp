#include"GLFilter.h"

#include<iostream>

using std::cerr;
using std::endl;

GLFilter::GLFilter()
{
	m_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	this->initializeOpenGLFunctions();

	for (int i = 0; i < m_shader_names.size(); i++)
	{
		std::string v_shader = m_shader_prefix + "filter.vert";
		std::string f_shader = m_shader_prefix + m_shader_names[i] + ".frag";
		_compileAndLinkShader_(m_shaders[i], v_shader, f_shader);
	}
	
	GLfloat vertices[] = {
		// positions    // textures
		  -1.0f,  1.0f,  0.0f, 1.0f,
		  -1.0f, -1.0f,  0.0f, 0.0f,
		   1.0f, -1.0f,  1.0f, 0.0f,

		  -1.0f,  1.0f,  0.0f, 1.0f,
		   1.0f, -1.0f,  1.0f, 0.0f,
		   1.0f,  1.0f,  1.0f, 1.0f
	};

	m_vbo.create();
	m_vbo.bind();
	m_vbo.allocate(vertices, sizeof(vertices));
	m_vbo.release();

	for(int i = 0; i < m_shader_names.size(); i++)
		_setShaderAttr_(m_shaders[i]);
}

void GLFilter::destroy()
{
	m_vbo.destroy();
}

void GLFilter::paint()
{
	m_shaders[(int)m_filter_type].bind();
	{
		m_vbo.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		m_vbo.release();
	}
	m_shaders[(int)m_filter_type].release();
}

void GLFilter::setFilter(FILTER filter_type)
{
	m_filter_type = filter_type;
}

void GLFilter::_setShaderAttr_(QOpenGLShaderProgram& shader)
{
	m_vbo.bind();

	// 坐标
	int attr = -1;
	attr = shader.attributeLocation(san::ATTR_POSITION);
	shader.setAttributeBuffer(attr, GL_FLOAT, 0, 2, sizeof(GLfloat) * 4);
	shader.enableAttributeArray(attr);

	// 纹理
	attr = shader.attributeLocation(san::ATTR_TEXTURE);
	shader.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 2, 2, sizeof(GLfloat) * 4);
	shader.enableAttributeArray(attr);

	m_vbo.release();
}

void GLFilter::_compileAndLinkShader_(
	QOpenGLShaderProgram& shader,
	const std::string& vert_shader,
	const std::string& frag_shader)
{
	shader.removeAllShaders();

	// 着色器编译和链接
	bool success = shader.addShaderFromSourceFile(QOpenGLShader::Vertex, vert_shader.c_str());
	if (!success)
	{
		cerr << m_class_name << "Error: ShaderProgram addShaderFromSourceFile failed!" << endl;
		goto fail;
	}

	success = shader.addShaderFromSourceFile(QOpenGLShader::Fragment, frag_shader.c_str());
	if (!success)
	{
		cerr << m_class_name << "Error: ShaderProgram addShaderFromSourceFile failed!" << endl;
		goto fail;
	}

	success = shader.link();
	if (!success)
	{
		cerr << m_class_name << "Error: shaderProgram link failed!" << endl;
		goto fail;
	}

	return;

fail:
	system("pause");
	exit(-1);
}