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

#include "Object.h"


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

int InitVBO()
{
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    return VBO;
}

int InitVAO()
{
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    return VAO;
}

int InitEBO()
{
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    return EBO;
}

int InitShader(string shaderLocation, int shader)
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
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", shaderLocation);
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
        if(shader == GL_VERTEX_SHADER)
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        else
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return -1;
    }



    return shaderNum;
}

int InitShaderProgram(int vertexShader, int fragmentShader)
{
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

    glUseProgram(shaderProgram);

    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    

    return shaderProgram;
}


//Mesh CreateSphere(float radius, int numDivisions)
//{
//    Mesh sphere;
//
//    // generate verts
//    for (float theta = 0; theta < 360; theta += 360.0f / numDivisions)
//    {
//        for (float phi = 0; phi < 180; phi += 180.0f / numDivisions)
//        {
//            float x = radius * sin(theta) * cos(phi);
//            float y = radius * sin(theta) * sin(phi);
//            float z = radius * cos(theta);
//
//            sphere.vertices.push_back(x); 
//            sphere.vertices.push_back(y); 
//            sphere.vertices.push_back(z); 
//
//            //generate normal
//            vec3 norm = glm::normalize(vec3(x, y, z));
//            sphere.vertices.push_back(norm.x);
//            sphere.vertices.push_back(norm.y);
//            sphere.vertices.push_back(norm.z);
//        }
//    }
//
//    //triagonalize verts
//    int indexA = -1;           // top vert
//    int indexB = numDivisions; // bottom vert
//
//    // initial strip
//    for (int i = 0; i < numDivisions + 1; i++)
//    {
//        indexA = (indexA + 1) % numDivisions;
//        sphere.indices.push_back(indexA);
//
//        indexB = numDivisions + indexA;
//        sphere.indices.push_back(indexB);
//    }
//
//    // every other strip
//    int vertexCount = sphere.vertices.size() / 6;
//    for (int count = 0; count < numDivisions; count++)
//    {
//        for (int i = 0; i < 2 * (numDivisions + 1); i++)
//        {
//            int index = (sphere.indices[i] + (count * numDivisions)) % vertexCount;
//            sphere.indices.push_back(index);
//        }
//    }
//    return sphere;
//
//
//}
Mesh CreateSphere(float radius, int numDivisions)
{
    Mesh sphere;

    // generate verts
    for (float theta = 0; theta < 360; theta += 360.0f / numDivisions)
    {
        for (float phi = 0; phi < 180; phi += 180.0f / numDivisions)
        {
            float x = radius * sin(theta) * cos(phi);
            float y = radius * sin(theta) * sin(phi);
            float z = radius * cos(theta);

            sphere.vertices.push_back(x);
            sphere.vertices.push_back(y);
            sphere.vertices.push_back(z);

            //generate normal
            vec3 norm = glm::normalize(vec3(x, y, z));
            sphere.vertices.push_back(norm.x);
            sphere.vertices.push_back(norm.y);
            sphere.vertices.push_back(norm.z);
        }
    }

    // generate indices
    int rows = numDivisions - 1;        // how many rows does the ball have
    int rowMax = 2 * numDivisions;  // max index for a row

    for (size_t r = 0; r < rows; r++)
    {
        int A = r * numDivisions;
        int B = A + rowMax - 1;
        int C = A + numDivisions;

        // push a quad of triangles at a time
        for (size_t tri = 0; tri < numDivisions; tri++)
        {
            // push top tri
            sphere.indices.push_back(A);
            sphere.indices.push_back(B);
            sphere.indices.push_back(C);

            // push next tri in quad
            B++;
            if (B % numDivisions == 0)
                B -= numDivisions;

            C += 1 - numDivisions;
            if (C % numDivisions == 0)
                C -= numDivisions;

            sphere.indices.push_back(A);
            sphere.indices.push_back(B);
            sphere.indices.push_back(C);

            // setup next tri
            A++;
            C += numDivisions;

        }
    }
    return sphere;
}

Mesh CreateCircleMesh(float radius, vec3 position)
{
    Mesh mesh;
    for (float theta = 0; theta < 360; theta++)
    {
        mesh.vertices.push_back(position.x + radius * cos(glm::radians(theta))); //x
        mesh.vertices.push_back(position.y);                            //y
        mesh.vertices.push_back(position.z + radius * sin(glm::radians(theta))); //z
        mesh.indices.push_back(theta);
    }

    return mesh;

}
Object CreateCircle(unsigned shader, unsigned vao, unsigned vbo, unsigned ebo, float radius, vec3 position)
{
    Mesh cMesh = CreateCircleMesh(radius, position);
    Object circleArch(shader, vao, vbo, ebo);
    circleArch.vertices = cMesh.vertices;
    circleArch.indices = cMesh.indices;
    circleArch.renderMode = GL_LINES;

    return circleArch;
}

Object CreateCube(unsigned shader, unsigned vao, unsigned vbo, unsigned ebo)
{
    Object obj(shader, vao, vbo, ebo);
    obj.vertices =
    {
        -1,1,-1,
        1,1,-1,
        1,-1,-1,
        -1,-1,-1,
        -1,-1,1,
        1,-1,1,
        1,1,1,
        -1,1,1
    };
    obj.indices = 
    {
        /*
        */
        0,3,1, 
        1,3,2,
        7,4,6,
        6,4,5,
        7,6,0,
        6,1,0,
        4,3,5,
        5,3,2,
        1,6,2,
        6,5,2,
        6,1,5,
        5,1,2
    };
    obj.renderMode = GL_TRIANGLES;
    obj.attributeCount = 1;


    return obj;
}

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

bool inFront(Object* A, Object* B)
{
    return A->zSort < B->zSort;
}

void Render(vector<Object*>& objects)
{
    sort(objects.begin(), objects.end(), inFront);

    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }
}

int main()
{
    vector<Object*> objects;

    GLFWwindow* window = nullptr;
    if (WindowInit(800 * 4, 600 * 4, 4, 0, &window) == false)
        return -1;

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // generate VBO
    int vao = InitVAO();
    int vbo = InitVBO();
    int ebo = InitEBO();

    // generate shaders
    int vs = InitShader("Source/vertexShader.vert", GL_VERTEX_SHADER);
    int fs = InitShader("Source/fragmentShader.frag", GL_FRAGMENT_SHADER);
    int shaderProg = InitShaderProgram(vs, fs);

    // create some objects
    Object sphereArch(shaderProg, vao, vbo, ebo);
    Mesh sphere = CreateSphere(1, 4);
    sphereArch.vertices = sphere.vertices;
    sphereArch.indices =  sphere.indices;
    sphereArch.renderMode = GL_TRIANGLES;
    sphereArch.transform.x = 5;
    sphereArch.attributeCount = 2;


    Object balls[] =
    {
        Object(shaderProg, vao, vbo, ebo),
        Object(shaderProg, vao, vbo, ebo),
        Object(shaderProg, vao, vbo, ebo),
        Object(shaderProg, vao, vbo, ebo),
        Object(shaderProg, vao, vbo, ebo),
        Object(shaderProg, vao, vbo, ebo),
        Object(shaderProg, vao, vbo, ebo),
        Object(shaderProg, vao, vbo, ebo),
        Object(shaderProg, vao, vbo, ebo)
    };
    for (int i = 0; i < _countof(balls); i++)
    {
        balls[i] = sphereArch;
        balls[i].transform = vec3(i, i, 5);
        balls[i].ID = i;

        objects.push_back(&balls[i]);
    }

    float radius = 5;
    float height = 0.0f;
    Object circles[] =
    {
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0)),
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0)),
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0)),
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0)),
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0)),
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0)),
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0)),
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0)),
        CreateCircle(shaderProg, vao, vbo, ebo, radius += 0.5, vec3(0, height++, 0))
    };
    for (int i = 0; i < _countof(circles); i++)
    {
        circles[i].attributeCount = 1;
        circles[i].renderMode = GL_LINES;
        objects.push_back(&circles[i]);
    }

    Object cube = CreateCube(shaderProg, vao, vbo, ebo);
    cube.transform = vec3(5, 5, 5);
    cube.fillPolygons = true;
    objects.push_back(&cube);

    Mesh destroyerMesh = LoadOBJ("Common/models/cube2.obj");
    Object destroyer(shaderProg, vao, vbo, ebo);
    destroyer.vertices = destroyerMesh.vertices;
    destroyer.indices = destroyerMesh.indices;
    destroyer.renderMode = GL_TRIANGLES;
    destroyer.fillPolygons = true;

    destroyer.attributeCount = 1;
    destroyer.transform = glm::vec3(0,0,0);
    objects.push_back(&destroyer);


    while (!glfwWindowShouldClose(window))
    {

        ProcessInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // update
        float rotationAmount = 2;
        sphereArch.rotation += rotationAmount;
        for (size_t i = 0; i < _countof(balls); i++)
        {
            balls[i].rotation += 0.1;

        }
            destroyer.rotation += 0.1;

        // render
        Render(objects);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // end of program
    Terminate(vs,fs);
    return 0;
}

