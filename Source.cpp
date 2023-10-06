#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>

#define PI 3.14159265

void LoadModel(std::vector<float>& vertices, std::vector<short>& indices, unsigned int& VBO, unsigned int& VBO2, unsigned int& VAO);
void OutlineModel(std::vector<float>& vertices, std::vector<short>& indices);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(3.0f, 2.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// boolean controls view perspective
bool defaultView = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Chess - Kara Allison", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
	Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	std::vector<float> vertices {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	std::vector<float> bishopVertices{
		// Bishop - half outline coordinates starting top to bottom
		// Vertex Positions    // Normals   // Texture Coordinates
		0.000f, 0.711f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 1.00f,
		0.017f, 0.707f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.98f,
		0.025f, 0.701f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.96f,
		0.031f, 0.685f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.94f,
		0.029f, 0.675f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.92f,
		0.015f, 0.657f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.90f,
		0.019f, 0.655f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.88f,
		0.021f, 0.653f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.86f,
		
		0.020f, 0.648f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.84f,
		0.033f, 0.632f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.82f,
		0.051f, 0.607f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.80f,
		0.063f, 0.582f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.78f,
		0.074f, 0.547f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.76f,
		0.076f, 0.520f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.74f,
		0.069f, 0.492f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.72f,
		0.060f, 0.475f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.70f,
		
		0.053f, 0.467f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.68f,
		0.062f, 0.464f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.66f,
		0.065f, 0.457f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.64f,
		0.059f, 0.450f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.62f,
		0.047f, 0.441f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.60f,
		0.057f, 0.434f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.58f,
		0.068f, 0.428f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.56f,
		0.070f, 0.420f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.54f,
		
		0.084f, 0.414f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.52f,
		0.090f, 0.408f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.50f,
		0.092f, 0.399f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.48f,
		0.087f, 0.390f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.46f,
		0.073f, 0.386f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.44f,
		0.058f, 0.377f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.42f,
		0.045f, 0.369f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.40f,
		0.046f, 0.330f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.38f,
		
		0.055f, 0.281f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.36f,
		0.072f, 0.239f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.34f,
		0.100f, 0.202f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.32f,
		0.106f, 0.189f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.30f,
		0.101f, 0.183f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.28f,
		0.093f, 0.179f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.26f,
		0.109f, 0.154f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.24f,
		0.133f, 0.128f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.22f,
		
		0.142f, 0.103f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.20f,
		0.139f, 0.085f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.18f,
		0.125f, 0.064f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.16f,
		0.140f, 0.056f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.14f,
		0.149f, 0.048f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.12f,
		0.149f, 0.005f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.10f,
		0.145f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.08f,
		0.000f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.06f
	};

	// Create empty vector for indices
	std::vector<short> bishopIndices;
	// Generate model from outline
	OutlineModel(bishopVertices, bishopIndices);

	std::vector<float> knightVertices{
		// knight - half outline coordinates starting top to bottom
		// Vertex Positions    // Normals   // Texture Coordinates
		0.000f, 0.189f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.40f,
		0.106f, 0.189f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.30f,
		0.101f, 0.183f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.28f,
		0.093f, 0.179f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.26f,
		0.109f, 0.154f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.24f,
		0.133f, 0.128f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.22f,

		0.142f, 0.103f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.20f,
		0.139f, 0.085f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.18f,
		0.125f, 0.064f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.16f,
		0.140f, 0.056f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.14f,
		0.149f, 0.048f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.12f,
		0.149f, 0.005f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.10f,
		0.145f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.08f,
		0.000f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.06f
	};

	// Create empty vector for indices
	std::vector<short> knightIndices;
	// Generate model from outline
	OutlineModel(knightVertices, knightIndices);

	std::vector<float> knightHeadVertices{
		// Vertex Positions       // Normals          // Texture
		0.000f, 0.189f, -0.105f,  0.0f, 0.0f, -1.0f,  0.00f, 0.00f,
		0.100f, 0.189f,  0.000f,  1.0f, 0.0f, 0.0f,  0.50f, 0.00f,
		0.000f, 0.189f,  0.105f,  0.0f, 0.0f, 1.0f,  1.00f, 0.00f,
		0.000f, 0.224f, -0.101f,  0.0f, 0.0f, -1.0f,  0.00f, 0.03f,
		0.100f, 0.224f, -0.005f,  1.0f, 0.0f, 0.0f,  0.50f, 0.03f,
		0.000f, 0.224f,  0.091f,  0.0f, 0.0f, 1.0f,  1.00f, 0.03f,

		0.000f, 0.248f, -0.106f,  0.0f, 0.0f, -1.0f,  0.00f, 0.06f,
		0.100f, 0.248f, -0.0015f,  1.0f, 0.0f, 0.0f,  0.50f, 0.06f,
		0.000f, 0.248f,  0.103f,  0.0f, 0.0f, 1.0f,  1.00f, 0.06f,
		0.000f, 0.268f, -0.107f,  0.0f, 0.0f, -1.0f,  0.00f, 0.09f,
		0.100f, 0.268f, -0.003f,  1.0f, 0.0f, 0.0f,  0.50f, 0.09f,
		0.000f, 0.268f,  0.101f,  0.0f, 0.0f, 1.0f,  1.00f, 0.09f,

		0.000f, 0.280f, -0.103f,  0.0f, 0.0f, -1.0f,  0.00f, 0.12f,
		0.100f, 0.280f,  0.0025f,  1.0f, 0.0f, 0.0f,  0.50f, 0.12f,
		0.000f, 0.280f,  0.108f,  0.0f, 0.0f, 1.0f,  1.00f, 0.12f,
		0.000f, 0.295f, -0.099f,  0.0f, 0.0f, -1.0f,  0.00f, 0.15f,
		0.100f, 0.295f,  0.005f,  1.0f, 0.0f, 0.0f,  0.50f, 0.15f,
		0.000f, 0.295f,  0.109f,  0.0f, 0.0f, 1.0f,  1.00f, 0.15f,

		0.000f, 0.307f, -0.091f,  0.0f, 0.0f, -1.0f,  0.00f, 0.18f,
		0.100f, 0.307f,  0.015f,  1.0f, 0.0f, 0.0f,  0.50f, 0.18f,
		0.000f, 0.307f,  0.121f,  0.0f, 0.0f, 1.0f,  1.00f, 0.18f,
		0.000f, 0.321f, -0.080f,  0.0f, 0.0f, -1.0f,  0.00f, 0.21f,
		0.100f, 0.321f,  0.0225f,  1.0f, 0.0f, 0.0f,  0.50f, 0.21f,
		0.000f, 0.321f,  0.125f,  0.0f, 0.0f, 1.0f,  1.00f, 0.21f,

		0.000f, 0.333f, -0.070f,  0.0f, 0.0f, -1.0f,  0.00f, 0.24f,
		0.100f, 0.333f,  0.034f,  1.0f, 0.0f, 0.0f,  0.50f, 0.24f,
		0.000f, 0.333f,  0.138f,  0.0f, 0.0f, 1.0f,  1.00f, 0.24f,
		0.000f, 0.350f, -0.043f,  0.0f, 0.0f, -1.0f,  0.00f, 0.27f,
		0.100f, 0.350f,  0.0495f,  1.0f, 0.0f, 0.0f,  0.50f, 0.27f,
		0.000f, 0.350f,  0.142f,  0.0f, 0.0f, 1.0f,  1.00f, 0.27f,

		0.000f, 0.363f, -0.024f,  0.0f, 1.0f, -1.0f,  0.00f, 0.30f,
		0.095f, 0.363f,  0.0665f,  1.0f, 1.0f, 0.0f,  0.50f, 0.30f,
		0.000f, 0.363f,  0.157f,  0.0f, 1.0f, 1.0f,  1.00f, 0.30f,
		0.000f, 0.379f,  0.008f,  0.0f, 1.0f, -1.0f,  0.00f, 0.33f,
		0.090f, 0.379f,  0.0835f,  1.0f, 1.0f, 0.0f,  0.50f, 0.33f,
		0.000f, 0.379f,  0.159f,  0.0f, 1.0f, 1.0f,  1.00f, 0.33f,

		0.000f, 0.400f,  0.038f,  0.0f, 1.0f, -1.0f,  0.00f, 0.36f,
		0.085f, 0.400f,  0.106f,  1.0f, 1.0f, 0.0f,  0.50f, 0.36f,
		0.000f, 0.400f,  0.174f,  0.0f, 1.0f, 1.0f,  1.00f, 0.36f,
		0.000f, 0.417f,  0.047f,  0.0f, 1.0f, -1.0f,  0.00f, 0.39f,
		0.080f, 0.417f,  0.111f,  1.0f, 1.0f, 0.0f,  0.50f, 0.39f,
		0.000f, 0.417f,  0.175f,  0.0f, 1.0f, 1.0f,  1.00f, 0.39f,

		0.000f, 0.438f, -0.144f,  0.0f, 1.0f, -1.0f,  0.00f, 0.42f,
		0.085f, 0.438f,  0.019f,  1.0f, 1.0f, 0.0f,  0.50f, 0.42f,
		0.000f, 0.438f,  0.182f,  0.0f, 1.0f, 1.0f,  1.00f, 0.42f,
		0.000f, 0.458f, -0.131f,  0.0f, 1.0f, -1.0f,  0.00f, 0.45f,
		0.090f, 0.458f,  0.0225f,  1.0f, 1.0f, 0.0f,  0.50f, 0.45f,
		0.000f, 0.458f,  0.176f,  0.0f, 1.0f, 1.0f,  1.00f, 0.45f,

		0.000f, 0.477f, -0.099f,  0.0f, 1.0f, -1.0f,  0.00f, 0.48f,
		0.095f, 0.477f,  0.039f,  1.0f, 1.0f, 0.0f,  0.50f, 0.48f,
		0.000f, 0.477f,  0.177f,  0.0f, 1.0f, 1.0f,  1.00f, 0.48f,
		0.000f, 0.495f, -0.076f,  0.0f, 1.0f, -1.0f,  0.00f, 0.51f,
		0.100f, 0.495f,  0.0455f,  1.0f, 1.0f, 0.0f,  0.50f, 0.51f,
		0.000f, 0.495f,  0.167f,  0.0f, 1.0f, 1.0f,  1.00f, 0.51f,

		0.000f, 0.517f, -0.060f,  0.0f, 0.0f, -1.0f,  0.00f, 0.54f,
		0.100f, 0.517f,  0.0505f,  1.0f, 0.0f, 0.0f,  0.50f, 0.54f,
		0.000f, 0.517f,  0.161f,  0.0f, 0.0f, 1.0f,  1.00f, 0.54f,
		0.000f, 0.531f, -0.047f,  0.0f, 0.0f, -1.0f,  0.00f, 0.57f,
		0.100f, 0.531f,  0.050f,  1.0f, 0.0f, 0.0f,  0.50f, 0.57f,
		0.000f, 0.531f,  0.147f,  0.0f, 0.0f, 1.0f,  1.00f, 0.57f,

		0.000f, 0.549f, -0.015f,  0.0f, 1.0f, -1.0f,  0.00f, 0.60f,
		0.100f, 0.549f,  0.059f,  1.0f, 1.0f, 0.0f,  0.50f, 0.60f,
		0.000f, 0.549f,  0.133f,  0.0f, 1.0f, 1.0f,  1.00f, 0.60f,
		0.000f, 0.554f,  0.001f,  0.0f, 1.0f, -1.0f,  0.00f, 0.63f,
		0.080f, 0.554f,  0.059f,  1.0f, 1.0f, 0.0f,  0.50f, 0.63f,
		0.000f, 0.554f,  0.117f,  0.0f, 1.0f, 1.0f,  1.00f, 0.63f,

		0.000f, 0.566f, -0.004f,  0.0f, 1.0f, -1.0f,  0.00f, 0.66f,
		0.075f, 0.566f,  0.048f,  1.0f, 1.0f, 0.0f,  0.50f, 0.66f,
		0.000f, 0.566f,  0.100f,  0.0f, 1.0f, 1.0f,  1.00f, 0.66f,
		0.000f, 0.576f, -0.008f,  0.0f, 1.0f, -1.0f,  0.00f, 0.69f,
		0.070f, 0.576f,  0.0175f,  1.0f, 1.0f, 0.0f,  0.50f, 0.69f,
		0.000f, 0.576f,  0.043f,  0.0f, 1.0f, 1.0f,  1.00f, 0.69f,

		0.000f, 0.589f, -0.012f,  0.0f, 1.0f, -1.0f,  0.00f, 0.72f,
		0.065f, 0.589f,  0.0105f,  1.0f, 1.0f, 0.0f,  0.50f, 0.72f,
		0.000f, 0.589f,  0.033f,  0.0f, 1.0f, 1.0f,  1.00f, 0.72f,
		0.000f, 0.595f, -0.013f,  0.0f, 1.0f, -1.0f,  0.00f, 0.75f,
		0.060f, 0.595f,  0.0025f,  1.0f, 1.0f, 0.0f,  0.50f, 0.75f,
		0.000f, 0.595f,  0.018f,  0.0f, 1.0f, 1.0f,  1.00f, 0.75f,

		0.000f, 0.607f, -0.014f,  0.0f, 1.0f, -1.0f,  0.00f, 0.78f,
		0.055f, 0.607f, -0.005f,  1.0f, 1.0f, 0.0f,  0.50f, 0.78f,
		0.000f, 0.607f,  0.004f,  0.0f, 1.0f, 1.0f,  1.00f, 0.78f,
		0.000f, 0.622f, -0.009f,  0.0f, 1.0f, -1.0f,  0.00f, 0.81f,
		0.050f, 0.622f, -0.009f,  1.0f, 1.0f, 0.0f,  0.50f, 0.81f,
		0.000f, 0.622f, -0.009f,  0.0f, 1.0f, 1.0f,  1.00f, 0.81f
	};

	// Capture size of original vector
	int knightHeadSizeInitial = knightHeadVertices.size();
	// Start from back of vector and mirror across Z axis
		for (int i = knightHeadSizeInitial -1; i > -1; i-=8) {
			knightHeadVertices.push_back(knightHeadVertices.at(i-7) * -1);
			
			knightHeadVertices.push_back(knightHeadVertices.at(i-6));
			
			knightHeadVertices.push_back(knightHeadVertices.at(i-5));
			
			knightHeadVertices.push_back(knightHeadVertices.at(i-4) * -1);
			
			knightHeadVertices.push_back(knightHeadVertices.at(i-3));
			
			knightHeadVertices.push_back(knightHeadVertices.at(i-2));
			
			knightHeadVertices.push_back(knightHeadVertices.at(i-1));
			
			knightHeadVertices.push_back(knightHeadVertices.at(i));
		}
	
	// Capture size of final vector
	int knightHeadSizeFinal = knightHeadVertices.size();
	// Create empty vector for indices
	std::vector<short> knightHeadIndices;
	int start = 0;
	// Adds indices to vector to form triangles
	while (true) {
		knightHeadIndices.push_back(start);
		start++;
		knightHeadIndices.push_back(start);
		start += 2;
		knightHeadIndices.push_back(start);
		start -= 2;
		knightHeadIndices.push_back(start);
		start++;
		knightHeadIndices.push_back(start);
		start += 2;
		knightHeadIndices.push_back(start);
		start -= 3;
		knightHeadIndices.push_back(start);
		start += 2;
		knightHeadIndices.push_back(start);
		start++;
		knightHeadIndices.push_back(start);
		start -= 2;
		knightHeadIndices.push_back(start);
		start += 2;
		knightHeadIndices.push_back(start);
		start++;
		knightHeadIndices.push_back(start);
		if (start == (knightHeadSizeFinal / 8) - 1) {
			break;
		}
		start -= 2;
	}
	std::vector<float> rookVertices{
		// Rook - half outline coordinates starting top to bottom
		// Vertex Positions    // Normals   // Texture Coordinates
		0.000f, 0.489f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 1.00f,
		0.109f, 0.489f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.98f,
		0.109f, 0.430f, 0.0f,  1.0f,  0.0f, 0.0f,   0.00f, 0.96f,
		0.104f, 0.425f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.94f,
		0.097f, 0.422f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.92f,
		0.104f, 0.418f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.90f,
		0.109f, 0.412f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.88f,
		0.108f, 0.399f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.86f,

		0.098f, 0.391f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.84f,
		0.089f, 0.385f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.82f,

		0.068f, 0.374f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.80f,
		0.069f, 0.329f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.78f,
		0.077f, 0.267f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.76f,
		0.089f, 0.224f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.74f,

		0.106f, 0.189f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.40f,
		0.101f, 0.183f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.38f,
		0.093f, 0.179f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.36f,
		0.109f, 0.154f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.34f,
		0.133f, 0.128f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.32f,

		0.142f, 0.103f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.30f,
		0.139f, 0.085f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.28f,
		0.125f, 0.064f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.26f,
		0.140f, 0.056f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.24f,
		0.149f, 0.048f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.22f,
		0.149f, 0.005f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.20f,
		0.145f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.18f,
		0.000f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.00f
	};

	// Create empty vector for indices
	std::vector<short> rookIndices;
	// Generate model from outline
	OutlineModel(rookVertices, rookIndices);

	std::vector<float> rookTopVertices{
		 0.075f, 0.000f, -0.0417f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
		 0.101f, 0.000f, -0.0417f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
		 0.101f, 0.000f,  0.0417f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 0.101f, 0.000f,  0.0417f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 0.075f, 0.000f,  0.0417f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		 0.075f, 0.000f, -0.0417f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

		 0.075f, 0.055f, -0.0417f,   0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		 0.101f, 0.055f, -0.0417f,   0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		 0.101f, 0.055f,  0.0417f,   0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
		 0.101f, 0.055f,  0.0417f,   0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
		 0.075f, 0.055f,  0.0417f,   0.0f,  1.0f, 0.0f,  0.0f, 0.0f,
		 0.075f, 0.055f, -0.0417f,   0.0f,  1.0f, 0.0f,  0.0f, 1.0f,

		 0.075f, 0.000f,  0.0417f,   0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
		 0.101f, 0.000f,  0.0417f,   0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
		 0.101f, 0.055f,  0.0417f,   0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		 0.101f, 0.055f,  0.0417f,   0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		 0.075f, 0.055f,  0.0417f,   0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
		 0.075f, 0.000f,  0.0417f,   0.0f,  0.0f, 1.0f,  0.0f, 1.0f,

		 0.075f, 0.000f, -0.0417f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		 0.101f, 0.000f, -0.0417f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.101f, 0.055f, -0.0417f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.101f, 0.055f, -0.0417f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.075f, 0.055f, -0.0417f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.075f, 0.000f, -0.0417f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

		 0.101f, 0.000f, -0.0417f,   1.0f,  0.0f, 0.0f,  0.0f, 1.0f,
		 0.101f, 0.000f,  0.0417f,   1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
		 0.101f, 0.055f,  0.0417f,   1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
		 0.101f, 0.055f,  0.0417f,   1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
		 0.101f, 0.055f, -0.0417f,   1.0f,  0.0f, 0.0f,  0.0f, 0.0f,
		 0.101f, 0.000f, -0.0417f,   1.0f,  0.0f, 0.0f,  0.0f, 1.0f,

		 0.075f, 0.000f, -0.0417f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.075f, 0.000f,  0.0417f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.075f, 0.055f,  0.0417f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.075f, 0.055f,  0.0417f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.075f, 0.055f, -0.0417f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.075f, 0.000f, -0.0417f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f
	};

	// Capture size of original vector
	int rookTopSizeInitial = rookTopVertices.size();
	// Replicate rook top rectangle 4 times around origin
		for (int i = 0; i < rookTopSizeInitial; i++) {
			rookTopVertices.push_back(rookTopVertices.at(i) * -1);
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i) * -1);
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
		}
		for (int i = 0; i < rookTopSizeInitial; i++) {
			// push the value of the rotated x coordinate
			rookTopVertices.push_back(rookTopVertices.at(i + 2));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i - 2));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i + 2));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i - 2));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
		}
		for (int i = 0; i < rookTopSizeInitial; i++) {
			// push the value of the rotated x coordinate
			rookTopVertices.push_back(rookTopVertices.at(i + 2));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i - 2) * -1);
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i + 2));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i - 2) * -1);
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
			i++;
			rookTopVertices.push_back(rookTopVertices.at(i));
		}
	
	// vector for indices
	std::vector<short> rookTopIndices;
	for (int i = 0; i < 144; i++) {
		rookTopIndices.push_back(i);
	}

	std::vector<float> queenVertices{
		// Queen - half outline coordinates starting top to bottom
		// Vertex Positions    // Normals   // Texture Coordinates
		0.000f, 0.793f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 1.00f,
		0.015f, 0.787f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.98f,
		0.025f, 0.776f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.96f,
		0.028f, 0.764f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.94f,
		0.026f, 0.750f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.92f,
		0.017f, 0.736f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.90f,
		0.021f, 0.734f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.88f,
		0.021f, 0.730f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.86f,

		0.036f, 0.728f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.84f,
		0.049f, 0.723f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.82f,
		0.068f, 0.711f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.80f,
		0.079f, 0.697f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.78f,
		0.089f, 0.683f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.76f,
		0.095f, 0.684f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.74f,
		0.105f, 0.693f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.72f,
		0.114f, 0.699f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.70f,

		0.117f, 0.692f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.68f,
		0.104f, 0.674f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.66f,
		0.076f, 0.628f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.64f,
		0.057f, 0.587f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.62f,
		0.053f, 0.562f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.60f,
		0.062f, 0.559f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.58f,
		0.065f, 0.553f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.56f,
		0.060f, 0.544f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.54f,

		0.053f, 0.543f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.52f,
		0.048f, 0.537f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.50f,
		0.051f, 0.532f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.48f,
		0.063f, 0.527f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.46f,
		0.070f, 0.521f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.44f,
		0.072f, 0.513f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.42f,
		0.086f, 0.510f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.40f,
		0.094f, 0.500f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.38f,

		0.093f, 0.488f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.36f,
		0.076f, 0.482f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.34f,
		0.054f, 0.471f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.32f,
		0.044f, 0.466f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.30f,
		0.044f, 0.412f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.28f,
		0.048f, 0.351f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.26f,
		0.063f, 0.279f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.24f,
		0.084f, 0.226f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.22f,

		0.106f, 0.189f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.20f,
		0.101f, 0.183f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.18f,
		0.093f, 0.179f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.16f,
		0.109f, 0.154f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.14f,
		0.133f, 0.128f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.12f,

		0.142f, 0.103f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.10f,
		0.139f, 0.085f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.08f,
		0.125f, 0.064f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.06f,
		0.140f, 0.056f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.05f,
		0.149f, 0.048f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.04f,
		0.149f, 0.005f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.03f,
		0.145f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.02f,
		0.000f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.00f
	};

	// Create empty vector for indices
	std::vector<short> queenIndices;
	// Generate model from outline
	OutlineModel(queenVertices, queenIndices);

	std::vector<float> kingVertices{
		// King - half outline coordinates starting top to bottom
		// Vertex Positions    // Normals   // Texture Coordinates
		0.000f, 0.743f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 1.00f,
		0.033f, 0.743f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.98f,
		0.040f, 0.739f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.96f,
		0.047f, 0.736f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.94f,
		0.048f, 0.726f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.92f,
		0.066f, 0.717f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.90f,
		0.089f, 0.697f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.88f,
		0.093f, 0.675f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.86f,

		0.084f, 0.643f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.84f,
		0.067f, 0.606f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.82f,
		
		0.053f, 0.562f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.80f,
		0.062f, 0.559f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.78f,
		0.065f, 0.553f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.76f,
		0.060f, 0.544f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.74f,

		0.053f, 0.543f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.72f,
		0.048f, 0.537f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.70f,
		0.051f, 0.532f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.68f,
		0.063f, 0.527f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.66f,
		0.070f, 0.521f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.64f,
		0.072f, 0.513f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.62f,
		0.086f, 0.510f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.60f,
		0.094f, 0.500f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.58f,

		0.093f, 0.488f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.56f,
		0.076f, 0.482f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.54f,
		0.054f, 0.471f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.52f,
		0.044f, 0.466f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.50f,
		0.044f, 0.412f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.48f,
		0.048f, 0.351f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.46f,
		0.063f, 0.279f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.44f,
		0.084f, 0.226f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.42f,

		0.106f, 0.189f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.40f,
		0.101f, 0.183f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.38f,
		0.093f, 0.179f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.36f,
		0.109f, 0.154f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.34f,
		0.133f, 0.128f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.32f,

		0.142f, 0.103f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.30f,
		0.139f, 0.085f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.28f,
		0.125f, 0.064f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.26f,
		0.140f, 0.056f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.24f,
		0.149f, 0.048f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.22f,
		0.149f, 0.005f, 0.0f,  1.0f,  1.0f, 0.0f,   0.00f, 0.20f,
		0.145f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.18f,
		0.000f, 0.000f, 0.0f,  1.0f, -1.0f, 0.0f,   0.00f, 0.00f
	};

	// Create empty vector for indices
	std::vector<short> kingIndices;
	// Generate model from outline
	OutlineModel(kingVertices, kingIndices);

	std::vector<float> kingCrossVertices{
		-0.016f, 0.000f, -0.016f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
		 0.016f, 0.000f, -0.016f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
		 0.016f, 0.000f,  0.016f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 0.016f, 0.000f,  0.016f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		-0.016f, 0.000f,  0.016f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		-0.016f, 0.000f, -0.016f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

		-0.016f, 0.127f, -0.016f,   0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		 0.016f, 0.127f, -0.016f,   0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		 0.016f, 0.127f,  0.016f,   0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
		 0.016f, 0.127f,  0.016f,   0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
		-0.016f, 0.127f,  0.016f,   0.0f,  1.0f, 0.0f,  0.0f, 0.0f,
		-0.016f, 0.127f, -0.016f,   0.0f,  1.0f, 0.0f,  0.0f, 1.0f,

		-0.016f, 0.000f,  0.016f,   0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
		 0.016f, 0.000f,  0.016f,   0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
		 0.016f, 0.127f,  0.016f,   0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		 0.016f, 0.127f,  0.016f,   0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		-0.016f, 0.127f,  0.016f,   0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
		-0.016f, 0.000f,  0.016f,   0.0f,  0.0f, 1.0f,  0.0f, 1.0f,

		-0.016f, 0.000f, -0.016f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		 0.016f, 0.000f, -0.016f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.016f, 0.127f, -0.016f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.016f, 0.127f, -0.016f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		-0.016f, 0.127f, -0.016f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		-0.016f, 0.000f, -0.016f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

		 0.016f, 0.000f, -0.016f,   1.0f,  0.0f, 0.0f,  0.0f, 1.0f,
		 0.016f, 0.000f,  0.016f,   1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
		 0.016f, 0.127f,  0.016f,   1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
		 0.016f, 0.127f,  0.016f,   1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
		 0.016f, 0.127f, -0.016f,   1.0f,  0.0f, 0.0f,  0.0f, 0.0f,
		 0.016f, 0.000f, -0.016f,   1.0f,  0.0f, 0.0f,  0.0f, 1.0f,

		-0.016f, 0.000f, -0.016f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.016f, 0.000f,  0.016f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.016f, 0.127f,  0.016f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.016f, 0.127f,  0.016f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.016f, 0.127f, -0.016f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.016f, 0.000f, -0.016f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

		-0.0635f, 0.0475f, -0.014f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
		 0.0635f, 0.0475f, -0.014f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
		 0.0635f, 0.0475f,  0.014f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 0.0635f, 0.0475f,  0.014f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		-0.0635f, 0.0475f,  0.014f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		-0.0635f, 0.0475f, -0.014f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

		-0.0635f, 0.0795f, -0.014f,   0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		 0.0635f, 0.0795f, -0.014f,   0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		 0.0635f, 0.0795f,  0.014f,   0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
		 0.0635f, 0.0795f,  0.014f,   0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
		-0.0635f, 0.0795f,  0.014f,   0.0f,  1.0f, 0.0f,  0.0f, 0.0f,
		-0.0635f, 0.0795f, -0.014f,   0.0f,  1.0f, 0.0f,  0.0f, 1.0f,

		-0.0635f, 0.0475f,  0.014f,   0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
		 0.0635f, 0.0475f,  0.014f,   0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
		 0.0635f, 0.0795f,  0.014f,   0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		 0.0635f, 0.0795f,  0.014f,   0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		-0.0635f, 0.0795f,  0.014f,   0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
		-0.0635f, 0.0475f,  0.014f,   0.0f,  0.0f, 1.0f,  0.0f, 1.0f,

		-0.0635f, 0.0475f, -0.014f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		 0.0635f, 0.0475f, -0.014f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.0635f, 0.0795f, -0.014f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.0635f, 0.0795f, -0.014f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		-0.0635f, 0.0795f, -0.014f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		-0.0635f, 0.0475f, -0.014f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

		 0.0635f, 0.0475f, -0.014f,   1.0f,  0.0f, 0.0f,  0.0f, 1.0f,
		 0.0635f, 0.0475f,  0.014f,   1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
		 0.0635f, 0.0795f,  0.014f,   1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
		 0.0635f, 0.0795f,  0.014f,   1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
		 0.0635f, 0.0795f, -0.014f,   1.0f,  0.0f, 0.0f,  0.0f, 0.0f,
		 0.0635f, 0.0475f, -0.014f,   1.0f,  0.0f, 0.0f,  0.0f, 1.0f,

		-0.0635f, 0.0475f, -0.014f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.0635f, 0.0475f,  0.014f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.0635f, 0.0795f,  0.014f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.0635f, 0.0795f,  0.014f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.0635f, 0.0795f, -0.014f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.0635f, 0.0475f, -0.014f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f
	};

	// vector for indices
	std::vector<short> kingCrossIndices;
	for (int i = 0; i < 72; i++) {
		kingCrossIndices.push_back(i);
	}

	std::vector<float> pawnVertices{
		// Pawn - half outline coordinates starting top to bottom
		// Vertex Positions    // Normals   // Texture Coordinates
		0.000f, 0.537f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 1.00f,
		0.012f, 0.535f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.98f,
		0.024f, 0.533f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.96f,
		0.033f, 0.528f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.94f,
		0.044f, 0.523f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.92f,
		0.054f, 0.516f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.90f,
		0.063f, 0.508f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.88f,
		0.068f, 0.504f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.86f,

		0.078f, 0.490f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.84f,
		0.083f, 0.478f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.82f,
		0.085f, 0.472f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.80f,
		0.085f, 0.455f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.78f,
		0.080f, 0.439f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.76f,
		0.071f, 0.426f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.74f,
		0.057f, 0.412f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.72f,
		0.050f, 0.402f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.70f,

		0.052f, 0.393f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.68f,
		0.057f, 0.386f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.66f,
		0.073f, 0.377f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.64f,
		0.089f, 0.367f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.62f,
		0.099f, 0.357f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.60f,
		0.099f, 0.355f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.58f,
		0.091f, 0.348f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.56f,
		0.074f, 0.343f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.54f,

		0.056f, 0.331f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.52f,
		0.047f, 0.315f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.50f,
		0.044f, 0.298f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.48f,
		0.047f, 0.258f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.46f,
		0.054f, 0.215f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.44f,
		0.063f, 0.184f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.42f,
		0.080f, 0.173f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.40f,
		0.091f, 0.165f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.38f,

		0.090f, 0.159f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.36f,
		0.089f, 0.143f, 0.0f,   1.0f, -1.0f, 0.0f,  0.00f, 0.34f,
		0.094f, 0.131f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.32f,
		0.111f, 0.109f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.30f,
		0.116f, 0.104f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.28f,
		0.130f, 0.089f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.26f,
		0.138f, 0.070f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.24f,
		0.138f, 0.061f, 0.0f,   1.0f, 1.0f, 0.0f,  0.00f, 0.22f,

		0.133f, 0.054f, 0.0f,   1.0f,  -1.0f, 0.0f,  0.00f, 0.20f,
		0.131f, 0.051f, 0.0f,   1.0f,  -1.0f, 0.0f,  0.00f, 0.18f,
		0.136f, 0.043f, 0.0f,   1.0f,  1.0f, 0.0f,  0.00f, 0.16f,
		0.144f, 0.037f, 0.0f,   1.0f,  1.0f, 0.0f,  0.00f, 0.14f,
		0.148f, 0.020f, 0.0f,   1.0f,  1.0f, 0.0f,  0.00f, 0.12f,
		0.141f, 0.007f, 0.0f,   1.0f,  -1.0f, 0.0f,  0.00f, 0.10f,
		0.115f, 0.001f, 0.0f,   1.0f,  -1.0f, 0.0f,  0.00f, 0.08f,
		0.000f, 0.000f, 0.0f,   1.0f,  -1.0f, 0.0f,  0.00f, 0.06f
	};

	// Create empty vector for indices
	std::vector<short> pawnIndices;
	// Generate model from outline
	OutlineModel(pawnVertices, pawnIndices);

	

	// Vector for plane coordinates, color cyan - hidden, texture repeat 4 times on x and z axis
	std::vector<float> planeVertices =
	{
		1.38f, 0.0f, 1.38f, 1.0f, 0.0f, 1.0f,  4.0f, 4.0f,
		1.38f, 0.0f, -1.38f, 1.0f, 1.0f, -1.0f,  4.0f, 0.0f,
		-1.38f, 0.0f, -1.38f, -1.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		-1.38f, 0.0f, 1.38f, -1.0f, 1.0f, 1.0f,  0.0f, 4.0f
	};
	// vector for indices
	std::vector<short> planeIndices =
	{
		0, 1, 3,
		1, 2, 3
	};

	// positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	// Bishop positions for black
	glm::vec3 bishopPositions[] = {
		glm::vec3(0.5175f, 0.0f, 1.2075f),
		glm::vec3(-0.5175f, 0.0f, 1.2075f),
	};
	// Bishop positions for white
	glm::vec3 bishopPositions2[] = {
		glm::vec3(0.5175f, 0.0f, -1.2075f),
		glm::vec3(-0.5175f, 0.0f, -1.2075f),
	};
	// Knight positions for black
	glm::vec3 knightPositions[] = {
		glm::vec3(0.8625f, 0.0f, 1.2075f),
		glm::vec3(-0.8625f, 0.0f, 1.2075f),
	};
	glm::vec3 knightHeadPositions[] = {
		glm::vec3(0.8625f, 0.0f, 1.2075f),
		glm::vec3(-0.8625f, 0.0f, 1.2075f),
	};

	// Knight positions for white
	glm::vec3 knightPositions2[] = {
		glm::vec3(0.8625f, 0.0f, -1.2075f),
		glm::vec3(-0.8625f, 0.0f, -1.2075f),
	};
	glm::vec3 knightHeadPositions2[] = {
		glm::vec3(0.8625f, 0.0f, -1.2075f),
		glm::vec3(-0.8625f, 0.0f, -1.2075f),
	};

	// Rook positions for black
	glm::vec3 rookPositions[] = {
		glm::vec3(1.2075f, 0.0f, 1.2075f),
		glm::vec3(-1.2075f, 0.0f, 1.2075f),
	};
	glm::vec3 rookTopPositions[] = {
		glm::vec3(1.2075f, 0.489f, 1.2075f),
		glm::vec3(-1.2075f, 0.489f, 1.2075f)
	};
	// Rook positions for white
	glm::vec3 rookPositions2[] = {
		glm::vec3(1.2075f, 0.0f, -1.2075f),
		glm::vec3(-1.2075f, 0.0f, -1.2075f),
	};
	glm::vec3 rookTopPositions2[] = {
		glm::vec3(1.2075f, 0.489f, -1.2075f),
		glm::vec3(-1.2075f, 0.489f, -1.2075f)
	};
	// Queen positions for black
	glm::vec3 queenPositions[] = {
		glm::vec3(0.1725f, 0.0f, 1.2075f)
	};
	// Queen positions for white
	glm::vec3 queenPositions2[] = {
		glm::vec3(0.1725f, 0.0f, -1.2075f)
	};
	// King positions for black
	glm::vec3 kingPositions[] = {
		glm::vec3(-0.1725f, 0.0f, 1.2075f)
	};
	glm::vec3 kingCrossPositions[] = {
		glm::vec3(-0.1725f, 0.743f, 1.2075f)
	};
	// King positions for white
	glm::vec3 kingPositions2[] = {
		glm::vec3(-0.1725f, 0.0f, -1.2075f)
	};
	glm::vec3 kingCrossPositions2[] = {
		glm::vec3(-0.1725f, 0.743f, -1.2075f)
	};
	// Pawn positions for black
	glm::vec3 pawnPositions[] = {
		glm::vec3(0.1725f, 0.0f, 0.8625f),
		glm::vec3(0.5175f, 0.0f, 0.8625f),
		glm::vec3(0.8625f, 0.0f, 0.8625f),
		glm::vec3(1.2075f, 0.0f, 0.8625f),
		glm::vec3(-0.1725f, 0.0f, 0.8625f),
		glm::vec3(-0.5175f, 0.0f, 0.8625f),
		glm::vec3(-0.8625f, 0.0f, 0.8625f),
		glm::vec3(-1.2075f, 0.0f, 0.8625f)
	};
	// Pawn positions for white
	glm::vec3 pawnPositions2[] = {
		glm::vec3(0.1725f, 0.0f, -0.8625f),
		glm::vec3(0.5175f, 0.0f, -0.8625f),
		glm::vec3(0.8625f, 0.0f, -0.8625f),
		glm::vec3(1.2075f, 0.0f, -0.8625f),
		glm::vec3(-0.1725f, 0.0f, -0.8625f),
		glm::vec3(-0.5175f, 0.0f, -0.8625f),
		glm::vec3(-0.8625f, 0.0f, -0.8625f),
		glm::vec3(-1.2075f, 0.0f, -0.8625f)
	};
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(3.0f,  5.0f,  3.0f),
		glm::vec3(-3.0f, 1.0f, 3.0f),
		glm::vec3(3.0f,  1.0f, -3.0f),
		glm::vec3(-3.0f,  5.0f, -3.0f)
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() *sizeof(float), &vertices.front(), GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// configure bishop's VAO and VBO
	unsigned int bishopVBO = 0, bishopVBO2 = 0, bishopVAO = 0;
	LoadModel(bishopVertices, bishopIndices, bishopVBO, bishopVBO2, bishopVAO);
	
	// configure knight's VAO and VBO
	unsigned int knightVBO = 0, knightVBO2 = 0, knightVAO = 0;
	LoadModel(knightVertices, knightIndices, knightVBO, knightVBO2, knightVAO);
	
	unsigned int knightHeadVBO = 0, knightHeadVBO2 = 0, knightHeadVAO = 0;
	LoadModel(knightHeadVertices, knightHeadIndices, knightHeadVBO, knightHeadVBO2, knightHeadVAO);
	
	// configure rook's VAO and VBO
	unsigned int rookVBO = 0, rookVBO2 = 0, rookVAO = 0;
	LoadModel(rookVertices, rookIndices, rookVBO, rookVBO2, rookVAO);
	
	unsigned int rookTopVBO = 0, rookTopVBO2 = 0, rookTopVAO = 0;
	LoadModel(rookTopVertices, rookTopIndices, rookTopVBO, rookTopVBO2, rookTopVAO);
	
	// configure queen's VAO and VBO
	unsigned int queenVBO = 0, queenVBO2 = 0, queenVAO = 0;
	LoadModel(queenVertices, queenIndices, queenVBO, queenVBO2, queenVAO);
	
	// configure king's VAO and VBO
	unsigned int kingVBO = 0, kingVBO2 = 0, kingVAO = 0;
	LoadModel(kingVertices, kingIndices, kingVBO, kingVBO2, kingVAO);
	
	unsigned int kingCrossVBO = 0, kingCrossVBO2 = 0, kingCrossVAO = 0;
	LoadModel(kingCrossVertices, kingCrossIndices, kingCrossVBO, kingCrossVBO2, kingCrossVAO);

	// configure pawn's VAO and VBO
	unsigned int pawnVBO = 0, pawnVBO2 = 0, pawnVAO = 0;
	LoadModel(pawnVertices, pawnIndices, pawnVBO, pawnVBO2, pawnVAO);

	// configure plane VAO and VBO
	unsigned int planeVBO = 0, planeVBO2 = 0, planeVAO = 0;
	LoadModel(planeVertices, planeIndices, planeVBO, planeVBO2, planeVAO);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int blackDiffuseMap = loadTexture("blackMarble.jpg");
	unsigned int blackSpecularMap = loadTexture("blackMarble_specular.jpg");
	unsigned int whiteDiffuseMap = loadTexture("whiteMarble.jpg");
	unsigned int whiteSpecularMap = loadTexture("whiteMarble_specular.jpg");
	unsigned int checkerDiffuseMap = loadTexture("checkerMarble.jpg");
	unsigned int checkerSpecularMap = loadTexture("checkerMarble_specular.jpg");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		/*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.00f, 0.00f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.2f, 0.0f, 0.0f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.00f, 0.00f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.2f, 0.0f, 0.0f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 projection;
		if (defaultView == true) {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		else {
			projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);
		}
		
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blackDiffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blackSpecularMap);

		// render black bishops
		glBindVertexArray(bishopVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, bishopPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, bishopIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render black knights
		glBindVertexArray(knightVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, knightPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, knightIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render black knights head
		glBindVertexArray(knightHeadVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, knightHeadPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, knightHeadIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render black rook
		glBindVertexArray(rookVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, rookPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, rookIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render black rook top
		glBindVertexArray(rookTopVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, rookTopPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, rookTopIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render black queen
		glBindVertexArray(queenVAO);
		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, queenPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, queenIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render black king
		glBindVertexArray(kingVAO);
		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, kingPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, kingIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render black king cross
		glBindVertexArray(kingCrossVAO);
		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, kingCrossPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, kingCrossIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render black pawns
		glBindVertexArray(pawnVAO);
		for (unsigned int i = 0; i < 8; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pawnPositions[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, pawnIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, whiteDiffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, whiteSpecularMap);

		// render white bishops
		glBindVertexArray(bishopVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, bishopPositions2[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, bishopIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render white knights
		glBindVertexArray(knightVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, knightPositions2[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, knightIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render white knights head
		glBindVertexArray(knightHeadVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, knightHeadPositions2[i]);
			float angle = 180.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, knightHeadIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render white rook
		glBindVertexArray(rookVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, rookPositions2[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, rookIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render white rook top
		glBindVertexArray(rookTopVAO);
		for (unsigned int i = 0; i < 2; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, rookTopPositions2[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, rookTopIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render white queen
		glBindVertexArray(queenVAO);
		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, queenPositions2[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, queenIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render white king
		glBindVertexArray(kingVAO);
		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, kingPositions2[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, kingIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render white king cross
		glBindVertexArray(kingCrossVAO);
		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, kingCrossPositions2[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, kingCrossIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// render white pawns
		glBindVertexArray(pawnVAO);
		for (unsigned int i = 0; i < 8; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pawnPositions2[i]);
			float angle = 0.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, pawnIndices.size(), GL_UNSIGNED_SHORT, NULL);
		}

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, checkerDiffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, checkerSpecularMap);

		// render plane
		glBindVertexArray(planeVAO);
		float angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		lightingShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, planeIndices.size(), GL_UNSIGNED_SHORT, NULL);

		// also draw the lamp object(s)
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
/*Loads a model into the buffer*/
void LoadModel(std::vector<float>& vertices, std::vector<short>& indices, unsigned int& VBO, unsigned int& VBO2, unsigned int& VAO)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(short), &indices.front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}
/*Uses the half outline vertices from a vector to generate a rotated model from the outline
//Updates the vertices vector and indices vector*/
void OutlineModel(std::vector<float>& vertices, std::vector<short>& indices) {
	// Capture size of original vector
	int sizeInitial = vertices.size();
	// determine number of slices to iterate, higher number = higher quality
	int sliceCount = 20;
	// Adds rotational vertex data to the vector (slice count + 1 used for full rotation).  Iterates original vector for each angle.
	for (int j = 1; j < sliceCount + 1; j++) {
		for (int i = 0; i < sizeInitial; i++) {
			// push the value of the rotated x coordinate
			vertices.push_back(vertices.at(i) * cos((360 / sliceCount * j) * PI / 180.0));
			i++;
			// push the original y value, this doesn't change
			vertices.push_back(vertices.at(i));
			i++;
			// push the value of the rotated z coordinate, calculated from the x value
			vertices.push_back(vertices.at(i - 2) * sin((360 / sliceCount * j) * PI / 180.0));
			i++;
			// push normal values with rotation

			if (j > 5 && j < 15) {
				vertices.push_back(-1.0f);
				i++;
			}
			else {
				vertices.push_back(1.0f);
				i++;
			}
			vertices.push_back(vertices.at(i));
			i++;
			if (j > 0 && j < 11) {
				vertices.push_back(1.0f);
				i++;
			}
			else {
				vertices.push_back(-1.0f);
				i++;
			}

			vertices.push_back(vertices.at(i) + 0.05f * j);
			i++;
			vertices.push_back(vertices.at(i));
		}
	}

	// Capture size of final vector
	int sizeFinal = vertices.size();
	// start variable accounts for first indice
	int start = 0;
	// Adds indices to vector to form triangles.  Pattern is current rotation first and second with next rotation first
	// followed by current rotation second with next rotation first and second.
	for (int j = sizeInitial / 8; j < (sizeFinal / 8) - 1;) {
		indices.push_back(start);
		start++;
		indices.push_back(start);
		indices.push_back(j);
		indices.push_back(start);
		indices.push_back(j);
		j++;
		indices.push_back(j);
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (defaultView == true) {
			defaultView = false;
		}
		else {
			defaultView = true;
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}