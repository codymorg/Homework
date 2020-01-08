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

#pragma once
#ifndef SHADERMANAGEMENT_H
#define SHADERMANAGEMENT_H

#include <string>
#include <vector>

#include "glm//common.hpp"

typedef class Camera;

enum class ShaderType
{
  None,
  Passthrough,
  Reflection,
  Line,
  PhongShading,
  Skybox,

  //***//
  TypeCount
};

class Shader
{
public:
  Shader()
  {};

  Shader(std::string vertexShader, std::string fragShader, ShaderType type);

  ~Shader()
  {};

  void shutdownShader();

  int getProgram();
  void updateWorldToCamTransform(glm::mat4 trans);
  void updateProjectionTransform(glm::mat4 trans);

  int reloadProgram();

protected:

  /******************************************************************************
  brief : compile shader

  input : shaderLocation : file location
          shader         : GL_VERTEX_SHADER or GL_FRAGMENT_SHADER

******************************************************************************/
  int InitShader(std::string shaderLocation, unsigned shader);

  int InitShaderProgram(std::string vertShaderLocation, std::string fragShaderLocation);

  int shaderProgram_ = -1;
  ShaderType type_ = ShaderType::None;
  std::string vertShader_;
  std::string fragShader_;

  int worldToCamLoc_ = -1;
  int projectionLoc_ = -1;
};



class ShaderManager
{
public:
  static ShaderManager* getShaderManager();
  ~ShaderManager();

  Shader addShader(const std::string& vertexShader, const std::string& fragShader, ShaderType shaderType);
  Shader getShader(ShaderType shaderType);
  void updateShaders(Camera& camera);
  unsigned reCompile(ShaderType shaderType);
  unsigned getCurrentBound();
  std::vector<int> getAllShaders();

private:
  ShaderManager();
  static ShaderManager* shaderManager_;

  Shader* compiledShaders_;
};

#endif
