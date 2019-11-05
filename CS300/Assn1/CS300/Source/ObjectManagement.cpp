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


/////***** Object Material Management *****/////


void MaterialManager::genUBO(unsigned shaderProgram)
{
  glUseProgram(shaderProgram);
  glGenBuffers(1, &ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialData), nullptr, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MaterialManager::updateUBO(MaterialData& material)
{
  void* uboBuffer;
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  uboBuffer = glMapNamedBuffer(ubo_, GL_READ_WRITE);

  memcpy(uboBuffer, &material, sizeof(MaterialData));

  glUnmapNamedBuffer(ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

/////***** Vertex Class *****/////

Vertex::Vertex(float x, float y, float z, float nx, float ny, float nz)
{
  *this = Vertex(vec3(x,y,z), vec3(nx, ny, nz));
}

Vertex::Vertex(vec3 pos, vec3 norm ) : position(pos), normal(norm)
{
}


/////***** Line Class *****/////


Line::Line(vec3 pointA, vec3 pointB) : centroid(pointA), normDir(pointB)
{}



/////***** Object Class *****/////


Object::Object(int shaderPgm, string ID) : name(ID), shaderProgram(shaderPgm)
{
  transform = glm::mat4(1.0f);

  // init openGL buffers
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  initBuffers();

}

Object::~Object()
{}

void Object::translate(glm::vec3 translation)
{
  vec3 trans = translation / modelScale;
  transform = glm::translate(transform, trans);
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
  glUseProgram(shaderProgram);
  modelLoc = glGetUniformLocation(shaderProgram, "model");

  glUseProgram(lineShader);
  vectorColorLoc = glGetUniformLocation(shaderProgram, "objColor");

  glUseProgram(shaderProgram);
}


void Object::drawFaceNormals()
{
  glBindVertexArray(lineVAO);
  vec3 red (1,0,0);
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

  glm::vec3 vMax(INT_MIN, INT_MIN, INT_MIN);
  glm::vec3 vMin(INT_MAX, INT_MAX, INT_MAX);
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
          vMin[i] = min(vMin[i], vertexData);
          vMax[i] = max(vMax[i], vertexData);
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
    vec3 centroid = { (vMax.x + vMin.x) / 2, (vMax.y + vMin.y) / 2, (vMax.z + vMin.z) / 2 };
    modelScale = 2 / glm::length(vMax - centroid);
    addScale(vec3(modelScale, modelScale, modelScale));

    genVertexNormals();

    vec3 translation((vMax.x + vMin.x) / 2, (vMax.y + vMin.y) / 2, (vMax.z + vMin.z) / 2);
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
    Vertex(-0.5f, 0.0f, -0.5f),
    Vertex( 0.5f, 0.0f, -0.5f),
    Vertex( 0.5f, 0.0f,  0.5f),
    Vertex(-0.5f, 0.0f,  0.5f),
  };

  indices =
  {
    //0,1,2, 
    //0,2,3,
    0,3,2,
    0,2,1
  };

  initBuffers();
}


void Object::draw()
{
  glUseProgram(shaderProgram);
  if (fillPolygons)
  {
    glPolygonMode(GL_FRONT, GL_FILL);
  }
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

  // unbind
  glBindVertexArray(0);


  setShader(shaderProgram);

  //assert(modelLoc >= 0 );
  //assert(colorLoc >= 0);

}

