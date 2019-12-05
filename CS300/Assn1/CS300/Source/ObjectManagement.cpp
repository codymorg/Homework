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

#include "ObjectManagement.h"
using glm::vec3;
using glm::vec4;
using glm::mat4;

using std::string;
using std::getline;

#include <vector>
using std::vector;

#include <fstream>
using std::ifstream;

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;

#include <algorithm>
using std::max;
using std::min;

#include "../Common/SOIL/SOIL.h"

vec3 red(1, 0, 0);


/////***** Object Material Management *****/////


void MaterialManager::genUBO(unsigned shaderProgram)
{
  glUseProgram(shaderProgram);
  glGenBuffers(1, &ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialData), nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_);
}

void MaterialManager::updateUBO(MaterialData& material)
{

  void* uboBuffer;
  //glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  uboBuffer = glMapNamedBuffer(ubo_, GL_WRITE_ONLY);

  memcpy(uboBuffer, &material, sizeof(MaterialData));

  glUnmapNamedBuffer(ubo_);
  //glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

/////***** Vertex Class *****/////

Vertex::Vertex(float x, float y, float z, float nx, float ny, float nz)
{
  *this = Vertex(vec3(x,y,z), vec3(nx, ny, nz));
}

Vertex::Vertex(vec3 pos, vec3 norm, glm::vec2 uv ) : position(pos), normal(norm), uv(uv)
{
}




/////***** Line Class *****/////


Line::Line(vec3 pointA, vec3 pointB) : centroid(pointA), normDir(pointB)
{}



/////***** Object Class *****/////


Object::Object(int shaderPgm, string ID) : name(ID), shaderProgram_(shaderPgm)
{
  transform = glm::mat4(1.0f);

  // init openGL buffers
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  initBuffers();
  hasTextureLoc = glGetUniformLocation(shaderProgram_, "hasTexture");
  useGPUuvLoc = glGetUniformLocation(shaderProgram_, "useGPUuv");
  lowerLoc = glGetUniformLocation(shaderProgram_, "lower");
  upperLoc = glGetUniformLocation(shaderProgram_, "upper");
  projectionLoc = glGetUniformLocation(shaderProgram_, "projectionType");
  glUniform1i(hasTextureLoc, 0);
}

Object::~Object()
{
}

void Object::translate(glm::vec3 translation)
{
  static vec3 total = vec3(0);
  vec3 trans = translation / modelScale;
  transform = glm::translate(transform, trans);
  total += trans;
}

void Object::spin(float degrees, glm::vec3 axis)
{
  // safeguard rotation
  while (degrees > 360.0f)
    degrees -= 360.0f;

  transform = glm::rotate(transform, glm::radians(degrees), axis);
}

void Object::addScale(glm::vec3 scale)
{
  transform = glm::scale(transform, scale);
  modelScale *= scale.x;
}



void Object::printTransform()
{
  for (size_t r = 0; r < 4; r++)
  {
    for (size_t c = 0; c < 4; c++)
    {
      std::cout.setf(std::ios::fixed, std::ios::floatfield);
      std::cout.precision(1);
      cout << transform[c][r] << "\t";
    }
    cout << "\n";
  }
  cout << '\n';
}

void PrintMatrix(mat4 mat)
{
  for (size_t r = 0; r < 4; r++)
  {
    for (size_t c = 0; c < 4; c++)
    {
      std::cout.setf(std::ios::fixed, std::ios::floatfield);
      std::cout.precision(1);
      cout << mat[c][r] << "\t";
    }
    cout << "\n";
  }
  cout << '\n';
}

void Object::rotateY(float degrees, float radius)
{
  glm::vec3 pos(transform[3][0], transform[3][1], transform[3][2]);
  if (pos.x > radius)
    transform[3][0] = radius;

  // rotate verts
  mat4 rotM(1.0f);
  rotM = glm::translate(rotM, -pos);
  rotM = glm::rotate(rotM, glm::radians(degrees), vec3(0,1,0));
  rotM = glm::translate(rotM, pos);
  transform = rotM * transform;
}

void Object::initLineBuffers()
{
  // init openGL buffers
  glGenVertexArrays(1, &lineVAO);
  glGenBuffers(1, &lineVBO);

  // bind with default data
  glBindVertexArray(lineVAO);
  glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
  glBufferData(GL_ARRAY_BUFFER, faceNorms.size() * sizeof(vec3), faceNorms.data(), GL_DYNAMIC_DRAW);

  // send vert info
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), nullptr);

  // unbind
  glBindVertexArray(0);

}

void Object::genVertexNormals()
{
  // ensure the face normals are generated
  faceNormDrawingMode = InvalidDrawMode;
  genFaceNormals();

  // average the face normals for vertex normal
  for (int i = 0; i < faceNorms.size() / 2; i++)
  {
    // apply the normal that affects these vertices
    vec3 norm = faceNorms[(2 * i + 1)] - faceNorms[2 * i];
    for (int j = 0; j < 3; j++)
    {
      vertices[indices[(3 * i) + j]].normal += norm;
    }
  }

  // normalize normals
  faceNorms.clear();
  for (int i = 0; i < vertices.size(); i++)
  {
    vertices[i].normal = glm::normalize(vertices[i].normal);
    faceNorms.push_back(vertices[i].position);
    faceNorms.push_back(vertices[i].position + (vectorScale * glm::normalize(vertices[i].normal) / modelScale));
  }
  initLineBuffers();

}

void Object::drawVertexNormals()
{
  if (vertexNormalDrawingMode == InvalidDrawMode)
  {
    genVertexNormals();
    toggleFaceNormals(true, false);
    faceNorms.clear();

    for (int i = 0; i < vertices.size(); i++)
    {
      faceNorms.push_back(vertices[i].position);
      faceNorms.push_back(vertices[i].position + vertices[i].normal);
    }

    initLineBuffers();
    vertexNormalDrawingMode = VertexNormalOn;
  }

  if (vertexNormalDrawingMode == VertexNormalOn)
  {

    glBindVertexArray(lineVAO);
    vec3 pink(1.0, 0.078, 0.576);
    glUniform3fv(vectorColorLoc, 1, glm::value_ptr(pink));

    glDrawArrays(GL_LINES, 0, faceNorms.size());

    glBindVertexArray(0);
  }
}



void Object::genFaceNormals()
{
  if (faceNormDrawingMode == FaceNormalDrawingMode::InvalidDrawMode)
  {
    faceNorms.clear();

    for (int i = 0; i < indices.size(); i += 3)
    {
      const vec3& A = vertices[indices[i]].position;
      const vec3& B = vertices[indices[i + 1]].position;
      const vec3& C = vertices[indices[i + 2]].position;

      vec3 center = 
      {
        (A.x + B.x + C.x) / 3,
        (A.y + B.y + C.y) / 3,
        (A.z + B.z + C.z) / 3,
      };

      vec3 vectorAB = vec3(B - A);
      vec3 vectorBC = vec3(C - B);
      vec3 norm = glm::normalize(glm::cross(vectorAB, vectorBC)) * vectorScale / modelScale;
      norm = center + norm;
      faceNorms.push_back(center);
      faceNorms.push_back(norm);
    }

    initLineBuffers();
  }
}

void Object::toggleFaceNormals(bool useHardSet, bool setToThis)
{
  if ((useHardSet && setToThis))
  {
    vertexNormalDrawingMode = VertexNormalOff;
    faceNormDrawingMode = FaceNormalDrawingMode::InvalidDrawMode;
    genFaceNormals();
    faceNormDrawingMode = FaceNormalDrawingMode::FaceNormalsOn;
  }
  else if ((useHardSet && !setToThis) || faceNormDrawingMode == FaceNormalDrawingMode::FaceNormalsOn)
  {
    faceNormDrawingMode = FaceNormalDrawingMode::FaceNormalsOff;
  }
  else
  {
    vertexNormalDrawingMode = VertexNormalOff;
    faceNormDrawingMode = FaceNormalDrawingMode::InvalidDrawMode;
    genFaceNormals();
    faceNormDrawingMode = FaceNormalDrawingMode::FaceNormalsOn;
  }
}

void Object::toggleVertexNormals(bool useHardSet, bool setToThis)
{
  if ((useHardSet && setToThis))
  {
    vertexNormalDrawingMode = VertexNormalOn;
    genVertexNormals();
    faceNormDrawingMode = FaceNormalDrawingMode::FaceNormalsOff;
  }
  else if ((useHardSet && !setToThis) || vertexNormalDrawingMode == VertexNormalOn)
  {
    vertexNormalDrawingMode = VertexNormalOff;
  }
  else
  {
    vertexNormalDrawingMode = VertexNormalOn;
    genVertexNormals();
    faceNormDrawingMode = FaceNormalDrawingMode::FaceNormalsOff;
  }
}

void Object::setShader(int shaderProgram)
{
  shaderProgram_ = shaderProgram;
  glUseProgram(shaderProgram);
  modelLoc = glGetUniformLocation(shaderProgram_, "model");

  //glUseProgram(lineShader);
  
  vectorColorLoc = glGetUniformLocation(shaderProgram_, "objColor");

  //glUseProgram(shaderProgram);
}


void Object::initFrameBuffer()
{
  // gen fbo
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // generate rbo
  int width  = 512;
  int height = 512;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

  // set up textures
  hasTextureLoc = glGetUniformLocation(shaderProgram_, "hasTexture");
  for (int i = 0; i < 6; i++)
  {
    // init texture
    glActiveTexture(GL_TEXTURE0 + i);
    glGenTextures(1, &reflTextures[i].tbo_);
    glBindTexture(GL_TEXTURE_2D, reflTextures[i].tbo_);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    string texName = "texSampler" + std::to_string(i);
    reflTextures[i].texSamplerLoc = glGetUniformLocation(shaderProgram_, texName.c_str());
    glUniform1i(reflTextures[i].texSamplerLoc, i);
    glUniform1i(hasTextureLoc, 1);

    reflTextures[i].isValid = true;

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // attach
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflTextures[0].tbo_, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

  //check status
  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    cout << "error FBO is not complete\n";

  // unbind everything
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Object::captureReflection()
{
  //glFramebufferTexture2D
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glPushAttrib(GL_VIEWPORT_BIT);
  double viewport[4];
  glGetDoublev(GL_VIEWPORT, viewport);
  glViewport(0, 0, int(viewport[2]), int(viewport[3]));
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0, 0, 1);
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  GLenum buffers[] =
  {
    GL_COLOR_ATTACHMENT0,
  };

  glDrawBuffers(1, buffers);

}

void Object::endCapture()
{
  //turn off fbo capture
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glPopAttrib();
}


void Object::drawFaceNormals()
{
  glBindVertexArray(lineVAO);
  glUniform3fv(vectorColorLoc, 1, glm::value_ptr(red));

  glDrawArrays(GL_LINES, 0, faceNorms.size());

  glBindVertexArray(0);
}

void Object::loadCube()
{
  vertices =
  {
    Vertex(-0.5f, -0.5f,  0.5f),
    Vertex( 0.5f, -0.5f,  0.5f),
    Vertex( 0.5f,  0.5f,  0.5f),
    Vertex(-0.5f,  0.5f,  0.5f),
    Vertex(-0.5f, -0.5f, -0.5f),
    Vertex( 0.5f, -0.5f, -0.5f),
    Vertex( 0.5f,  0.5f, -0.5f),
    Vertex(-0.5f,  0.5f, -0.5f)
  };

  indices =
  {
    0,1,2, // Front
    0,2,3,
    0,5,1, // Bottom
    0,4,5,
    1,5,6, // Right
    1,6,2,
    4,0,3, // Left
    4,3,7,
    4,7,6, // Back
    4,6,5,
    3,2,6, // Top
    3,6,7
  };

  initBuffers();
}

void Object::loadOBJ(string fileLocation)
{
  ifstream file(fileLocation);
  vector<float>verts;

  maxPos = vec3(INT_MIN, INT_MIN, INT_MIN);
  minPos = vec3(INT_MAX, INT_MAX, INT_MAX);
  vectorScale = 1;
  transform = glm::mat4();


  // load vert data and normal data
  if (file)
  {
    // since the file is good - clear the old vertex data if any
    vertices.clear();
    indices.clear();

    string line;
    while(getline(file,line))
    {
      if (line.substr(0, 2) == "v " || line.substr(0, 2) == "v\t")
      {
        std::istringstream objData(line.substr(2));
        float vertexData;
        for (size_t i = 0; i < 3; i++)
        {
          objData >> vertexData;
          verts.push_back(vertexData);
          minPos[i] = min(minPos[i], vertexData);
          maxPos[i] = max(maxPos[i], vertexData);
        }
      }
      else if (line.substr(0, 2) == "f ")
      {
        std::istringstream objData(line.substr(2));
        std::string token;
        int faceCount = 0;
        vector<unsigned> fanIndices;

        // if you have '//' delimiters you monster
        if (line.find("//") != string::npos)
        {
          while (getline(objData, token, '/'))
          {
            int index;
            std::istringstream substring(token);
            substring >> index;

            // add this face to the fan list if you have more than 3 faces
            fanIndices.push_back(index - 1);
            indices.push_back(index - 1);

            faceCount++;

            // skip uv stuff
            objData.get();
            char peek = '\0';
            while (peek != ' ' && peek != -1)
            {
              peek = objData.peek();
              objData.get();
            }
          }
        }

        // if you have '\\' delimiters
        else if (line.find("\\") != string::npos && line.find("\\par") == string::npos)
        {
          while (getline(objData, token, '\\'))
          {
            int index;
            std::istringstream substring(token);
            substring >> index;

            fanIndices.push_back(index - 1);
            faceCount++;

            char peek = '\0';
            while (peek != ' ' && peek != -1)
            {
              peek = objData.peek();
              objData.get();
            }
            indices.push_back(index - 1);
          }
        }

        // if you have ' ' delimitation like a normal person
        else
        {
          while (getline(objData, token, ' '))
          {
            int index;
            std::istringstream substring(token);
            substring >> index;
            indices.push_back(index - 1);
            fanIndices.push_back(index - 1);

            faceCount++;

          }
        }

        if (faceCount > 3)
        {
          // shear off those fan faces
          while (faceCount > 3)
          {
            indices.pop_back();
            faceCount--;
          }

          // convert fan to gl tris
          unsigned triA = fanIndices[0];
          for (int i = 0; i < fanIndices.size() - 3; i++)
          {
            unsigned triB = fanIndices[2 + i];
            unsigned triC = fanIndices[3 + i];
            indices.push_back(triA);
            indices.push_back(triB);
            indices.push_back(triC);
          }
        }

      }
    }

    // load data into vertex struct

    for (size_t i = 0; i < verts.size(); i += 3)
    {
      vec3 normalData(0, 0, 0);
      vec3 vertData(verts[i], verts[i + 1], verts[i + 2]);
      vertices.push_back(Vertex(vertData, normalData));
    }
    vec3 centroid = { (maxPos.x + minPos.x) / 2, (maxPos.y + minPos.y) / 2, (maxPos.z + minPos.z) / 2 };
    modelScale = 2 / glm::length(maxPos - centroid);
    addScale(vec3(modelScale, modelScale, modelScale));

    genVertexNormals();

    vec3 translation((maxPos.x + minPos.x) / 2, (maxPos.y + minPos.y) / 2, (maxPos.z + minPos.z) / 2);
    transform = glm::translate(transform, -translation);

    initBuffers();
  }
  else
  {
    cout << "error opening file: " << fileLocation << "\n";
  }


}

void Object::loadSphere(float radius, int divisions)
{
	vertices.clear();
	indices.clear();
	vec3 position;
	vec3 norm;
	float pi = glm::pi<float>();
	float sectorStep = 2.0f * pi / divisions;
	float stackStep = pi / divisions;
	float theta = 0;
	float phi = 0;
	float length = 1.0f / radius;

	// generate vertices
	for (int i = 0; i <= divisions; ++i)
	{
		theta = pi / 2 - i * stackStep;    
		float xy = radius * cosf(theta);       
		position.z = radius * sinf(theta);      

		for (int j = 0; j <= divisions; ++j)
		{
			phi = j * sectorStep;    
			position.x = xy * cosf(phi);       
			position.y = xy * sinf(phi);    

			// normalized vertex normal (nx, ny, nz)
			norm.x = position.x / radius;
			norm.y = position.y / radius;
			norm.z = position.z / radius;

			vertices.push_back(Vertex(position, norm));
		}
	}

	// generate indices
	unsigned k1;
	unsigned k2;
	for (int i = 0; i < divisions; ++i)
	{
		k1 = i * (divisions + 1);   // beginning of current stack
		k2 = k1 + divisions + 1;    // beginning of next stack

		for (int j = 0; j < divisions; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (divisions - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	initBuffers();
}

void Object::loadcircle(float radius, int divisions)
{
	vertices.clear();
	indices.clear();
  renderMode = GL_LINE_LOOP;

	// generate vertices
	for (int i = 0; i < divisions; i++)
	{
		vertices.push_back(Vertex(vec3(radius * glm::sin(glm::radians(360.0f / divisions * i)), 0, radius * glm::cos(glm::radians(360.0f / divisions * i))),vec3(0,1,0)));
    indices.push_back(i);
	}

	initBuffers();
}

void Object::loadPlane()
{
  vertices =
  {
    Vertex(vec3(-0.5f, 0.0f, -0.5f),vec3(0,1,0),glm::vec2(0,0)),
    Vertex(vec3( 0.5f, 0.0f, -0.5f),vec3(0,1,0),glm::vec2(1,0)),
    Vertex(vec3( 0.5f, 0.0f,  0.5f),vec3(0,1,0),glm::vec2(1,1)),
    Vertex(vec3(-0.5f, 0.0f,  0.5f),vec3(0,1,0),glm::vec2(0,1)),
  };

  indices =
  {
    //0,1,2, 
    //0,2,3,
    0,3,2,
    0,2,1
  };

  minPos = vec3(-0.5f, 0.0f, -0.5f);
  maxPos = vec3(0.5f, 0.0f, 0.5f);

  initBuffers();
}

void Object::loadTexture(std::string location, string location2, Texture::Projector projector)
{
  texture = Texture(location, location2,0, projector);
  texture.texSamplerLoc = glGetUniformLocation(shaderProgram_, "texSampler");
  texture.texSamplerLoc2 = glGetUniformLocation(shaderProgram_, "texSampler2");

  glUniform1i(hasTextureLoc, 1);

  for (Vertex& vertex : vertices)
  {
    vertex.uv = texture.generateUV(minPos, maxPos, vertex.position);
  }
  initBuffers();
}

void Object::draw()
{
  glUseProgram(shaderProgram_);

  if (fillPolygons)
  {
    glPolygonMode(GL_FRONT, GL_FILL);
  }
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // diffuse shade lights
  if (name.find("light") != string::npos)
  {
    glUniform3fv(vectorColorLoc, 1, glm::value_ptr(this->material.ambient));
  }
  else
  {
    glUniform3fv(vectorColorLoc, 1,glm::value_ptr(vec3(0)));
    glUniform1i(useGPUuvLoc, useGPUuv);

    // we are only going to use these if we're using gpu for uv's 
    if (useGPUuv)
    {
      glUniform3fv(lowerLoc, 1, glm::value_ptr(minPos));
      glUniform3fv(upperLoc, 1, glm::value_ptr(maxPos));
      glUniform1i(projectionLoc, texture.getProjector());
    }
  }

  glBindVertexArray(vao);
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &transform[0][0]);

  glDrawElements(renderMode, indices.size() , GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  if (faceNormDrawingMode == FaceNormalsOn)
  {
    glUseProgram(lineShader);
    genFaceNormals();
    drawFaceNormals();
  }
  else if (vertexNormalDrawingMode == VertexNormalOn)
  {
    glUseProgram(lineShader);
    drawVertexNormals();
  }
}


/////***** Private Object functions *****/////

void Object::initBuffers()
{
  // bind with default data
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);

  // send vert info
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

  // send normal info
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));

  // texture
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));

  // unbind
  glBindVertexArray(0);


  setShader(shaderProgram_);

  //assert(modelLoc >= 0 );
  //assert(colorLoc >= 0);

}

Texture::Texture(): location_("none")
{
}

Texture::Texture(std::string location, string location2, int texNum, Projector projector) : 
  projector_(projector), location_(location), location2_(location2)
{
  isValid = true;
  unsigned char* buffer = SOIL_load_image(location.c_str(), &width_, &height_, &channels_, SOIL_LOAD_AUTO);
  if (!buffer)
    cout << "invalid file " << location << "\n";

  glGenTextures(1, &tbo_);
  glActiveTexture(GL_TEXTURE0 + texNum);
  glBindTexture(GL_TEXTURE_2D, tbo_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  int format = 0;
  if (channels_ == 1)
    format = GL_RED;
  else if (channels_ == 3)
    format = GL_RGB;
  else if (channels_ == 4)
    format = GL_RGBA;

  glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, buffer);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(buffer);

  // spec image 
  buffer = SOIL_load_image(location2.c_str(), &width_, &height_, &channels_, SOIL_LOAD_AUTO);

  glGenTextures(1, &tbo2_);
  glActiveTexture(GL_TEXTURE1 + texNum);
  glBindTexture(GL_TEXTURE_2D, tbo2_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  format = 0;
  if (channels_ == 1)
    format = GL_RED;
  else if (channels_ == 3)
    format = GL_RGB;
  else if (channels_ == 4)
    format = GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, buffer);

  glGenerateMipmap(GL_TEXTURE_2D);

  SOIL_free_image_data(buffer);
}

Texture::~Texture()
{
}

void Texture::setTexture(int texNum)
{
  glGenTextures(1, &tbo_);
  glActiveTexture(GL_TEXTURE0 + texNum);
  glBindTexture(GL_TEXTURE_2D, tbo_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenerateMipmap(GL_TEXTURE_2D);
}



glm::vec2 Texture::generateUV(glm::vec3 lower, glm::vec3 upper, glm::vec3 point)
{
  glm::vec2 uv;
  vec3 centroid = vec3(lower + upper) / 2.0f;

  // translate centroid to the origin (and everyhting else by that amount)
  vec3 point2 = point - centroid;
  vec3 lower2 = lower - centroid;
  vec3 upper2 = upper - centroid;


  if (projector_ == Projector::Cylindrical)
  {
    float theta = atan2(point2.y, point2.x);
    if (theta < 0)
      theta = 2 * glm::pi<float>() - theta;

    float z = (point2.z - lower2.z) / (upper2.z - lower2.z);
    uv.x = theta / (2 * glm::pi<float>());
    uv.y = z;
  }

  else if (projector_ == Projector::Sphere)
  {
    // you promised this would be positive... but it's not always
    float theta =  atan2 (point2.y, point2.x);
    if (theta < 0)
      theta = 2 * glm::pi<float>() - theta;

    float r = sqrtf(point2.x * point2.x + point2.y * point2.y + point2.z * point2.z);
    float phi = acosf(point2.z / r);
    uv.x = (theta / (2 * glm::pi<float>()));
    uv.y = ((glm::pi<float>() - phi) / glm::pi<float>());
  }

  else if (projector_ == Projector::Cube)
  {
    float absX = fabs(point2.x);
    float absY = fabs(point2.y);
    float absZ = fabs(point2.z);
    float upperAxis = 1.0f;
    float lowerAxis = 0.0f;
    
    // +-X : ([z,-z],y)
    if (absX >= absY && absX >= absZ)
    {
      if (point2.x < 0.0f)
      {
        uv.x = point2.z;
      }
      else
      {
        uv.x = -point2.z;
      }

      uv.y = point2.y;
      upperAxis = upper2.x;
      lowerAxis = lower2.x;
    }
    
    //+-Y : (x,[z,-z])
    else if (absY >= absX && absY >= absZ)
    {
      uv.x = point2.x;

      if (point2.y < 0.0f)
      {
        uv.y = point2.z;
      }
      else
      {
        uv.y = -point2.z;
      }

      upperAxis = upper2.y;
      lowerAxis = lower2.y;
    }
    
    // +-Z : ([-x,x],y)
    else if (absZ >= absX && absZ >= absY)
    {
      if (point2.z < 0.0f)
      {
        uv.x = -point2.x;
      }
      else
      {
        uv.x = point2.x;
      }

      uv.y = point2.y;
      upperAxis = upper2.z;
      lowerAxis = lower2.z;
    }

    uv.x = (uv.x + upperAxis) / (upperAxis - lowerAxis);
    uv.y = (uv.y + upperAxis) / (upperAxis - lowerAxis);
  }

  return uv;
}

Skybox::Skybox(int shaderProgram, std::string folder) : Object(shaderProgram, "skybox")
{
  loadOBJ("Common/models/cube.obj");
  for (Vertex& vert : vertices)
  {
    vert.position -= 0.5;
  }
  string files[] =
  {
    "back.png",
    "down.png",
    "front.png",
    "left.jpg",
    "right.png",
    "up.png",
  };

  for (int i = 0; i < 6; i++)
  {
    string name = folder + files[i];
    textures[i] = Texture(name, name, i, Texture::Projector::Cube);
    string texName = "texSampler" + std::to_string(i);
    textures[i].texSamplerLoc = glGetUniformLocation(shaderProgram_, texName.c_str());
    textures[i].isValid = true;
  }

  initBuffers();
  useGPUuv = 1;
  glUniform1i(hasTextureLoc, 6);
}

