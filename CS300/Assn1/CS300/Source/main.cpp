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

#include "imGui/imgui.h"
#include "imGui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"

#include "ObjectManagement.h"
#include "Camera.h"
#include "Light.h"

static vector<Object> objects;
static vec3 up(0, 1, 0);
static vec3 right(1, 0, 0);
static vec3 down(0, -1, 0);
static vec3 left(-1, 0, 0);
static vec3 back(0, 0, -1);
static vec3 forward(0, 0, 1);

static int selectedObject = 0;


int FindObject(string name);

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
void Terminate()
{
    glfwTerminate();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

void Render(GLFWwindow* window, Camera& camera, Light& light)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    light.update();
    camera.update();
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i].draw();
    }
}

void GenerateScene(unsigned shaderProgram)
{
    Object center(shaderProgram, "OBJ model");
    center.loadOBJ("Common/models/bunny.obj");
    center.addScale(vec3(2));
    center.color = vec3(0,0,.7);
    center.genFaceNormals();
    objects.push_back(center);

    int ballCount = 8;
    int circleRadius = 4;
    for (int i = 0; i < ballCount; i++)
    {
        Object ball(shaderProgram, "ball" + std::to_string(i));
        ball.loadSphere(.5, 30);
        ball.translate(vec3(circleRadius * glm::cos(glm::radians(360.0f / ballCount * i)), 0, circleRadius *  glm::sin(glm::radians(360.0f / ballCount * i))));
        ball.orbitRadius = circleRadius;
        ball.color = vec3(0, 0.7f, 0);

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

void InitGUI(GLFWwindow* window)
{
    // context 
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // open GL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // setup stuff
    ImGui::StyleColorsDark();
}
void UpdateGUI()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Welcome to CS300 Assignment 1!");

    // normal stuff
    ImGui::Text("Selected Object");
    ImGui::SliderInt(objects[selectedObject].name.c_str(), &selectedObject, 0, objects.size() - 1);
    bool toggleFN = ImGui::Button("Toggle selected object's Face Normal");
    bool changeNormalLength = ImGui::SliderFloat("Normal Display Scale", &objects[selectedObject].vectorScale, 0, 2.0);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    if (toggleFN || changeNormalLength)
    {
        objects[selectedObject].toggleFaceNormals();

        if(changeNormalLength)
            objects[selectedObject].toggleFaceNormals();
    }
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
    glEnable(GL_DEPTH_TEST);

    InitGUI(window);

    //make a test object
    int shaderProgram = InitShaderProgram("Source/vertexShader.vert", "Source/fragmentShader.frag");

    Camera camera(vec3(0,-2,-10), 0.0f, vec3(1,0,0), shaderProgram);
    Light light(shaderProgram, "light");
    light.color = vec3(1);
    light.emitter.color = light.color;
    light.ambientStrength = 0.75f;
    light.translate(vec3(0,5,0));

    GenerateScene(shaderProgram);

    // loop
    while (!glfwWindowShouldClose(window))
    {
        double time = glfwGetTime();
        ProcessInput(window);
        UpdateGUI();

        {
            float moveStr = 0.1f;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                light.translate(moveStr * up);
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                light.translate(moveStr * down);
            }
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            {
                light.translate(moveStr * back);
            }
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            {
                light.translate(moveStr * forward);
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                light.translate(moveStr * left);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                light.translate(moveStr * right);
            }
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            {
                objects[0].spin(.5, up);
            }
        }

        // rotate balls
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        {

            for (size_t i = 0; i < objects.size(); i++)
            {
                if (objects[i].name.find("ball") != string::npos)
                {
                    float angularV = -0.05f;
                    float radius = objects[i].orbitRadius;
                    objects[i].rotateY(angularV, radius);
                }
            }
        }

        //maintain viewport
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // render scene and GUI window
        Render(window, camera, light);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();

        // maintain a max of 120hertz
        double dt = glfwGetTime() - time;
        while (dt < 1 / 120.0)
        {
            dt = glfwGetTime() - time;
        }
    }

    Terminate();

    return 0;
}