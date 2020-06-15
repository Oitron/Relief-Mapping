//Run test platform: Windows 10, Visual Studio 12 2013
//OpenGL version: Version 3.3 Core. Encapsulation: GLAD
//Library used: GLFW, GLM


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.h"
#include <direct.h>


/*---Define functions such as window display settings and response to mouse and keyboard events---*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/*---Define the function to load the texture---*/
unsigned int load_texture(char const * path);

/*---Define the function to get the path of the texture image---*/
std::string get_full_path(std::string path);

/*---Define the GPU rendering function, including all data that needs to be sent to the vertex shader---*/
void render_obj();


/*---Some global variable settings---*/
//The width and height of the display window
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

//camera settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/*Used to select different texture modes:*/
//1.Don't use any Mappings
//2.Use Normal Mapping.
//3.Use Parallax Mapping.
//4.Use Relief Mapping.
int renderType = 1; //default					

//Define a cube to simulate the light source
float light_vertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,

	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,

	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,

	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
};

//Define light source position
glm::vec3 lightPos(-0.75f, 1.1f, 0.5f);



/*---Start main function---*/
int main() {

	/*Initialize glfw window*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/*Create window*/
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Various Mappings Test", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback); //Respond to cursor events(Camera lens rotation)
	glfwSetScrollCallback(window, scroll_callback); //Respond to scroll events(Camera focal length change)

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Open OpenGL depth test
	glEnable(GL_DEPTH_TEST);

	/*create shaders*/
	Shader shader_NoMap("vertex_shader\\v_NoMap.vs", "fragment_shader\\f_NoMap.fs");//Don't use any Mappings
	Shader shader_NormalMap("vertex_shader\\v_NormalMap.vs", "fragment_shader\\f_NormalMap.fs");//Use Normal Mapping
	Shader shader_ParallaxMap("vertex_shader\\v_ParallaxMap.vs", "fragment_shader\\f_ParallaxMap.fs");//Use Parallax Mapping
	Shader shader_ReliefMap("vertex_shader\\v_ReliefMap.vs", "fragment_shader\\f_ReliefMap.fs");//Use Relief Mapping
	Shader shader_Light("vertex_shader\\v_light.vs", "fragment_shader\\f_light.fs");//create shader of light source

	/*load texture image*/
	unsigned int toyBoxDiffuseMap = load_texture(get_full_path("img\\wood_texture.jpg").c_str());
	unsigned int toyBoxNormalMap = load_texture(get_full_path("img\\toyBox_normal.png").c_str());
	unsigned int toyBoxParallaxMap = load_texture(get_full_path("img\\toyBox_depth.png").c_str());

	/*define shaders*/
	shader_NormalMap.use();
	shader_NormalMap.setInt("diffuseMap", 0);
	shader_NormalMap.setInt("normalMap", 1);

	shader_ParallaxMap.use();
	shader_ParallaxMap.setInt("diffuseMap", 0);
	shader_ParallaxMap.setInt("normalMap", 1);
	shader_ParallaxMap.setInt("depthMap", 2);

	shader_ReliefMap.use();
	shader_ReliefMap.setInt("diffuseMap", 0);
	shader_ReliefMap.setInt("normalMap", 1);
	shader_ReliefMap.setInt("parallaxMap", 2);


	/*create VBO and VAO corresponding to the light source*/
	unsigned int light_VBO, light_VAO;
	
	glGenBuffers(1, &light_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, light_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(light_vertices), light_vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &light_VAO);
	glBindVertexArray(light_VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //the light source only needs to define the coordinates, so the size is 3, the span is 3, and the offset is 0
	glEnableVertexAttribArray(0);


	//main loop
	while (!glfwWindowShouldClose(window)) {
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);//Respond to keyboard events

		/*clear screen*/
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/***---------------------------------------------------------***/
		
		//Define the projection transformation matrix to project the scene onto the screen
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();//get observation matrix
		glm::mat4 model = glm::mat4();

		/*---render---*/
		/*1.render with no map*/
		if (renderType == 1) {
			shader_NoMap.use();
			shader_NoMap.setMat4("projection", glm::value_ptr(projection));
			shader_NoMap.setMat4("view", glm::value_ptr(view));
			shader_NoMap.setMat4("model", glm::value_ptr(model));
			shader_NoMap.setVec3("viewPos", camera.Position);
			shader_NoMap.setVec3("lightPos", lightPos);
		}
		/*2.render with normal map*/
		else if (renderType == 2) {
			shader_NormalMap.use();
			shader_NormalMap.setMat4("projection", glm::value_ptr(projection));
			shader_NormalMap.setMat4("view", glm::value_ptr(view));
			shader_NormalMap.setMat4("model", glm::value_ptr(model));
			shader_NormalMap.setVec3("viewPos", camera.Position);
			shader_NormalMap.setVec3("lightPos", lightPos);
		}
		/*3.render with parallax map*/
		else if (renderType == 3){
			shader_ParallaxMap.use();
			shader_ParallaxMap.setMat4("projection", glm::value_ptr(projection));
			shader_ParallaxMap.setMat4("view", glm::value_ptr(view));
			shader_ParallaxMap.setMat4("model", glm::value_ptr(model));
			shader_ParallaxMap.setVec3("viewPos", camera.Position);
			shader_ParallaxMap.setVec3("lightPos", lightPos);
		}
		/*render with relief map*/
		else if (renderType == 4) {
			shader_ReliefMap.use();
			shader_ReliefMap.setMat4("projection", glm::value_ptr(projection));
			shader_ReliefMap.setMat4("view", glm::value_ptr(view));
			shader_ReliefMap.setMat4("model", glm::value_ptr(model));
			shader_ReliefMap.setVec3("viewPos", camera.Position);
			shader_ReliefMap.setVec3("lightPos", lightPos);
		}

		/*Activate the corresponding texture unit and create textures*/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, toyBoxDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, toyBoxNormalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, toyBoxParallaxMap);
		
		//start rending object
		render_obj();

		//start rending right source
		shader_Light.use();
		shader_Light.setMat4("projection", glm::value_ptr(projection));//adapte projection transformation matrix
		shader_Light.setMat4("view", glm::value_ptr(view));//adapte the observation transformation matrix

		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f));
		shader_Light.setMat4("model", glm::value_ptr(model));//adapte model transformation matrix

		glBindVertexArray(light_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		/***---------------------------------------------------------***/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &light_VAO);
	glDeleteBuffers(1, &light_VBO);
	glfwTerminate();

	return 0;
}

/*Define VBO and VAO of object(toybox)*/
unsigned int obj_VAO = 0;
unsigned int obj_VBO = 0;
void render_obj() {
	if (obj_VAO == 0) {
		/*the first triangle primitive*/
		//location
		glm::vec3 pos1(-1.0, 1.0, 0.0);
		glm::vec3 pos2(-1.0, -1.0, 0.0);
		glm::vec3 pos3(1.0, -1.0, 0.0);
		glm::vec3 pos4(1.0, 1.0, 0.0);
		//texture coordinates
		glm::vec2 uv1(0.0, 1.0);
		glm::vec2 uv2(0.0, 0.0);
		glm::vec2 uv3(1.0, 0.0);
		glm::vec2 uv4(1.0, 1.0);
		//normal
		glm::vec3 nm(0.0, 0.0, 1.0);

		glm::vec3 e1 = pos2 - pos1; //E1
		glm::vec3 e2 = pos3 - pos1; //E2
		glm::vec2 deltaUV1 = uv2 - uv1; //deltaUV1.x = U1 and deltaUV1.y = V1
		glm::vec2 deltaUV2 = uv3 - uv1; //deltaUV2.x = U2 and deltaUV2.y = V2

		//coefficient (1/(U1V2 - U2V1))
		float coefficient = 1 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent1, bitangent1;

		//calcule the TBN matrix
		tangent1.x = (deltaUV2.y * e1.x - deltaUV1.y * e2.x) * coefficient; //Tx
		tangent1.y = (deltaUV2.y * e1.y - deltaUV1.y * e2.y) * coefficient; //Ty
		tangent1.z = (deltaUV2.y * e1.z - deltaUV1.y * e2.z) * coefficient; //Tz
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = (-deltaUV2.x * e1.x + deltaUV1.x * e2.x) * coefficient; //Bx
		bitangent1.y = (-deltaUV2.x * e1.y + deltaUV1.x * e2.y) * coefficient; //By
		bitangent1.z = (-deltaUV2.x * e1.z + deltaUV1.x * e2.z) * coefficient; //Bz
		bitangent1 = glm::normalize(bitangent1);

		/*the second triangle primitive*/
		glm::vec3 e2_1 = pos3 - pos1;
		glm::vec3 e2_2 = pos4 - pos1;
		glm::vec2 deltaUV2_1 = uv3 - uv1;
		glm::vec2 deltaUV2_2 = uv4 - uv1;

		//coefficient
		coefficient = 1 / (deltaUV2_1.x * deltaUV2_2.y - deltaUV2_1.y * deltaUV2_2.x);
		glm::vec3 tangent2, bitangent2;
		tangent2.x = (deltaUV2_2.y * e2_1.x - deltaUV2_1.y * e2_2.x) * coefficient;
		tangent2.y = (deltaUV2_2.y * e2_1.y - deltaUV2_1.y * e2_2.y) * coefficient;
		tangent2.z = (deltaUV2_2.y * e2_1.z - deltaUV2_1.y * e2_2.z) * coefficient;
		tangent2 = glm::normalize(tangent2);

		bitangent2.x = (-deltaUV2_2.x * e2_1.x + deltaUV2_1.x * e2_2.x) * coefficient;
		bitangent2.y = (-deltaUV2_2.x * e2_1.y + deltaUV2_1.x * e2_2.y) * coefficient;
		bitangent2.z = (-deltaUV2_2.x * e2_1.z + deltaUV2_1.x * e2_2.z) * coefficient;
		bitangent2 = glm::normalize(bitangent2);

		float quadVertices[] = {					  //texture
			//location				//normal		  //coordinates	//tangent							//bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};

		glGenVertexArrays(1, &obj_VAO);
		glGenBuffers(1, &obj_VBO);
		glBindVertexArray(obj_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, obj_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);//location
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));//normal
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));//texture coordinates
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));//tangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));//bitangent
	}

	glBindVertexArray(obj_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//"esc" quit the program
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)//"Q" make the camera move forward
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)//"E" make the camera move backword
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)//"A" make the camera move to left
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)//"D" make the camera move to right
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)//"W" make the camera move up
		camera.ProcessKeyboard(UP, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)//"S" make the camera move down
		camera.ProcessKeyboard(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)//"1" render without any maps
		renderType = 1;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)//"2" render with normal map
		renderType = 2;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)//"3" render with parallax map
		renderType = 3;

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)//"4" render with relief map
		renderType = 4;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xoffset = lastX - xPos;	//the left coordinate is smaller than the right coordinate, and we need a positive angle
	float yoffset = lastY - yPos;	//the lower coordinate is larger than the upper coordinate, and we need a positive angle when we look up
	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

//scroll event
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//load texture
unsigned int load_texture(char const * path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents/*Number of channels*/;
	unsigned char * data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		}
		else if (nrComponents == 3/*RGB*/) {
			format = GL_RGB;
		}
		else if (nrComponents == 4/*RGBAlpha*/) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture loading failed, the path is:" << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

std::string get_full_path(std::string path) {
	char* buffer;
	buffer = _getcwd(NULL, 0);
	std::string fullPath(buffer);

	fullPath = fullPath + "\\" + path;

	return fullPath;
}
