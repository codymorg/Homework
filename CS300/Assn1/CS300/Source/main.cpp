/******************************************************************************
    Name : Cody Morgan
    Class: CS 300
    Assn : 01
    Brief: Creates window and sets up openGL environment
    Date:  4 OCT 2019

******************************************************************************/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using glm::vec3;
using glm::vec4;

#include <iostream>
using std::cout;

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <algorithm>
using std::sort;

#include "ObjectManagement.h"
#include "Camera.h"

static vector<Object> objects;


/******************************************************************************
    brief : creates a window

    input : width  - window width
            height - window height
            major  - opengl version major
            minor  - opengl minor
            window - window handle

    output: true if everything is ok

******************************************************************************/
bool WindowInit(int width, int height, int major, int minor, GLFWwindow** window)
{
    GLenum error = glfwInit();
    if (error == GLFW_FALSE)
    {
        char buffer[256];
        glfwGetError((const char**)buffer);
        return getchar();
    }

    // instance the window
    *window = glfwCreateWindow(width, height, "CS300 Assgnment 1", NULL, NULL);
    glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // create the window obj
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);

    error = glewInit();
    if (error)
    {
        cout << glewGetErrorString(error);
        return getchar();
    }
    glViewport(0, 0, width, height);
    // window is gtg
    return true;
}

/******************************************************************************
    brief : cleans up program

******************************************************************************/
void Terminate(int vertexShader, int fragmentShader)
{
    glfwTerminate();
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

/******************************************************************************
    brief : process all keyboard input

    input : window - window handle

******************************************************************************/
void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/******************************************************************************
    brief : compile shader

    input : shaderLocation : file location
            shader         : GL_VERTEX_SHADER or GL_FRAGMENT_SHADER

******************************************************************************/
int InitShader(string shaderLocation, unsigned shader)
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
        printf("Impossible to open << %s >>. Are you in the right directory ? Don't forget to read the FAQ !\n", shaderLocation);
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

        return -1;
    }

    return shaderNum;
}
int InitShaderProgram(string vertShaderLocation, string fragShaderLocation)
{
    int vertexShader = InitShader(vertShaderLocation, GL_VERTEX_SHADER);
    int fragmentShader = InitShader(fragShaderLocation, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram;
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

        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);

    return shaderProgram;
}

/******************************************************************************
    brief : error callback

******************************************************************************/
void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{

#ifndef DEBUG_SEVERITY_HIGH
#define DEBUG_SEVERITY_HIGH         0x9146
#define DEBUG_SEVERITY_MEDIUM       0x9147
#define DEBUG_SEVERITY_LOW          0x9148
#define DEBUG_SEVERITY_NOTIFICATION 0x826B

#define DEBUG_TYPE_ERROR               0x824C
#define DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define DEBUG_TYPE_PORTABILITY         0x824F
#define DEBUG_TYPE_PERFORMANCE         0x8250
#define DEBUG_TYPE_OTHER               0x8251
#define DEBUG_TYPE_MARKER              0x8268
#endif

    if (severity != DEBUG_SEVERITY_NOTIFICATION)
    {
        string debugTypes[] =
        {
            "DEBUG_TYPE_ERROR",
            "DEBUG_TYPE_DEPRECATED_BEHAVIOR",
            "DEBUG_TYPE_UNDEFINED_BEHAVIOR",
            "DEBUG_TYPE_PORTABILITY",
            "DEBUG_TYPE_PERFORMANCE",
            "DEBUG_TYPE_OTHER",
            "DEBUG_TYPE_MARKER"
        };
        string severityTypes[] =
        {
            "HIGH",
            "MEDIUM",
            "LOW"
        };

        string typeString = debugTypes[(type - DEBUG_TYPE_ERROR) % 7];
        string severityString = severityTypes[(severity - DEBUG_SEVERITY_HIGH) % 3];

        fprintf(stderr, "Error\n     Severity: %s     Type: %s\n     \n     %s\n\n", severityString.c_str(), typeString.c_str(), message);
    }
}


bool inFront(Object& A, Object& B)
{
    return A.transform[3].z < B.transform[3].z;
}

void Render(GLFWwindow* window, Camera& camera)
{
    sort(objects.begin(), objects.end(), inFront);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < objects.size(); i++)
    {
        camera.update();
        objects[i].draw();
    }
    glfwSwapBuffers(window);
}

void GenerateScene(unsigned shaderProgram)
{
    Object center(shaderProgram, "center");
    center.loadOBJ("Common/models/cube.obj");
    center.addScale(vec3(1.5f,3.0,1));
    objects.push_back(center);

    int ballCount = 15;
    int circleRadius = 4;
    for (int i = 0; i < ballCount; i++)
    {
        Object ball(shaderProgram, "ball" + std::to_string(i));
        ball.loadSphere(.5, 30);
        ball.translate(vec3(circleRadius * glm::cos(glm::radians(360.0f / ballCount * i)), 0, circleRadius *  glm::sin(glm::radians(360.0f / ballCount * i))));
        ball.orbitRadius = circleRadius;
        objects.push_back(ball);
    }

    Object circle(shaderProgram, "circle");
    circle.loadcircle(circleRadius,45);
    objects.push_back(circle);
}

int FindObject(string name)
{
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i].name.find(name) != string::npos)
        {
            return i;
        }
    }

    return -1;
}

int main()
{
    // make a window
    GLFWwindow* window = nullptr;
    if (WindowInit(1600 , 1200 , 4, 0, &window) == false)
        return -1;

    // GL state setting
#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Camera setup

    //make a test object
    int shaderProgram = InitShaderProgram("Source/vertexShader.vert", "Source/normalShader.frag");

    Camera camera(vec3(0,-2,-10), 0.0f, vec3(1,0,0), shaderProgram);

    GenerateScene(shaderProgram);

    // loop
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);
        
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.view = glm::rotate(camera.view, 0.01f, vec3(1, 0, 0));
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
        {
            for (size_t i = 0; i < objects.size(); i++)
            {
                if (objects[i].name.find("ball") != string::npos)
                {
                    float angularV = 0.5f;
                    float radius = objects[i].orbitRadius;
                    objects[i].rotateY(angularV, radius);
                    vec4 position = objects[i].transform[3];
                    vec3 axis(position.x, 0, -position.z);
                    float spinAmount = angularV * radius;
                    objects[i].spin(spinAmount,-axis);

                }
            }
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
        static float y = 3;
            objects.pop_back();
            Object circle(shaderProgram, "circle");
            circle.loadcircle(5, y+=0.1);
            objects.push_back(circle);
        }


        Render(window, camera);
        glfwPollEvents();

    }


    return 0;
}