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

#define MAX_LIGHTS 16

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
static vector<Object*> objects;
static vector<Light> lights;
static Camera* camera;
static Camera* reflCamera;

// common vectors
static vec3 up(0, 1, 0);
static vec3 right(1, 0, 0);
static vec3 down(0, -1, 0);
static vec3 left(-1, 0, 0);
static vec3 back(0, 0, -1);
static vec3 forward(0, 0, 1);
static vec3 zero(0, 0, 0);

// gui related
static int selectedObject = 0;
static int selectedModel = 0;
static int selectedLight = 0;
static float selectedColor[3];
static int selectedShader = 0;
static int ballCount = 4;
static int circleRadius = 4;
static int selectedType = 0;
static int selectedSpotAngles[2]; // degrees
static float selectedDirection[2];
static int selectedScene = 0;
static int currentScene = 0;
static int currentProjector = 0;


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

  static bool Wdown = false;
    float moveStr = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && Wdown)
    {
      Wdown = false;
      camera->rotate(90, right);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
      Wdown = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
      objects[FindObject("skybox")]->translate(moveStr * right);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
      objects[FindObject("skybox")]->translate(moveStr * left);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
      objects[FindObject("skybox")]->translate(moveStr * down);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
      objects[FindObject("skybox")]->translate(moveStr * back);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
      objects[FindObject("skybox")]->translate(moveStr * forward);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
      objects[selectedObject]->spin(.5, up);
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


void Render(GLFWwindow* window, Camera& camera)
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw the skybox
  Skybox* skybox = dynamic_cast<Skybox*>(objects[2]);
  if (skybox != nullptr)
  {
    glUseProgram(skybox->shaderProgram_);

    glDisable(GL_DEPTH_TEST);

    int textureNumber = 0;
    for (Texture& thisTexture : skybox->textures)
    {
      glBindTexture(GL_TEXTURE_2D, thisTexture.getTBO());
      glActiveTexture(GL_TEXTURE0 + textureNumber);
      glUniform1i(skybox->hasTextureLoc, 1);
      glUniform1i(thisTexture.texSamplerLoc, textureNumber);
      if (textureNumber >= 2)
      break;
      textureNumber++;
    }
    objects[2]->draw();

    glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
  }
  // lights update
  glUseProgram(lights[0].getShader());
  int t = shaderManager.getCurrentBound();
  GetLightManager()->updateUBO(lights);
  for (Light& light : lights)
  {
    light.update();
  }
  
  camera.update(shaderManager);
  return; //TODO(DEBUG)
  // objects
  for (int i = objects.size()-2; i >= 0; i--)
  {
    Object* object = objects[i];
    glUseProgram(object->shaderProgram_);
    materials->updateUBO(object->material);

    //textured
    if (object->texture.isValid)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, object->texture.getTBO());
      glUniform1i(object->hasTextureLoc, 1);
      glUniform1i(object->texture.texSamplerLoc, 0);
    }

    // reflections 
    else if (object->reflTextures[0].isValid)
    {
      int textureNumber = 0;
      for (Texture& reflTex : object->reflTextures)
      {
        glActiveTexture(GL_TEXTURE0 + textureNumber);
        glBindTexture(GL_TEXTURE_2D, reflTex.getTBO());
        glUniform1i(object->hasTextureLoc, 1);
        glUniform1i(reflTex.texSamplerLoc, textureNumber);
        textureNumber++;
      }
        object->draw();
    }
    // turn off texturing stuff in shaders
    else
    {
      glUniform1i(object->hasTextureLoc, 0);
    }

    object->draw();
    //glUniform1i(object->texture.texSamplerLoc, 0);
    
  }

  glUseProgram(0);
}



// this must be before Render because this generates the textures that render wil then draw
void RenderRefl(GLFWwindow* window)
{
  Object* model = objects[FindObject("model")];
  model->reflTextures[0].isValid = false; // we're setting this so that the textures are not drawn before they're ready
  model->captureReflection();

  //                   bk  dn     ft   lt  rt   up
  float rotation[] = { 0,  90,    180, 90, -90, -90 };
  vec3 axis[] =      { up, right, up,  up, up,  right };


  // generate the textures
  for (size_t i = 0; i < _countof(rotation); i++)
  {
    // move to center of model
    reflCamera->rotate(rotation[i], axis[i]);

    //snap a pic to ith texture
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0 + i);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, model->reflTextures[i].tbo_, 0);
    Render(window, *reflCamera);

    // reset camera
    reflCamera->rotate(-rotation[i], axis[i]);
    reflCamera->reset();
  }
  model->endCapture();


  model->reflTextures[0].isValid = true; // ready for Render
}

void GenerateLightRing()
{
  lights.clear();
  int phongLightingSP = shaderManager.getShader(ShaderType::PhongLighting);
  for (int i = 0; i < ballCount; i++)
  {
    Light light(phongLightingSP, phongLightingSP, "light");
    light.emitter.loadSphere(0.5, 30);
    light.translate(vec3(circleRadius * glm::cos(glm::radians(360.0f / ballCount * i)), 0, circleRadius * glm::sin(glm::radians(360.0f / ballCount * i))));
    light.emitter.orbitRadius = circleRadius;
    light.lightData.emissive = glm::vec4(0.1);
    light.lightData.ambient = glm::vec4(0.1);
    vec3 color;
    color.x = float(i) / ballCount;
    color.y = float(ballCount - i) / ballCount;
    light.lightData.attenuation = vec3(0.1);

    lights.push_back(light);
    lights.back().lightData.number = int(lights.size());
  }

}

void LoadScene0()
{
  GenerateLightRing();
}
void LoadScene1()
{
  for (Light& light : lights)
  {
    vec3 randColor = vec3((rand() % 99 + 1) / 100.0f, (rand() % 99 + 1) / 100.0f, (rand() % 99 + 1) / 100.0f);
    light.setColor(randColor);
    light.lightData.type = 2;
    light.lightData.direction = vec3(0, 0, -1);
  }
}
void LoadScene2()
{
  for (Light& light : lights)
  {
    vec3 randColor = vec3((rand() % 99 + 1) / 100.0f, (rand() % 99 + 1) / 100.0f, (rand() % 99 + 1) / 100.0f);
    light.setColor(randColor);
    light.lightData.type = rand() % 3;
    light.lightData.direction = vec3(0, 0, -1);
  }
}

typedef void(*Scenario)();
Scenario scenes[] =
{
  LoadScene0,
  LoadScene1,
  LoadScene2
};

void GenerateScene(ShaderManager& shaderManager)
{
  int phongLightingSP = shaderManager.getShader(ShaderType::PhongLighting);
  int phongShadingSP = shaderManager.getShader(ShaderType::PhongShading);
  int reflShadingSP = shaderManager.getShader(ShaderType::Reflection);
  
  // generate out God object
  Object* center = new Object(reflShadingSP, "OBJ model");
  center->loadOBJ("Common/models/sphere.obj");
  center->material.diffuse = vec3(.9f);
  center->material.ambient = vec3(6/256.0f);
  center->genFaceNormals();
  center->loadTexture("Common/textures/metalRoofDiff.png", "Common/textures/metalRoof.png");
  //center->initFrameBuffer();
  objects.push_back(center);

  // generate a bunch of lights
  GenerateLightRing();
  
  // keep the orbit tracker
  Object* circle = new Object(phongLightingSP, "circle");
  circle->loadcircle(circleRadius, 45);
  circle->material.diffuse = vec3(1);
  circle->material.ambient = vec3(1);
  objects.push_back(circle);

  // skybox
  Skybox* skybox = new Skybox(shaderManager.getShader(ShaderType::Skybox),"Common/textures/skybox/");
  skybox->material.diffuse = vec3(.9f);
  skybox->material.ambient = vec3(6 / 256.0f);
  skybox->genFaceNormals();
  objects.push_back(skybox);

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

}

void ShutdownScene()
{
  for (Object* obj : objects)
  {
    delete obj;
  }
  lights.clear();
  objects.clear();
  camera->reset();
}

int FindObject(string name)
{
  for (int i = 0; i < objects.size(); i++)
  {
    if (objects[i]->name.find(name) != string::npos)
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

  const char* shaders[] =
  {
    "PhongLighting",
    "PhongShading",
    "PhongBlinn"
  };

  const char* lightType[] =
  {
    "Point",
    "Directional",
    "Spot"
  };

  const char* projectorTypes[] =
  {
	"Cubic",
	"Spherical",
	"Cylindrical",
  };

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Welcome to CS300 Assignment 2!");

  // normal stuff
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("Selected Object");
  ImGui::SliderInt(objects[selectedObject]->name.c_str(), &selectedObject, 0, objects.size() - 1);
  ImGui::SliderInt("Light", &selectedLight, 0, lights.size() - 1);
  ImGui::RadioButton("Scene 1", &selectedScene, 0); ImGui::SameLine();
  ImGui::RadioButton("Scene 2", &selectedScene, 1); ImGui::SameLine();
  ImGui::RadioButton("Scene 3", &selectedScene, 2);


  bool changeModel = ImGui::ListBox("Model Selection", &selectedModel, files, _countof(files));
  bool changeColor = ImGui::ColorEdit3("Selected Object Color", selectedColor, ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB);
  bool changeLightColor = ImGui::ColorEdit3("Selected light Color", selectedColor, ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB);
  bool changeLightCount = ImGui::SliderInt("Light", &ballCount, 1, MAX_LIGHTS);
  bool changeLightType = ImGui::ListBox("Change type of selected light", &selectedType, lightType, _countof(lightType));
  bool changeSpotAngle = ImGui::SliderInt2("Inner and Outer angles of spotlight", selectedSpotAngles, 0, 180);
  bool changeDirectionalLight = ImGui::SliderFloat2("Direction of light for Directional and Spot", selectedDirection, -1, 1, "%.1f");
  //bool changeShader = ImGui::ListBox("Shader Selection", &selectedShader, shaders, _countof(shaders));
  bool recompileShader = ImGui::Button("Recompile Shaders");
  bool changeProjector = ImGui::ListBox("Projector Type Selection", &currentProjector, projectorTypes, _countof(projectorTypes));

  ImGui::End();

  if (currentScene != selectedScene)
  {
    scenes[selectedScene]();
    currentScene = selectedScene;
  }
  if (changeDirectionalLight)
  {
     lights[selectedLight].lightData.direction = vec3(selectedDirection[0], selectedDirection[1], lights[selectedLight].lightData.direction.z);
  }
  if (changeSpotAngle)
  {
    float cosInner = glm::cos(glm::radians<float>(selectedSpotAngles[0]));
    float cosOuter = glm::cos(glm::radians<float>(selectedSpotAngles[1]));
    lights[selectedLight].lightData.spot = glm::vec2(cosInner, cosOuter);
    lights[selectedLight].lightData.type = 2; // spot == 2
  }
  if (changeLightType)
  {
    lights[selectedLight].lightData.type = selectedType;
  }
  if (changeLightCount)
  {
    GenerateLightRing();
  }

  if (recompileShader)
  {
    shaderManager.reCompile(ShaderType::TypeCount);
  }


  if (changeModel || changeProjector)
  {
    string fileName = "Common/models/";
    fileName += files[selectedModel];
    int oID = FindObject("OBJ model");
    objects[oID]->loadOBJ(fileName);

    if (objects[oID]->texture.isValid)
    {
		  Texture::Projector project = Texture::Projector(currentProjector);

      objects[oID]->loadTexture(objects[oID]->texture.getLocation(), objects[oID]->texture.getLocation2(), project);
    }
  }

  if (changeColor)
  {
    objects[selectedObject]->material.ambient = vec3(selectedColor[0], selectedColor[1], selectedColor[2]);
    objects[selectedObject]->material.diffuse = vec3(selectedColor[0], selectedColor[1], selectedColor[2]);

  }
  if (changeLightColor)
  {
    lights[selectedLight].setColor(vec3(selectedColor[0], selectedColor[1], selectedColor[2]));
  }
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
   camera->projection = glm::perspective(glm::radians(45.0f), float(width) / height, 0.1f, 1000.0f);

   int display_w, display_h;
   glfwGetFramebufferSize(window, &display_w, &display_h);
   glViewport(0, 0, display_w, display_h);
}

// Use NVIDIA on laptops
extern "C" {
  _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
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

  //setup all shaders
  int phongLightingSP = shaderManager.addShader("Source/PhongLighting.vert", "Source/PhongLighting.frag", ShaderType::PhongLighting);
  int phongShadingSP = shaderManager.addShader("Source/PhongShading.vert", "Source/PhongShading.frag", ShaderType::PhongShading);
  shaderManager.addShader("Source/skyboxShader.vert", "Source/skyboxShader.frag", ShaderType::Skybox);
  int reflShader = shaderManager.addShader("Source/PhongShading.vert", "Source/skyboxShader.frag", ShaderType::Reflection);

  camera = &Camera(vec3(0, -4, -8), 30.0f, right, phongLightingSP);
  reflCamera = &Camera(zero, 0, right, reflShader);

  GetLightManager()->genUBO(phongLightingSP);
  materials = new MaterialManager;
  materials->genUBO(phongLightingSP);

  GenerateScene(shaderManager);

  while (!glfwWindowShouldClose(window))
  {
    // scene loop
    double time = glfwGetTime();
    ProcessInput(window);
    UpdateGUI();

    //maintain viewport
    glfwSetFramebufferSizeCallback(window, window_size_callback);

    // render scene and GUI window
    UpdateScene(window);
    //RenderRefl(window);
    Render(window, *camera);
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