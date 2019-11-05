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

#ifndef SHADERMANAGEMENT_H
#define SHADERMANAGEMENT_H
#pragma once

#include <string>
#include <vector>

class Shader;
enum class ShaderType
  {
    PhongLighting,
    Diffuse,
    Reflection,
    Line,
    PhongShading,

    //***//
    TypeCount
  };
class ShaderManager
{
public:

  ShaderManager();
  ~ShaderManager();

  unsigned addShader(std::string vertexShader, std::string fragShader, ShaderType shaderType);
  unsigned getShader(ShaderType shaderType);
  unsigned reCompile(ShaderType shaderType);
  unsigned getCurrentBound();
  std::vector<int> getAllShaders();

private:
  Shader* compiledShaders_;
};

#endif
