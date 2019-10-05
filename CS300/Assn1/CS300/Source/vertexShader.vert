/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

Purpose :   Instructions on how to use this software
Language:   C++ Visual Studio
Platform:   Windows 10
Project :   cody.morgan_CS300_1
Author  :   Cody Morgan  ID: 180001017
Date    :   4 OCT 2019
End Header --------------------------------------------------------*/ 

#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 fragPos;
out vec3 normal;

void main()
{
  fragPos = vec3(model * vec4(aPos, 1.0));
  normal = mat3(transpose(inverse(model))) * aNormal;

  gl_Position = projection * view * model * vec4(aPos,1.0f);

}