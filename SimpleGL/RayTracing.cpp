#include"RayTracing.h"
#include"Utils.h"

/************************************************************************/
/* GLMaterial                                                           */
/************************************************************************/

std::map<std::string, GLMaterial::MATERIAL_TYPE> material_name_map
{
	{"jade", GLMaterial::MATERIAL_TYPE::MAT_JADE},
	{"brass", GLMaterial::MATERIAL_TYPE::MAT_BRASS},
	{"bronze", GLMaterial::MATERIAL_TYPE::MAT_BRONZE},
	{"copper", GLMaterial::MATERIAL_TYPE::MAT_COPPER},
	{"obsidian", GLMaterial::MATERIAL_TYPE::MAT_OBSIDIAN},
	{"pearl", GLMaterial::MATERIAL_TYPE::MAT_PEARL},
	{"silver", GLMaterial::MATERIAL_TYPE::MAT_SILVER},
	{"white_plastic", GLMaterial::MATERIAL_TYPE::MAT_WHITE_PLASTIC},
	{"default", GLMaterial::MATERIAL_TYPE::MAT_DEFAULT}
};

GLMaterial::GLMaterial(MATERIAL_TYPE type)
{
	switch (type)
	{
	case MAT_BRASS:
		m_ka = QVector3D(0.329412, 0.223529, 0.027451);
		m_kd = QVector3D(0.780392, 0.568627, 0.113725);
		m_ks = QVector3D(0.992157, 0.941176, 0.807843);
		m_shininess = 0.21794872;
		break;
	case MAT_JADE:
		m_ka = QVector3D(0.135,	0.2225,	0.1575);
		m_kd = QVector3D(0.54, 0.89, 0.63);
		m_ks = QVector3D(0.316228, 0.316228, 0.316228);
		m_shininess = 0.1;
		break;
	default:
		m_ka = QVector3D(0.2, 0.2, 0.2);
		m_kd = QVector3D(0.6, 0.6, 0.6);
		m_ks = QVector3D(0.3, 0.3, 0.3);
		m_shininess = 0.3;
		break;
	}
	isRough = true;
}

GLMaterial::GLMaterial(int type)
{
	MATERIAL_TYPE mtype = MATERIAL_TYPE(type);
	new(this) GLMaterial(mtype);
}

GLMaterial::GLMaterial(const std::string& mat_name)
{
	auto it = material_name_map.find(mat_name);
	if (it != material_name_map.end())
		new(this) GLMaterial(it->second);
	else
		new(this) GLMaterial(MATERIAL_TYPE::MAT_DEFAULT);
}

void GLMaterial::setSmooth(float Il)
{
	isRough = false;
	isRefractive = false;
	m_Il = Il;
	m_F0 = QVector3D(1, 1, 1);
}

void GLMaterial::setSmooth(float Il, float ior, QVector3D F0)
{
	isRough = false;
	isRefractive = true;
	m_Il = Il;
	m_ior = ior;
	m_F0 = F0;
}

std::shared_ptr<GLMaterial> GLMaterial::loadFromJson(Json::Value& val)
{
	if (val == Json::nullValue)
	{
		return std::make_shared<GLMaterial>("default");
	}
	auto pMaterial = std::make_shared<GLMaterial>(val["name"].asString());
	if (val["ior"] != Json::nullValue)
	{
		pMaterial->setSmooth(val["Il"].asFloat(), val["ior"].asFloat(), getVec3FromJson(val["F0"]));
	}
	else if(val["Il"] != Json::nullValue)
	{
		pMaterial->setSmooth(val["Il"].asFloat());
	}
	return pMaterial;
}


/************************************************************************/
/* GLRay                                                                */
/************************************************************************/

GLRay::GLRay(QVector3D start_pos, QVector3D dir)
	: m_start_pos(start_pos),
	m_dir(dir.normalized()),
	m_cur_hit_times(0)
{ }

GLRay::GLRay(QVector3D start_pos, QVector3D dir, int hit_times)
	: m_start_pos(start_pos),
	m_dir(dir.normalized()),
	m_cur_hit_times(hit_times)
{ }