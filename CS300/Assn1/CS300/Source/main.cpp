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
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}


bool inFront(Object& A, Object& B)
{
    return A.transform[3].z < A.transform[3].z;
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
    Object test(shaderProgram, "test");
    test.addScale(vec3(100));
    test.translate(vec3(0, 0, -5));
    objects.push_back(test);

    Object test2(shaderProgram, "test2");
    test2.loadOBJ("Common/models/cube.obj");
    test2.translate(vec3(0, 0, -2));
    objects.push_back(test2);
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
    if (WindowInit(3200, 2400, 4, 0, &window) == false)
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

    //view = glm::rotate(view, glm::radians(y), glm::vec3(0, 1, 0));

    GenerateScene(shaderProgram);

    // loop
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);
        
        //objects[FindObject("test")].addRotation(1, vec3(1,0,0));
        //objects[FindObject("test2")].addRotation(1, vec3(0, 1, 0));

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.view = glm::rotate(camera.view, 0.01f, vec3(1, 0, 0));



        Render(window, camera);
        glfwPollEvents();

    }

    return 0;
}