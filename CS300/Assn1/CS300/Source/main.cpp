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
#include "Common.h"

// managers and static variables
static ObjectManager* objectMgr = nullptr;
static ShaderManager* shaderMgr = nullptr;
static Camera* camera = nullptr;
static bool pauseSimulation = false;


/////***** Window and OpenGL Management *****/////


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

void Window_size_callback(GLFWwindow* window, int width, int height)
{
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
}


/////***** Scene Management *****/////


void SceneSetup()
{
  Object* obj = objectMgr->addObject("model");
  obj->setShader("shaders/Passthrough.vert", "shaders/normalShader.frag", ShaderType::Passthrough);
  obj->loadOBJ("Common/models/4Sphere.obj");

  Object* obj2 = objectMgr->addObject("ball");
  obj2->setShader("shaders/Passthrough.vert", "shaders/normalShader.frag", ShaderType::Passthrough);
  obj2->loadSphere(1.0f, 50);
  obj2->translate(right * 3.0f);

  Object* obj3 = objectMgr->addObject("center");
  obj3->setShader("shaders/Passthrough.vert", "shaders/normalShader.frag", ShaderType::Passthrough);
  obj3->loadeCube(1.0f);
  obj3->wiremode = true;

}

void SceneUpdate()
{
  // dont update invalid object managers
  if (!objectMgr->isValid() || pauseSimulation)
    return;

  objectMgr->getFirstObjectByName("model")->rotate(1);
  objectMgr->getFirstObjectByName("ball")->rotate(-1, left * 3.0f);
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

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    pauseSimulation = true;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    pauseSimulation = false;
}

void UpdateGUI()
{
  // get all current state data
  int& objectIndex = objectMgr->selectedObject;
  Object* selectedObject = objectMgr->getSelected();

  // dont use any GUI if there are no objects
  if (!selectedObject)
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("ERROR - THERE ARE NO OBJECTS");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    return;
  }

  vec3 currentPosition = selectedObject->getWorldPosition();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // text
  ImGui::Begin("Welcome to CS300 Assignment 2!");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  // object options
  ImGui::Text("Selected Object");
  bool changedObject = ImGui::SliderInt(objectMgr->getAt(objectIndex)->name.c_str(), &objectIndex, 0, objectMgr->getSize() -1);
  bool changedPosition = ImGui::DragFloat3("World Position", &currentPosition[0]);
  ImGui::Text("Drawing Mode");
  bool changetoWire = ImGui::RadioButton("WireFrame", selectedObject->wiremode);
  bool changeFromWire = ImGui::RadioButton("Shaded", !selectedObject->wiremode);

  // resetting options
  bool recompileShaders = ImGui::Button("Recompile Shaders");
  bool resetCamera = ImGui::Button("Reset Camera");
  bool resetScene = ImGui::Button("Reset Scene");


  ImGui::End();


  // object effects
  if (changedPosition)
  {
    vec3 trans = selectedObject->getWorldPosition() - currentPosition;
    selectedObject->translate(trans);
  }
  if (changetoWire || changeFromWire)
  {
    selectedObject->wiremode = !selectedObject->wiremode;
  }

  // resetting effects
  if (recompileShaders)
  {
    shaderMgr->reCompile(ShaderType::TypeCount);
  }
  if (resetCamera)
  {
    camera->reset();
  }
  if (resetScene)
  {
    SceneShutdown();
    SceneSetup();
    camera->reset();
  }

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


/////***** Main *****/////


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

  // scene setup
  camera =  new Camera(vec3(0, -4, -8), 30.0f, right);
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