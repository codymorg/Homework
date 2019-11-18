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

#version 430 core

uniform sampler2D texSampler;
uniform int hasTexture;

in vec3 color;
in vec2 texCoord;

out vec3 fragColor;

void main()
{
  if(hasTexture == 1)
  {
    fragColor = mix(texture( texSampler, texCoord ).rgb,color,0.5);
  }
  else
  {
    fragColor = color;
  }
} 