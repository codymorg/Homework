/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

Purpose :   manages shader objects
Language:   C++ Visual Studio
Platform:   Windows 10
Project :   cody.morgan_CS300_1
Author  :   Cody Morgan  ID: 180001017
Date  :   4 OCT 2019
End Header --------------------------------------------------------*/

#include "ShaderManager.h"
#include "Camera.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
  using std::cout;
#include <fstream>
  using std::ifstream;
#include <string>
  using std::string;

ShaderManager* ShaderManager::shaderManager_ = nullptr;


Shader::Shader(std::string vertexShader, std::string fragShader, ShaderType type) :
    vertShader_(vertexShader), fragShader_(fragShader), type_(type)
{
  shaderProgram_ = InitShaderProgram(vertexShader, fragShader);
};


void Shader::shutdownShader()
{
  glUseProgram(0);
  glDeleteProgram(shaderProgram_);
  shaderProgram_ = -1;
}

int Shader::getProgram()
{
  return shaderProgram_;
}

ShaderType Shader::getShaderType()
{
  return type_;
}

// update this shader's camera matrix
void Shader::updateWorldToCamTransform(glm::mat4 trans)
{
  glUseProgram(shaderProgram_);

  worldToCamLoc_ = glGetUniformLocation(shaderProgram_, "worldToCam");
  if(worldToCamLoc_)
    glUniformMatrix4fv(worldToCamLoc_, 1, GL_FALSE, &trans[0][0]);

  glUseProgram(0);
}

void Shader::updateProjectionTransform(glm::mat4 trans)
{
  glUseProgram(shaderProgram_);

  projectionLoc_ = glGetUniformLocation(shaderProgram_, "projection");
  if(projectionLoc_ != -1)
    glUniformMatrix4fv(projectionLoc_, 1, GL_FALSE, &trans[0][0]);

  glUseProgram(0);
}

void Shader::updateDisplayMode(int value)
{
  glUseProgram(shaderProgram_);

  displayModeLoc_ = glGetUniformLocation(shaderProgram_, "displayMode");
  if (displayModeLoc_ != -1)
    glUniform1i(displayModeLoc_, value);

  glUseProgram(0);
}

int Shader::reloadProgram()
{
  glUseProgram(0);
  glDeleteProgram(shaderProgram_);
  shaderProgram_ = InitShaderProgram(vertShader_, fragShader_);

  return shaderProgram_;
};


int Shader::InitShader(string shaderLocation, unsigned shader)
{
  // Read the Vertex Shader code from the file
  string shaderCode;
  ifstream stream(shaderLocation, std::ios::in);
  if (stream.is_open())
  {
    string Line;
    while (getline(stream, Line))
    {
      shaderCode += "\n" + Line;
    }
    stream.close();
  }
  else
  {
    printf("Impossible to open << %s >>. Are you in the right directory ? Don't forget to read the FAQ !\n", shaderLocation.c_str());
    return getchar();
  }

  int shaderNum;
  shaderNum = glCreateShader(shader);
  const GLchar* vertShader = shaderCode.c_str();
  glShaderSource(shaderNum, 1, &vertShader, NULL);
  glCompileShader(shaderNum);

  // check it's ok
  int  success;
  char infoLog[512];
  glGetShaderiv(shaderNum, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shaderNum, 512, NULL, infoLog);
    if (shader == GL_VERTEX_SHADER)
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    else
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

    assert(success != 0);
    return -1;
  }

  return shaderNum;
};

int Shader::InitShaderProgram(string vertShaderLocation, string fragShaderLocation)
  {
    unsigned vertexShader = InitShader(vertShaderLocation, GL_VERTEX_SHADER);
    unsigned fragmentShader = InitShader(fragShaderLocation, GL_FRAGMENT_SHADER);

    unsigned shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check it's ok
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::FAILED\n" << infoLog << std::endl;

      assert(success != 0);
      return -1;
    }

    // cleanup in betweens
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
  };




ShaderManager::ShaderManager()
{
  compiledShaders_ = new Shader[int(ShaderType::TypeCount)];

  // create all the shaders here
  addShader("Shaders/normal.vert", "Shaders/normal.frag", ShaderType::Normal);
  addShader("Shaders/phong.vert", "Shaders/phong.frag", ShaderType::Phong);
  addShader("Shaders/deferred.vert", "Shaders/deferred.frag", ShaderType::Deferred);
  addShader("Shaders/deferredLighting.vert", "Shaders/deferredLighting.frag", ShaderType::DeferredLighting);
}

ShaderManager::~ShaderManager()
{
  for (unsigned i = 0; i < unsigned(ShaderType::TypeCount); i++)
  {
    compiledShaders_[i].shutdownShader();
  }

  delete shaderManager_;
  shaderManager_ = nullptr;
}


Shader& ShaderManager::addShader(const std::string& vertexShader, const std::string& fragShader, ShaderType shaderType)
{
  // you are changing the shader for this type
  if (compiledShaders_[int(shaderType)].getProgram() != -1)
  {
    compiledShaders_[int(shaderType)].~Shader();
    cout << "WARNING: changing shader for shader type, did you mean to do this?\n";
  }

  // make a new shader program
  compiledShaders_[int(shaderType)] = Shader(vertexShader, fragShader, shaderType);

  return compiledShaders_[int(shaderType)];
}

Shader ShaderManager::getShader(ShaderType shaderType)
{
  return compiledShaders_[int(shaderType)];
}

void ShaderManager::updateShaders(Camera& camera)
{
  for (int i = 0; i < int(ShaderType::TypeCount); i++)
  {
    if (compiledShaders_[i].getProgram() != -1)
    {
      compiledShaders_[i].updateWorldToCamTransform(camera.worldToCam);
      compiledShaders_[i].updateProjectionTransform(camera.projection);
    }
  }
}

void ShaderManager::reCompile(ShaderType shaderType)
{
  for (int i = 0; i < int(ShaderType::TypeCount); i++)
  {
    if (compiledShaders_[i].getProgram() != -1)
      compiledShaders_[i].reloadProgram();
  }
}

unsigned ShaderManager::getCurrentBound()
{
  int id;
  glGetIntegerv(GL_CURRENT_PROGRAM, &id);
  assert(id >= 0);

  return unsigned(id);
}

std::vector<int> ShaderManager::getAllShaders()
{
  std::vector<int> allShaders;
  for (int i = 0; i < int(ShaderType::TypeCount); i++)
  {
    int shader = compiledShaders_[i].getProgram();
    if (shader >= 0)
      allShaders.push_back(compiledShaders_[i].getProgram());
  }

  return allShaders;
}

ShaderManager* ShaderManager::getShaderManager()
{
  if(!shaderManager_)
    shaderManager_ = new ShaderManager();

  return shaderManager_;
}
