/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

Purpose :   Instructions on how to use this software
Language:   C++ Visual Studio
Platform:   Windows 10
Project :   cody.morgan_CS300_1
Author  :   Cody Morgan  ID: 180001017
Date  :   4 OCT 2019
End Header --------------------------------------------------------*/

#include "ShaderManagement.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using glm::vec3;
using glm::vec4;

#include <iostream>
using std::cout;

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"

#include "ObjectManagement.h"
#include "Camera.h"
#include "Light.h"

// scene relates
static vector<Object> objects;
static vector<Light> lights;
static Camera* camera;

// common vectors
static vec3 up(0, 1, 0);
static vec3 right(1, 0, 0);
static vec3 down(0, -1, 0);
static vec3 left(-1, 0, 0);
static vec3 back(0, 0, -1);
static vec3 forward(0, 0, 1);

// gui related
static int selectedObject = 0;
static int selectedModel = 0;
static int selectedLight = 0;

// object relates
static ShaderManager shaderManager;
static MaterialManager* materials;


int FindObject(string name);

/******************************************************************************
  brief : creates a window

  input : width  - window width
      height - window height
      major  - opengl version major
      minor  - opengl minor
      window - window handle

  output: true if everything is ok

******************************************************************************/
bool WindowInit(int width, int height, int major, int minor, GLFWwindow** window)
{
  GLenum error = glfwInit();
  if (error == GLFW_FALSE)
  {
    char buffer[256];
    glfwGetError((const char**)buffer);
    return getchar();
  }

  // instance the window
  *window = glfwCreateWindow(width, height, "CS300 Assgnment 1", NULL, NULL);
  glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


  // create the window obj
  if (*window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(*window);

  error = glewInit();
  if (error)
  {
    cout << glewGetErrorString(error);
    return getchar();
  }
  glViewport(0, 0, width, height);
  // window is gtg
  return true;
}

/******************************************************************************
  brief : cleans up program

******************************************************************************/
void Terminate()
{
  glfwTerminate();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  delete materials;
}

/******************************************************************************
  brief : process all keyboard input

  input : window - window handle

******************************************************************************/
void ProcessInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

    float moveStr = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
    lights[selectedLight].translate(moveStr * up);
    camera->reset();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
    lights[selectedLight].translate(moveStr * down);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
    lights[selectedLight].translate(moveStr * back);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
    lights[selectedLight].translate(moveStr * forward);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
    lights[selectedLight].translate(moveStr * left);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
    lights[selectedLight].translate(moveStr * right);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
    objects[0].spin(.5, up);
    }

    float cameraScale = 0.1;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
    camera->translate(cameraScale * forward);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
    camera->translate(cameraScale * left);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
    camera->translate(cameraScale * back);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
    camera->translate(cameraScale * right);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
    camera->translate(cameraScale * down);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
    camera->translate(cameraScale * up);
    }
}


/******************************************************************************
  brief : error callback

******************************************************************************/
void GLAPIENTRY MessageCallback(GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam)
{

#ifndef DEBUG_SEVERITY_HIGH
#define DEBUG_SEVERITY_HIGH     0x9146
#define DEBUG_SEVERITY_MEDIUM     0x9147
#define DEBUG_SEVERITY_LOW      0x9148
#define DEBUG_SEVERITY_NOTIFICATION 0x826B

#define DEBUG_TYPE_ERROR         0x824C
#define DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define DEBUG_TYPE_PORTABILITY     0x824F
#define DEBUG_TYPE_PERFORMANCE     0x8250
#define DEBUG_TYPE_OTHER         0x8251
#define DEBUG_TYPE_MARKER        0x8268
#endif

  if (severity != DEBUG_SEVERITY_NOTIFICATION)
  {
    string debugTypes[] =
    {
      "DEBUG_TYPE_ERROR",
      "DEBUG_TYPE_DEPRECATED_BEHAVIOR",
      "DEBUG_TYPE_UNDEFINED_BEHAVIOR",
      "DEBUG_TYPE_PORTABILITY",
      "DEBUG_TYPE_PERFORMANCE",
      "DEBUG_TYPE_OTHER",
      "DEBUG_TYPE_MARKER"
    };
    string severityTypes[] =
    {
      "HIGH",
      "MEDIUM",
      "LOW"
    };

    string typeString = debugTypes[(type - DEBUG_TYPE_ERROR) % 7];
    string severityString = severityTypes[(severity - DEBUG_SEVERITY_HIGH) % 3];

    fprintf(stderr, "Error\n   Severity: %s   Type: %s\n   \n   %s\n\n", severityString.c_str(), typeString.c_str(), message);
  }
}


void Render(GLFWwindow* window, Camera& camera, const vector<Light>& lightArray)
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (Light light : lightArray)
  {
    light.update();
  }
  camera.update(shaderManager);
  materials->updateUBO(objects);
  for (Object object : objects)
  {
    object.draw();
  }
}

void GenerateScene(ShaderManager& shaderManager)
{
  int phongLightingSP = shaderManager.getShader(ShaderType::PhongLighting);

  // generate out God object
  Object center(shaderManager.getShader(ShaderType::Diffuse), "OBJ model");
  center.loadOBJ("Common/models/4sphere.obj");
  center.material.diffuse = vec3(0.25);
  center.genFaceNormals();
  objects.push_back(center);

  // generate a bunch of lights
  int ballCount = 1;
  int circleRadius = 4;
  int diffuseSP = shaderManager.getShader(ShaderType::Diffuse);
  for (int i = 0; i < ballCount; i++)
  {
    Light light(shaderManager.getShader(ShaderType::Diffuse), diffuseSP, "light");
    light.emitter.loadSphere(0.5, 30);
    light.translate(vec3(circleRadius * glm::cos(glm::radians(360.0f / ballCount * i)), 0, circleRadius * glm::sin(glm::radians(360.0f / ballCount * i))));
    light.emitter.orbitRadius = circleRadius;
    light.lightData.emissive = glm::vec4(0.5, 0.5, 0.5,0);
    light.setColor(vec3(i / float(ballCount), 0.7f, 0));

    lights.push_back(light);
  }
  
  // keep the orbit tracker
  Object circle(shaderManager.getShader(ShaderType::Diffuse), "circle");
  circle.loadcircle(circleRadius, 45);
  circle.material.diffuse = vec3(0, 0, 0);
  objects.push_back(circle);

  // load out floor
  Object floor(diffuseSP, "floor");
  floor.loadPlane();
  floor.translate(vec3(0, -1, -2));
  floor.addScale(vec3(10));
  floor.material.diffuse = vec3(1, 1, 1);
  objects.push_back(floor);
}

void UpdateScene(GLFWwindow* window)
{
  // rotate balls
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
  {
    for (size_t i = 0; i < lights.size(); i++)
    {
      if (lights[i].emitter.name.find("light") != string::npos)
      {
        float angularV = -0.5f;
        float radius = lights[i].emitter.orbitRadius;
        lights[i].rotateY(angularV, radius);
      }
    }
  }

  GetLightManager()->updateUBO(lights);
}

void ShutdownScene()
{
  lights.clear();
  objects.clear();
  camera->reset();
}

int FindObject(string name)
{
  for (int i = 0; i < objects.size(); i++)
  {
    if (objects[i].name.find(name) != string::npos)
    {
      return i;
    }
  }

  return -1;
}

void InitGUI(GLFWwindow* window)
{
  // context 
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  // open GL
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  // setup stuff
  ImGui::StyleColorsDark();
}

void UpdateGUI()
{
  const char* files[] =
  {
    "4Sphere.obj",
    "bunny.obj",
    "bunny_high_poly.obj",
    "cube.obj",
    "cube2.obj",
    "cup.obj",
    "lucy_princeton.obj",
    "rhino.obj",
    "sphere.obj",
    "sphere_modified.obj",
    "starwars1.obj",
  };
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Welcome to CS300 Assignment 2!");

  // normal stuff
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("Selected Object");
  ImGui::SliderInt(objects[selectedObject].name.c_str(), &selectedObject, 0, objects.size() - 1);
  bool toggleFN = ImGui::Button("Toggle selected object's Face Normal");
  bool toggleVN = ImGui::Button("Toggle selected object's Vertex Normal");
  bool changeNormalLength = ImGui::SliderFloat("Normal Display Scale", &objects[selectedObject].vectorScale, 0, 2.0);
  bool changeModel = ImGui::ListBox("Model Selection", &selectedModel, files, _countof(files));
  ImGui::End();

  if (toggleFN)
  {
    objects[selectedObject].toggleFaceNormals();
  }
  else if (toggleVN)
  {
    objects[selectedObject].toggleVertexNormals();
  }
  if (changeNormalLength)
  {
    if (objects[selectedObject].faceNormDrawingMode == Object::FaceNormalDrawingMode::FaceNormalsOn)
    {
      objects[selectedObject].toggleFaceNormals(true,true);
    }
    else if (objects[selectedObject].vertexNormalDrawingMode == Object::VertexNormalDrawingMode::VertexNormalOn)
    {
      objects[selectedObject].toggleVertexNormals(true, true);
    }
  }

  if (changeModel)
  {
    string fileName = "Common/models/";
    fileName += files[selectedModel];
    objects[FindObject("OBJ model")].loadOBJ(fileName);
  }
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
   camera->projection = glm::perspective(glm::radians(45.0f), float(width) / height, 0.1f, 1000.0f);
   int display_w, display_h;
   glfwGetFramebufferSize(window, &display_w, &display_h);
   glViewport(0, 0, display_w, display_h);
}

int main()
{
  // make a window
  GLFWwindow* window = nullptr;
  if (WindowInit(1600 , 1200 , 4, 0, &window) == false)
    return -1;

  // GL state setting
#ifdef _DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
#endif
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  
  // initialization of generic scene
  InitGUI(window);

  int phongLightingSP = shaderManager.addShader("Source/PhongLighting.vert", "Source/PhongLighting.frag", ShaderType::PhongLighting);
  shaderManager.addShader("Source/PhongDiffuse.vert", "Source/PhongLighting.frag", ShaderType::Diffuse);
  camera = &Camera(vec3(0,-4,-8), 30.0f, vec3(1,0,0), phongLightingSP);
  GetLightManager()->genUBO(shaderManager.getShader(ShaderType::Diffuse));
  materials = new MaterialManager;
  materials->genUBO(phongLightingSP);

  // generate the scene for this homework - someday have scene switching?
  GenerateScene(shaderManager);

  while (!glfwWindowShouldClose(window))
  {
    // scene loop
    double time = glfwGetTime();
    ProcessInput(window);
    UpdateGUI();
    UpdateScene(window);

    //maintain viewport
    glfwSetFramebufferSizeCallback(window, window_size_callback);

    // render scene and GUI window
    Render(window, *camera, lights);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();

    // maintain a max of 120hertz
    double dt = glfwGetTime() - time;
    while (dt < 1 / 120.0)
    {
      dt = glfwGetTime() - time;
    }
  }

  Terminate();

  return 0;
}