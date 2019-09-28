/******************************************************************************
    Name : Cody Morgan
    Class: CS 300
    Assn : 01
    Brief: object related classes
    Date:  4 OCT 2019

******************************************************************************/

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


Vertex::Vertex(float x, float y, float z, float nx, float ny, float nz)
{
    *this = Vertex(vec3(x,y,z), vec3(nx, ny, nz));
}

Vertex::Vertex(vec3 pos, vec3 norm ) : position(pos), normal(pos)
{
    position = pos;
    if (norm.x + norm.y + norm.z > 0.0f)
    {
        normal = norm;
    }
}


/////***** Object Class *****/////

Object::Object(int shaderPgm, string ID) : name(ID), shaderProgram(shaderPgm), objectColor(0,128,128)
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
    transform = glm::translate(transform, translation);
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

void Object::rotateY(float degrees, glm::vec3 point)
{
    glm::vec3 pos(transform[3][0], transform[3][1], transform[3][2]);
    mat4 rotM(1.0f);
    rotM = glm::translate(rotM, -pos);
    rotM = glm::rotate(rotM, glm::radians(degrees), vec3(0,1,0));
    rotM = glm::translate(rotM, pos);
    transform = rotM * transform;
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
    vector<float>normals;
    glm::vec3 vMax(INT_MIN, INT_MIN, INT_MIN);
    glm::vec3 vMin(INT_MAX, INT_MAX, INT_MAX);

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
            else if (line.substr(0, 2) == "vn")
            {
                std::istringstream objData(line.substr(2));
                float vertexData;
                for (size_t i = 0; i < 3; i++)
                {
                    objData >> vertexData;
                    normals.push_back(vertexData);
                }
            }
            else if (line.substr(0, 2) == "f ")
            {
                std::istringstream objData(line);
                std::string token;

                // if you have '\\' delimiters
                if (line.find("\\") != string::npos)
                {
                    while (getline(objData, token, '\\'))
                    {
                        int index;
                        std::istringstream substring(token);
                        substring >> index;
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
                        objData >> index;
                        indices.push_back(index - 1);
                    }
                }

            }
        }
    }
    else
    {
        cout << "error opening file: " << fileLocation << "\n";
    }

    // load data into vertex struct

    for (size_t i = 0; i < verts.size(); i += 3)
    {
        vec3 normalData(0, 0, 0);
        vec3 vertData(verts[i], verts[i + 1], verts[i + 2]);
        if (!normals.empty())
        {
            normalData = vec3(normals[i], normals[i + 1], normals[i + 2]);
        }
        vertices.push_back(Vertex(vertData, normalData));
    }

    vec3 scale(1 / (vMax.x - vMin.x), 1 / (vMax.y - vMin.y), 1 / (vMax.z - vMin.z));
    addScale(scale);

    vec3 translation((vMax.x + vMin.x) / 2, (vMax.y + vMin.y) / 2, (vMax.z + vMin.z) / 2);
    translate(-translation);

    initBuffers();
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
		k1 = i * (divisions + 1);     // beginning of current stack
		k2 = k1 + divisions + 1;      // beginning of next stack

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


void Object::draw()
{
    if (fillPolygons)
    {
        glPolygonMode(GL_FRONT, GL_FILL);
    }
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(vao);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &transform[0][0]);

    glDrawElements(renderMode, indices.size() , GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
}