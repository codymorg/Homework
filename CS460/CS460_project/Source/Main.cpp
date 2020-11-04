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
#include "Line.h"
#include "Quaternion.h"
#include "Curve.h"

// managers and static variables
static ObjectManager* objectMgr = nullptr;
static ShaderManager* shaderMgr = nullptr;
static Camera* camera = nullptr;

//proj1
static string currentBone = "No Bone Selected";
static vec3 stepSize = vec3(0.01, 0.1, 0.9);
static bool spaceWasDown = false;
static bool pause = false;

//proj2
static float velocity = 0.05;
static float originalVelocity = 0.05;
static float radius = 2.0;
static float lastRotation = 0;
static float t;
static float easeIn = 0.1f;
static float easeOut = 0.9f;
static double dt = 0;
static Curve curve;
static float rotateBy;

//proj3
static float animationTime = 1.0f; // seconds
static float ikAnimationTime = 0.0f;
static bool allowAnimation = true;
static float animationStep = 0.1;
static int bonelimit = 3;

static const int scene = 3;

// common vectors
glm::vec3 up(0, 1, 0);
glm::vec3 right(1, 0, 0);
glm::vec3 down(0, -1, 0);
glm::vec3 left(-1, 0, 0);
glm::vec3 back(0, 0, -1);
glm::vec3 forward(0, 0, 1);
glm::vec3 zero(0, 0, 0);

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
  *window = glfwCreateWindow(mode->width-10, mode->height, "CS350 Assignment 1 Cody Morgan", NULL, NULL);
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

void BasicScene_Setup()
{
  Object* obj = objectMgr->addObject("Mr. Tiny Hands");
  obj->setShader(ShaderType::Phong);
  obj->loadModel("Common/models/tinyhands.fbx");
  obj->material.diffuse = vec3(0.1, 0.2, 0.3);
  obj->material.ambient = vec3(0.1, 0.1, 0.1);
  obj->setPosition(vec3(0, 0.1f, 0));
  obj->setScale(vec3(0.01f));
  obj->skeleton.createBones(obj->getTransform());
  obj->skeleton.update(obj->getTransform());

  Object* obj2 = objectMgr->addLight("light");
  obj2->setShader(ShaderType::Normal);
  obj2->setPosition((right + up) * 3.0f);
  obj2->scale(vec3(0.2f));
  dynamic_cast<Light*>(obj2)->lightData.ambient = vec4(1);

  Object* floor = objectMgr->addObject("floor");
  floor->setShader(ShaderType::Phong);
  floor->loadBox(vec3(5, 0.1, 5), false);
  floor->setPosition(vec3(0, -0.15, 0));
  floor->material.diffuse = vec3(0.5);
  floor->material.ambient = vec3(0.2);

  // axis and origin markers
  {
    Object* origin = objectMgr->addObject("origin");
    origin->loadSphere(1, 50);
    origin->setScale(vec3(0.1));
    origin->setPosition(vec3(0));
    origin->setShader(ShaderType::Normal);

    Object* xAx = objectMgr->addObject("X-axis");
    xAx->loadSphere(1, 50);
    xAx->setScale(vec3(0.1));
    xAx->setPosition(vec3(5, 0, 0));
    xAx->setShader(ShaderType::Phong);
    xAx->material.diffuse = vec3(1,0,0);

    Object* zAx = objectMgr->addObject("Z-axis");
    zAx->loadSphere(1, 50);
    zAx->setScale(vec3(0.1));
    zAx->setPosition(vec3(0, 0, 5));
    zAx->setShader(ShaderType::Phong);
    zAx->material.diffuse = vec3(0,0,1);
  }
}

void Scene2_Setup()
{
  camera = new Camera(vec3(0, 0, 0), 45.0f, right);
  camera->translate(vec3(0, 0.5, -10));

  auto obj = objectMgr->getFirstObjectByName("Mr. Tiny Hands");
  obj->rotate(90);
  obj->skeleton.animation.setActive("Armature|walk");

  curve.setControlPoints(
  {
    vec3(-2.6, 0.0, -4.3),
    vec3(1.1, 0.0, -2.1),
    vec3(0.2, 0.0, -0.3),
    vec3(2.3, 0.0,  4.3),
    vec3(2.8, 0.0, -2.1),
    vec3(-1.7, 0.0,  0.4),
    vec3(-3.3, 0.0, -0.9),
    vec3(-1.2, 0.0, -2.6),
    vec3(0.9, 0.0, -2.6),
    vec3(0.0, 0.0,  0.7),
  });
  obj->setPosition(curve.evaluate(9));

  objectMgr->selectedObject = 0; // set to model
}

void Scene3_Setup()
{
  camera = new Camera(vec3(0, 0, 0), 0, right);
  camera->translate(vec3(0, -1, -5));

  auto obj = objectMgr->getFirstObjectByName("Mr. Tiny Hands");
  obj->skeleton.animation.setActive("Armature|start", AnimationManager::PlayMode::PlayOnce);
  obj->wiremode = true;

  Object* goal = objectMgr->addObject("goal");
  goal->loadSphere(1, 50);
  goal->setScale(vec3(0.05));
  goal->setPosition(vec3(0,1.5,1.5));
  goal->setShader(ShaderType::Normal);  
  
  //Object* gPrime = objectMgr->addObject("gPrime");
  //gPrime->loadSphere(1, 50);
  //gPrime->setScale(vec3(0.05));
  //gPrime->setPosition(vec3(0.5,1.5,0));
  //gPrime->setShader(ShaderType::Normal);
  //
  //Object* ee = objectMgr->addObject("endEffector");
  //ee->loadSphere(1, 50);
  //ee->setScale(vec3(0.05));
  //ee->setPosition(vec3(0, 0, 0));
  //ee->setShader(ShaderType::Phong);
  //ee->material.diffuse = vec3(0,0,0);  
  //
  //Object* e = objectMgr->addObject("E");
  //e->loadSphere(1, 50);
  //e->setScale(vec3(0.05));
  //e->setPosition(vec3(0, 0, 0));
  //e->setShader(ShaderType::Phong);
  //e->material.diffuse = vec3(1,1,1);
  //
  //Object* O = objectMgr->addObject("O");
  //O->loadSphere(1, 50);
  //O->setScale(vec3(0.05));
  //O->setPosition(vec3(0, 0, 0));
  //O->setShader(ShaderType::Phong);
  //O->material.diffuse = vec3(1, 1, 0);

  objectMgr->selectedObject = 0; // set to model
}

void SceneUpdate()
{
  if(!pause)
    objectMgr->getFirstObjectByName("light")->rotate(-1, left * 10.0f);
}

void Scene2_Update()
{
  float d = velocity * dt;
  auto model = objectMgr->getFirstObjectByName("Mr. Tiny Hands");
  auto lastPos = model->getWorldPosition();
  t = curve.adjustToNearestT(lastPos, d);

  model->setPosition(curve.evaluate(t));

  float thisRotation = curve.getRotation(t);
  rotateBy = thisRotation - lastRotation;
  model->rotate(rotateBy);
  lastRotation = thisRotation;
}

void Scene3_Update()
{
  auto model = objectMgr->getFirstObjectByName("Mr. Tiny Hands");
  ikAnimationTime = glm::clamp(ikAnimationTime + (float)dt, (float)dt, animationTime);
  if(ikAnimationTime <= animationTime)
  {
    auto goal = objectMgr->getFirstObjectByName("goal");
    model->skeleton.runIK(goal->getWorldPosition(), ikAnimationTime / animationTime, bonelimit);
  }
  //auto endef = model->skeleton.findEndEffector(zero);
  //ObjectManager::getObjectManager()->getFirstObjectByName("endEffector")->setPosition(model->modelToWorld(endef->getModelPosition()));  ////DEBUG////

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
    objectMgr->getAt(0)->rotate(1, vec3(0), vec3(0, 1, 0));
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    objectMgr->getAt(0)->rotate(-1, vec3(0), vec3(0, 1, 0));


  
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && spaceWasDown)
  {
    spaceWasDown = false;
    pause = !pause;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    spaceWasDown = true;

}

int GetBoneParentsNames(const Bone* inbone, string* names)
{
  auto bone = inbone;
  *names = "";
  int count = 0;

  while (bone && count < bonelimit)
  {
    *names += bone->name + "->";
    bone = bone->parent;
    count++;
  }

  return count;
}

int GetChainCount(const Bone* inbone)
{
  auto bone = inbone;
  int count = 0;

  while (bone)
  {
    bone = bone->parent;
    count++;
  }

  return count;
}

void UpdateGUI()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Object Controls");
  ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
  auto currentObj = objectMgr->getSelected();
  auto model = objectMgr->getFirstObjectByName("Mr. Tiny Hands");

  if (ImGui::BeginTabBar("MyTabBar"))
  {
    // Model TAB
    if (ImGui::BeginTabItem("Model Controls"))
    {
      ImGui::Text(std::string("Currently Selected: " + currentObj->name).c_str());
      
      auto currentPos = currentObj->getWorldPosition();
      if (ImGui::SliderFloat("X", &currentPos.x, currentPos.x - stepSize.y, currentPos.x + stepSize.y))
      {
        currentObj->setPosition(currentPos);
      }
      if (ImGui::SliderFloat("Y", &currentPos.y, currentPos.y - stepSize.y, currentPos.y + stepSize.y))
      {
        currentObj->setPosition(currentPos);
      }
      if (ImGui::SliderFloat("Z", &currentPos.z, currentPos.z - stepSize.y, currentPos.z + stepSize.y))
      {
        currentObj->setPosition(currentPos);
      }

      auto scale = currentObj->getWorldScale();
      ImGui::Text(("Scale: [" + std::to_string(scale.x) + ", " + std::to_string(scale.y) + ", " + std::to_string(scale.z) + "]").c_str());
      ImGui::SliderFloat("Step Size", &stepSize.y, stepSize.x, stepSize.z);
      if(ImGui::Button("Scale Up"))
      {
        currentObj->scale(vec3(1+stepSize.y, 1+stepSize.y, 1+stepSize.y));
      }
      if (ImGui::Button("Scale Down"))
      {
        currentObj->scale(vec3(1-stepSize.y, 1-stepSize.y, 1-stepSize.y));
      }

      if (ImGui::Button("Recompie Shaders"))
      {
        shaderMgr->reCompile();
      }

      if(ImGui::Button("Toggle Wireframe"))
      {
        currentObj->wiremode = !currentObj->wiremode;
      }

      if (ImGui::BeginMenu("Select Object"))
      {
        int i = -1;
        for (auto b : objectMgr->getObjectNames())
        {
          i++;
          // don't show bones
          if (string(b).find("Line") != -1)
            continue;
          bool wasClicked = ImGui::MenuItem(b);
          if (wasClicked)
          {
            objectMgr->selectedObject = i;
          }
        }
        ImGui::EndMenu();
      }

      ImGui::EndTabItem();
    }
    
    // Bone TAB
    if (ImGui::BeginTabItem("Bone Controls"))
    {
      auto boneNames = model->skeleton.getBoneNames();
      if (ImGui::BeginMenu(currentBone.c_str()))
      {
        ImGui::Text(currentBone.c_str());
        for (auto b : boneNames)
        {
          bool wasClicked = ImGui::MenuItem(b.c_str());
          if (wasClicked)
          {
            currentBone = b;
          }
        }
        ImGui::EndMenu();
      }

      Bone* bone;
      if (model->skeleton.getBone(currentBone, &bone))
      {
        auto currentPos = bone->getBonePosition();
        if (ImGui::SliderFloat("X", &currentPos.x, currentPos.x - stepSize.y, currentPos.x + stepSize.y))
        {
          bone->setBonePosition(currentPos);
        }
        if (ImGui::SliderFloat("Y", &currentPos.y, currentPos.y - stepSize.y, currentPos.y + stepSize.y))
        {
          bone->setBonePosition(currentPos);
        }
        if (ImGui::SliderFloat("Z", &currentPos.z, currentPos.z - stepSize.y, currentPos.z + stepSize.y))
        {
          bone->setBonePosition(currentPos);
        }

        auto trans = bone->transform;
        auto btw =   bone->boneToModel;
        auto off = bone->offsetMatrix;
        auto skin = bone->skinTransform;
        glm::mat4x4 collection[4] = 
        {
          trans,
          btw,
          off,
          skin
        };
        string mats ="Transform                                              Bone To Model\n";
        for(int nextSet = 0; nextSet <= 2; nextSet+=2)
        {
          for(int matrow = 0; matrow < 4; matrow++)
          {
            for(int mat = 0; mat < 2; mat++)
            {
              mats+="[";
              for(int col = 0; col < 4; col++)
              {
                float num = collection[mat + nextSet][col][matrow];
                if(num >= 10)
                  mats+=" ";
                else if(num < 10 && num >= 0)
                  mats+="  ";
                else if(num < 0 && num > -9)
                  mats +=" ";
              

                mats += std::to_string(num) + " ";
              }
              mats += "] ";

            }
            mats+="\n";
          }
          
          if(nextSet == 0)
            mats+="\n\nOffset                                         Skin\n";
        }
        ImGui::Text(mats.c_str());
      }

      ImGui::EndTabItem();
    }

    // Animation TAB
    if (ImGui::BeginTabItem("Animation Controls"))
    {
      auto names = model->skeleton.animation.getAnimationNames();
      auto currentName = model->skeleton.animation.getCurrentAnimationName();
      ImGui::SliderFloat("Animation Speed", &model->skeleton.animation.animationSpeed, 0, 1);
      ImGui::SliderFloat("Velocity", &velocity, 0, 0.1f, "%.4f");
      
      ImGui::SliderFloat("Ease In", &easeIn, 0, 1.0f);
      ImGui::SliderFloat("Ease Out", &easeOut, 0, 1.0f);
      
      string mode = "Curve";
      float range = curve.t_max - curve.t_min;
      float t0 = curve.t_min;
      float t1 = curve.dimensions() + (easeIn * range);
      float t2 = curve.dimensions() + (easeOut * range);
      float t3 = curve.t_max;
      
      if (t < t1)
      {
        mode = "EaseIn";
        velocity = t * (originalVelocity / t1);
        model->skeleton.animation.animationSpeed = velocity / originalVelocity;
      }
      else if (t > t2)
      {
        mode = "EaseOut";
        velocity = (t3 - t) * (originalVelocity / (t3 - t2));
        model->skeleton.animation.animationSpeed = velocity / originalVelocity;
      }
      else
      {
        velocity = originalVelocity;
        if (std::abs(rotateBy) <= 2)
        {
          model->skeleton.animation.animationSpeed = std::max(0.8f, 1.0f - std::abs(rotateBy / 4));
        }
      }
      ImGui::Text("[%s] t-value: %f", mode.c_str(), t);
      ImGui::Text("\n\n");


      if (ImGui::BeginMenu(currentName.empty() ? "Select an Animation..." : currentName.c_str()))
      {
        ImGui::Text("Animation List...");
        for (auto b : names)
        {
          bool wasClicked = ImGui::MenuItem(b.c_str());
          if (wasClicked)
          {
            model->skeleton.animation.setActive(b);
          }
        }
        ImGui::EndMenu();
      }


      ImGui::EndTabItem();
    }
    
    // IK TAB
    if (ImGui::BeginTabItem("IK Controls"))
    {
      auto goal = objectMgr->getFirstObjectByName("goal");
      auto endeffector = model->skeleton.findEndEffector();

      ImGui::SliderFloat("Animation Time", &animationTime, 0.5, 5);

      auto currentPos = goal->getWorldPosition();
      {
        if (ImGui::SliderFloat("X", &currentPos.x, currentPos.x - stepSize.y, currentPos.x + stepSize.y))
        {
          goal->setPosition(currentPos);
        }
        if (ImGui::SliderFloat("Y", &currentPos.y, currentPos.y - stepSize.y, currentPos.y + stepSize.y))
        {
          goal->setPosition(currentPos);
        }
        if (ImGui::SliderFloat("Z", &currentPos.z, currentPos.z - stepSize.y, currentPos.z + stepSize.y))
        {
          goal->setPosition(currentPos);
        }
      }

      string boneNames;
      auto count = GetBoneParentsNames(model->skeleton.findEndEffector(), &boneNames);

      ImGui::Text(boneNames.c_str());
      ImGui::SliderInt("Depth limit", &bonelimit,0, GetChainCount(model->skeleton.findEndEffector()));
      ImGui::SliderFloat("Animation t value", &ikAnimationTime, animationStep, animationTime);
      if(ImGui::Button("Start Animation"))
      {
        ikAnimationTime = 0.0;
        model->skeleton.resetIK();
      }

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
  QuaternionTest(100);

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
  BasicScene_Setup();
  scene == 2 ? Scene2_Setup() : Scene3_Setup();

  auto model = objectMgr->getFirstObjectByName("Mr. Tiny Hands");
  double time = 0;
  while (!glfwWindowShouldClose(window))
  {
    // scene loop
    dt = (glfwGetTime() - time);
    if(dt > 1)
      dt = 1/40.0f;
    objectMgr->dt = dt;

    time = glfwGetTime();

    // sim loop
    SceneUpdate();
    scene == 2 ? Scene2_Update() : Scene3_Update();

    UpdateGUI();
    ProcessInput(window);

    // end of the loop
    objectMgr->render(*camera);
    GUIendFrame(window, time);
  }

  Terminate();

  return 0;
}