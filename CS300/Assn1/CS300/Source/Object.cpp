/******************************************************************************
    Name : Cody Morgan
    Class: CS 300
    Assn : 01
    Brief: object related classes
    Date:  4 OCT 2019

******************************************************************************/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

#include "Object.h"
using std::string;
using std::getline;

Vertex::Vertex(float x, float y, float z)
{
    position = glm::vec3(x, y, z);
    color = glm::vec3(0.5f, 0.5f, 0.5f);
}

Object::Object(unsigned shader, unsigned VAO, unsigned VBO, unsigned EBO) : vao(VAO), vbo(VBO), ebo(EBO)
{
    shaderProgram = shader;
    polyColor = glm::vec4(0.5, 0.5, 0.5, 1);
    transform = glm::vec3(0, 0, 0);

}


void Object::draw()
{
    if (fillPolygons)
    {
        glPolygonMode(GL_FRONT, GL_FILL);
    }
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // safeguard rotation
    while (rotation >= 360.0f)
        rotation -= 360.0f;

    // camera
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, modelScale);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 1, 0));
    model = glm::translate(model, transform);
    zSort = model[3][2];

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -2, -25.0f));
    
    //view = glm::rotate(view, glm::radians(y), glm::vec3(0, 1, 0));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 4 / 3.0f, 0.1f, 100.0f);
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(lightColorLoc, 0, 0.5, 1.0);
    glUniform3f(lightPosLoc, 0, 0, 0);



    // color
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform3f(vertexColorLocation, polyColor.r, polyColor.g, polyColor.b);

    // vertex info
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attributeCount * 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    if (attributeCount > 1)
    {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, attributeCount * 3 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    glUseProgram(shaderProgram);
    glDrawElements(renderMode, indices.size(), GL_UNSIGNED_INT, 0);

}

void Object::addVertex(float x, float y, float z)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

Mesh LoadOBJ(string fileLocation)
{
    ifstream file(fileLocation);
    Mesh objMesh;
    vector<float>verts;
    vector<float>normals;
    glm::vec3 vMax(INT_MIN, INT_MIN, INT_MIN);
    glm::vec3 vMin(INT_MAX, INT_MAX, INT_MAX);


    if (file)
    {
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
                        objMesh.indices.push_back(index - 1);
                    }
                }

                // if you have ' ' delimitation like a normal person
                else
                {
                    while (getline(objData, token, ' '))
                    {
                        int index;
                        objData >> index;
                        objMesh.indices.push_back(index - 1);
                    }
                }

            }
        }
    }
    else
    {
        cout << "error opening file: " << fileLocation << "\n";
    }

    for (size_t i = 0; i < verts.size(); i+=3)
    {
        objMesh.vertices.push_back(verts[i]);
        objMesh.vertices.push_back(verts[i+1]);
        objMesh.vertices.push_back(verts[i+2]);
        if (!normals.empty())
        {
            objMesh.vertices.push_back(normals[i]);
            objMesh.vertices.push_back(normals[i + 1]);
            objMesh.vertices.push_back(normals[i + 2]);
            i += 3;
        }
    }
    return objMesh;

}

