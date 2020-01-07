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
  addShader("Shaders/Passthrough.vert", "Shaders/normalShader.frag", ShaderType::Passthrough);
}

ShaderManager::~ShaderManager()
{
  for (unsigned i = 0; i < unsigned(ShaderType::TypeCount); i++)
  {
    compiledShaders_[i].shutdownShader();
  }

  delete[] compiledShaders_;
  delete shaderManager_;
  shaderManager_ = nullptr;
}


Shader ShaderManager::addShader(std::string vertexShader, std::string fragShader, ShaderType shaderType)
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

unsigned ShaderManager::reCompile(ShaderType shaderType)
{
  if (shaderType < ShaderType::TypeCount)
    return compiledShaders_[int(shaderType)].reloadProgram();
  else
  {
    for (int i = 0; i < int(ShaderType::TypeCount); i++)
    {
      if (compiledShaders_[i].getProgram() != -1)
        compiledShaders_[i].reloadProgram();
    }
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
