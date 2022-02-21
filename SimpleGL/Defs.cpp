#include"Defs.h"
#include<string>

// shader attribute name
namespace san
{
	const char* ATTR_POSITION = "aPos";
	const char* ATTR_NORMAL = "aNormal";
	const char* ATTR_TEXTURE = "aTex";
	const char* ATTR_VERT_COLOR = "aColor";
	const char* ATTR_BONE_IDX[3]{ "aBoneIdx0", "aBoneIdx1", "aBoneIdx2" };
	const char* ATTR_BONE_WEIGHT[3]{ "aBoneWeight0", "aBoneWeight1", "aBoneWeight2" };
	const char* ATTR_OPEN_LIGHT = "openLight";
	const char* ATTR_LIGHT_POS = "lightPos";
	const char* ATTR_DIFFUSE_STR = "material.diffuse";
	const char* ATTR_SPECULAR_STR = "material.specular";
	const char* ATTR_AMBIENT_STR = "material.ambient";
	const char* ATTR_SHININESS = "material.shininess";
	const char* ATTR_LIGHT_COLOR = "lightColor";
	const char* ATTR_VIEW_POS = "viewPos";
	const char* ATTR_OBJ_COLOR = "objectColor";
	const char* ATTR_OBJ_COLOR_ALPHA = "colorAlpha";
	const char* ATTR_PROJ_MAT = "projection";
	const char* ATTR_VIEW_MAT = "view";
	const char* ATTR_LOC_TO_GLOB_MAT = "localToGlobal";
	const char* ATTR_CENTER_POS = "centerPos";
	const char* ATTR_RADIUS = "radius";
	const char* ATTR_LENGTH = "length";
	const char* ATTR_LENGTH_X = "lenX";
	const char* ATTR_LENGTH_Y = "lenY";
	const char* ATTR_LENGTH_Z = "lenZ";
	const char* ATTR_TEX_0 = "texture0";
	const char* ATTR_USE_TEX = "useTex";
	const char* ATTR_ANIMATE = "animate";
	const char* ATTR_BONE_ARR = "bones";
}