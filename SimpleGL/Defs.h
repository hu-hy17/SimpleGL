#pragma once
#include<string>

// shader attribute name
namespace san
{
	// ������Ϣ
	extern const char* ATTR_POSITION;
	extern const char* ATTR_NORMAL;
	extern const char* ATTR_TEXTURE;
	extern const char* ATTR_VERT_COLOR;

	// ����
	extern const char* ATTR_OPEN_LIGHT;
	extern const char* ATTR_LIGHT_POS;
	extern const char* ATTR_DIFFUSE_STR;
	extern const char* ATTR_SPECULAR_STR;
	extern const char* ATTR_AMBIENT_STR;
	extern const char* ATTR_SHININESS;
	extern const char* ATTR_LIGHT_COLOR;
	extern const char* ATTR_VIEW_POS;
	extern const char* ATTR_OBJ_COLOR;
	extern const char* ATTR_OBJ_COLOR_ALPHA;

	// �任����
	extern const char* ATTR_PROJ_MAT;
	extern const char* ATTR_VIEW_MAT;
	extern const char* ATTR_LOC_TO_GLOB_MAT;

	// ��/Բ��/Բ׶���
	extern const char* ATTR_CENTER_POS;
	extern const char* ATTR_RADIUS;
	extern const char* ATTR_LENGTH;

	// ���������
	extern const char* ATTR_LENGTH_X;
	extern const char* ATTR_LENGTH_Y;
	extern const char* ATTR_LENGTH_Z;

	// �������
	extern const char* ATTR_USE_TEX;
	extern const char* ATTR_TEX_0;

	// �����������
	extern const char* ATTR_BONE_IDX[3];
	extern const char* ATTR_BONE_WEIGHT[3];
	extern const char* ATTR_ANIMATE;
	extern const char* ATTR_BONE_ARR;
}