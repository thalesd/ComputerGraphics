#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "main.h"

const GLint WIDTH = 800, HEIGHT = 600;
const int MY_GLFW_MAJOR_VERSION = 3;
const int MY_GLFW_MINOR_VERSION = 3;
const float toRadians = glm::pi<float>() / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

bool moveDirection = true;
float triOffset = 0.0f;
float triMaxOffset = 0.6f;
float triIncrement = 0.0005f;

bool rotationDirection = true;
float rotationOffset = 0.0f;
float rotationMaxOffset = 360.0f * toRadians;
float rotationIncrement = 0.05f * toRadians;

bool scaleDirection = true;
float scaleOffset = 1.0f;
float scaleMaxOffset = 1.2f;
float scaleMinOffset = 0.2f;
float scaleIncrement = 0.001f;

//Vertex Shader
static const char* vShader = "						\n\
#version 330										\n\
													\n\
layout (location = 0) in vec3 pos;					\n\
													\n\
uniform mat4 model;									\n\
													\n\
void main(){										\n\
	gl_Position = model * vec4(pos.x, pos.y, pos.z, 1.0);	\n\
}";

//Fragment Shader
static const char* fShader = "						\n\
#version 330										\n\
													\n\
out vec4 color;										\n\
													\n\
void main(){										\n\
	color = vec4(1.0, 0.55, 0.0, 1.0);				\n\
}";

void CreateTriangle() {
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	#pragma region inside VAO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint MY_GLINDEX = 0;
	GLint MY_GLSIZE = 3;

	glVertexAttribPointer(MY_GLINDEX, MY_GLSIZE, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(MY_GLINDEX);
	#pragma endregion

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AddShader(GLuint program, const char* shaderCode, GLenum shaderType) {
	GLuint addingShader = glCreateShader(shaderType);

	const GLchar* inputShaderCode[1];
	inputShaderCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(addingShader, 1, inputShaderCode, codeLength);
	glCompileShader(addingShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(addingShader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(addingShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;//change to throw
	}

	glAttachShader(program, addingShader);
}

void CompileShaders() {
	shader = glCreateProgram();

	if (!shader) {
		printf("Error creating shader program\n");
		return;//change to throw 
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;//change to throw
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;//change to throw
	}

	uniformModel = glGetUniformLocation(shader, "model");
}

int main() {
	if (!glfwInit()) {
		printf("GLFW initialization failed");

		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MY_GLFW_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MY_GLFW_MINOR_VERSION);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);

	if (!mainWindow) {
		printf("GLFW Window creation failed");

		glfwTerminate();
		return 1;
	}

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	glfwMakeContextCurrent(mainWindow);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("GLEW initialization failed");

		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glViewport(0, 0, bufferWidth, bufferHeight);
	CreateTriangle();
	CompileShaders();

	while (!glfwWindowShouldClose(mainWindow)) {
		glfwPollEvents();

		if (moveDirection) {
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset) {
			moveDirection = !moveDirection;
		}

		if (rotationDirection) {
			rotationOffset += rotationIncrement;
		}
		else {
			rotationOffset -= rotationIncrement;
		}

		if (abs(rotationOffset) >= rotationMaxOffset) {
			rotationDirection = !rotationDirection;
		}

		if (scaleDirection) {
			scaleOffset += scaleIncrement;
		}
		else {
			scaleOffset -= scaleIncrement;
		}

		if (scaleOffset >= scaleMaxOffset || scaleOffset <= scaleMinOffset) {
			scaleDirection = !scaleDirection;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
		model = glm::scale(model, scaleOffset * glm::vec3(1.0f));
		model = glm::rotate(model, rotationOffset, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}