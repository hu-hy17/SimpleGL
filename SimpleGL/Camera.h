#pragma once
#include <QVector3D>
#include <QMatrix4x4>
#include <QKeyEvent>

#include "RayTracing.h"

#define M_PI 3.1415926

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -M_PI / 2;
const float PITCH = 0.0f;
const float SPEED = 30.0f;
const float SENSITIVITY = 0.0015f;
const float ZOOM = 45.0f;

class Camera {
public:
	Camera(QVector3D position = QVector3D(0.0f, 0.0f, -1.0f), 
		QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
		float yaw = YAW, float pitch = PITCH);
	~Camera();

	QMatrix4x4 getViewMatrix();
	QMatrix4x4 getProjectionMatrix();
	GLRay getRay(int px, int py);

	void processMouseMovement(float xoffset, float yoffset, bool constraintPitch = true);
	void processMouseScroll(float yoffset);
	void processInput(float dt);

	void setScreenSize(int width, int height);

public:
	QVector3D position;
	QVector3D worldUp;
	QVector3D front;

	QVector3D up;
	QVector3D right;

	// screen size
	int screen_width;
	int screen_height;

	//Eular Angles
	float picth;
	float yaw;

	//Camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	//Keyboard multi-touch
	bool keys[1024];

private:
	void updateCameraVectors();
	void processKeyboard(Camera_Movement direction, float deltaTime);
};
