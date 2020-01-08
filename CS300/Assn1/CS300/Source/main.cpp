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

#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"

#include "Object.h"
#include "ShaderManager.h"
#include "Camera.h"

// static stuff
static vec3 up(0, 1, 0);
static vec3 right(1, 0, 0);
static vec3 down(0, -1, 0);
static vec3 left(-1, 0, 0);
static vec3 back(0, 0, -1);
static vec3 forward(0, 0, 1);

// managers
static ObjectManager* objectMgr = nullptr;
static ShaderManager* shaderMgr = nullptr;

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

void UpdateGUI()
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Welcome to CS300 Assignment 2!");

  // normal stuff
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();

}

void Window_size_callback(GLFWwindow* window, int width, int height)
{
   int display_w, display_h;
   glfwGetFramebufferSize(window, &display_w, &display_h);
   glViewport(0, 0, display_w, display_h);
}

void GUIendFrame(GLFWwindow* window, float time)
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

void SceneSetup()
{
  Object* obj = objectMgr->addObject("Object");
  obj->setShader("shaders/Passthrough.vert", "shaders/normalShader.frag", ShaderType::Passthrough);
  obj->loadeCube(1.0f);

  Object* obj2 = objectMgr->addObject("Object2");
  obj2->setShader("shaders/Passthrough.vert", "shaders/normalShader.frag", ShaderType::Passthrough);
  obj2->loadeCube(0.5f);
  obj2->translate(vec3(3, 0, 0));
}
void SceneUpdate()
{
  objectMgr->getFirstObjectByName("Object")->rotate(1,vec3(3,0,0));
}

int main()
{
  // make a window
  GLFWwindow* window = nullptr;
  if (WindowInit(1600 , 1200 , 4, 0, &window) == false)
    return -1;
  
  // setup GLFW and IMgui
  InitGUI(window);
  
  // managers setup
  objectMgr = ObjectManager::getObjectManager();
  shaderMgr = ShaderManager::getShaderManager();

  Camera camera = Camera(vec3(0, -4, -8), 30.0f, right);

  // scene setup
  SceneSetup();

  while (!glfwWindowShouldClose(window))
  {
    // scene loop
    double time = glfwGetTime();
    UpdateGUI();

    // sim loop
    SceneUpdate();

    // end of the loop
    objectMgr->render(camera);
    GUIendFrame(window, time);
  }

  Terminate();

  return 0;
}