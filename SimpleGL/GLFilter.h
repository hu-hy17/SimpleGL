#pragma once

#include<QOpenGLShader>
#include<QOpenGLShaderProgram>
#include<QOpenGLVertexArrayObject>
#include<QOpenGLBuffer>
#include<QOpenGLFunctions>
#include<QOpenGLTexture>

#include <string>
#include <vector>

#include"Defs.h"

class GLFilter : protected QOpenGLFunctions
{
public:
	enum class FILTER {
		NONE,
		FILTER_INVERSE,
	};

private:
	const std::string m_class_name = "(GLFilter)";

	FILTER m_filter_type = FILTER::NONE;

	const std::string m_shader_prefix = ":shader/resources/shader/";
	QOpenGLShaderProgram m_shaders[2];
	std::vector<std::string> m_shader_names{
		"filter_none", "filter_inverse"
	};

	QOpenGLBuffer m_vbo;

public:
	GLFilter();
	void destroy();
	void paint();
	void setFilter(FILTER filter_type);
		
private:
	void _compileAndLinkShader_(
		QOpenGLShaderProgram& shader,
		const std::string& vert_shader,
		const std::string& frag_shader);
	void _setShaderAttr_(QOpenGLShaderProgram& shader);
};