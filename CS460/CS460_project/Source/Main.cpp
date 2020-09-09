/******************************************************************************
  Project : load, display and manipulate a 3D object
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ObjectManager.h"
#include "ShaderManager.h"
#include "Camera.h"
#include "Common.h"
#include "Light.h"

// managers and static variables
static ObjectManager* objectMgr = nullptr;
static ShaderManager* shaderMgr = nullptr;
static Camera* camera = nullptr;

/////***** Window and OpenGL Management *****/////


bool WindowInit(int major, int minor, GLFWwindow** window)
{
  GLFWvidmode return_struct;
  GLenum error = glfwInit();
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());


  if (error == GLFW_FALSE)
  {
    char buffer[256];
    glfwGetError((const char**)buffer);
    return getchar();
  }

  // instance the window
  *window = glfwCreateWindow(mode->width, mode->height, "CS350 Assignment 1 Cody Morgan", NULL, NULL);
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
  glViewport(0, 0, mode->width, mode->height);

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
#define DEBUG_SEVERITY_HIGH            0x9146
#define DEBUG_SEVERITY_MEDIUM          0x9147
#define DEBUG_SEVERITY_LOW             0x9148
#define DEBUG_SEVERITY_NOTIFICATION    0x826B
#define DEBUG_TYPE_ERROR               0x824C
#define DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define DEBUG_TYPE_PORTABILITY         0x824F
#define DEBUG_TYPE_PERFORMANCE         0x8250
#define DEBUG_TYPE_OTHER               0x8251
#define DEBUG_TYPE_MARKER              0x8268
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

void Window_size_callback(GLFWwindow* window, int width, int height)
{
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
}



void SceneSetup()
{
  Object* obj = objectMgr->addObject("model");
  obj->setShader(ShaderType::Phong);
  //obj->loadSphere(1, 50);
  obj->loadModel("C:/Users/Cody/Desktop/fbx/Dragon_Animated.fbx");
  obj->material.diffuse = vec3(0.1, 0.2, 0.3);
  obj->material.ambient = vec3(0.1, 0.1, 0.1);
  obj->rotate(-90, vec3(0), vec3(1, 0, 0));
  obj->translate(vec3(0, 0, -1));

  Object* obj2 = objectMgr->addLight("light");
  obj2->translate(right * 3.0f);
  obj2->scale(vec3(0.2f));
  dynamic_cast<Light*>(obj2)->lightData.ambient = vec4(1);

  Object* obj3 = objectMgr->addLight("light");
  obj3->translate(left * 3.0f);
  obj3->scale(vec3(0.2f));
  dynamic_cast<Light*>(obj3)->lightData.ambient = vec4(1);
}

void SceneUpdate()
{
  objectMgr->getFirstObjectByName("light")->rotate(-1, left * 10.0f);
}

void SceneShutdown()
{
  objectMgr->removeAllObjects();
}

/////***** GUI Management *****/////


void InitGUI(GLFWwindow* window)
{
  // GL state setting
#ifdef _DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
#endif
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

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

void ProcessInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // camera movement
  float speed = 0.1f;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera->translate(forward * speed);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera->translate(right * speed);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera->translate(back * speed);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera->translate(left * speed);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera->translate(up * speed);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera->translate(down * speed);
  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    camera->rotate(1, up);
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    camera->rotate(-1, up);

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    objectMgr->getAt(0)->rotate(1, vec3(0), vec3(1, 0, 0));
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    objectMgr->getAt(0)->rotate(-1, vec3(0), vec3(1, 0, 0));
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    objectMgr->getAt(0)->rotate(1, vec3(0), vec3(0, 0, 1));
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    objectMgr->getAt(0)->rotate(-1, vec3(0), vec3(0, 0, 1));
}


void UpdateGUI()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Object Controls");
  ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);

  if (ImGui::BeginTabBar("MyTabBar"))
  {
    if (ImGui::BeginTabItem("Model Controls"))
    {
      auto model = objectMgr->getFirstObjectByName("model");
      auto currentScale = model->getWorldScale();

      if(ImGui::SliderFloat("Scale", &currentScale.x,-10,10))
      {
        objectMgr->getFirstObjectByName("model")->scale(vec3(currentScale.x, currentScale.x, currentScale.x));
      }

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Broccoli"))
    {
      ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Cucumber"))
    {
      ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  
  ImGui::End();
}

void GUIendFrame(GLFWwindow* window, double time)
{
  //maintain viewport
  glfwSetFramebufferSizeCallback(window, Window_size_callback);

  // render scene and GUI window
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(window);
  glfwPollEvents();

  // maintain a max of 120hertz
  double dt = glfwGetTime() - time;
  while (dt < (1 / 120.0))
  {
    dt = glfwGetTime() - time;
  }
}


/////***** Main *****/////


int main()
{
  // make a window
  GLFWwindow* window = nullptr;
  if (WindowInit(4, 0, &window) == false)
    return -1;

  // setup GLFW and IMgui
  InitGUI(window);

  // managers setup
  objectMgr = ObjectManager::getObjectManager();
  shaderMgr = ShaderManager::getShaderManager();

  // scene setup
  camera = new Camera(vec3(0, 0, -8), 0.0f, right);

  SceneSetup();
  while (!glfwWindowShouldClose(window))
  {
    // scene loop
    double time = glfwGetTime();
    UpdateGUI();
    ProcessInput(window);

    // sim loop
    SceneUpdate();

    // end of the loop
    objectMgr->render(*camera);
    GUIendFrame(window, time);
  }

  Terminate();

  return 0;
}