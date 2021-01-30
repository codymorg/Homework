/******************************************************************************
 *                            Change Log
 * main now splits the viewport into 4 quadrants
 * update now updates for eacch quadrant
 * each quadrant rotates from a static variable stored in update
 *****************************************************************************/


/*!
@file    spinning-cube.cpp
@author  pghali@digipen.edu

Who doesn't like a spinning 3D cube?
This is the first example that illustrates the 3d graphics pipe. The program
does the following:
1) initializes GLFW,
2) creates and initializes an OpenGL 4.5 context (the context is the entire
OpenGL state plus framebuffer (colorbuffer and depthbuffer),
3) initializes OpenGL extension loading library GLEW,
4) prints the OpenGL version and other context parameters, and
5) sets callbacks for the following events: keyboard key (press, repeat,
release), mouse button (left/right press and release), mouse scroll
offset, and mouse cursor position (relative to top-left corner of window)
6) transfers cube geometry from client to server using a VBO, initializes
a VAO and EBO (vertex index object) that allow the client to specify the 
properties of this geometry to the pipe,
7) compiles a vertex shader and a fragment shader which are then linked
to create a program object,
7) loops for ever until ESC key is pressed or the window's close flag is
clicked or Alt+F4 is pressed. Each frame, the VAO and EBO are used to 
transfer the cube geometry specified by the VAO and EBO along with a
shader program object that will process the transferred geometry.

This source file uses the following OpenGL commands:
1) glGetString
2) glGetIntegerv
3) glViewport
4) glClearColor
5) glClear

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
// Extension loader library's header must be included before GLFW's header!!!
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// glm related headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// C/C++ standard library
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility> // for std::forward, std::pair
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

/*                                                      function declarations
-----------------------------------------------------------------------------*/
// instead of a monolithic function, split code into functional units ...

// initialization of OpenGL state ...
static bool create_log_file();
static GLFWwindow* create_gl_context(GLuint framebuffer_width,
                                    GLuint framebuffer_height,
                                    std::string app_title);
static bool init_gl_loader();
static void query_gl_context();
static void init_gl_state(GLFWwindow *pwin);

// set up VAO for object to be drawn ...
static void init_cube();

// game loop ...
static void draw(GLFWwindow*);
static void update(GLFWwindow*);
static void cleanup(GLFWwindow*);

// dealing with the compilation and linking and checking the sanity of shaders
static void compile_and_link_shaders();
static GLboolean shader_compile_status(GLuint shader_hdl, 
                                      std::string& diag_msg);
static GLboolean shader_pgm_link_status(GLuint program_hdl, 
                                        std::string& diag_msg);

// time per frame calculations ...
static double update_time(double &fps, double fpsCalcInt = 1.0);

// I/O ...
static void glfw_error_cb(int error, char const* description);
static void fbsize_cb(GLFWwindow *pwin, int width, int height);
static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);
//-----------------------------------------------------------------------------------------------------------------------

/*                                                   objects with file scope
-----------------------------------------------------------------------------*/
std::string const g_logfilename{ "debug-log.txt" };

// stuff related to initialization ...
std::string const g_window_title{"spinning-cube: first 3D program"};
static double s_frames_per_second; // frames per second

// everything required to define the geometry of cube object ...
// 12 triangles with 3 vertices per triangle
static GLuint const s_index_element_count = 36; 
static GLuint s_vao_hdl;
static GLuint s_ebo_hdl;

// everything required to update and display cube object ...
// shader program for transforming and coloring cube object
static GLuint s_shaderpgm_hdl;
// flag to indicate whether cube is animate
static GLboolean s_cube_animate_flag = true;
// model-world-view-clip transform matrix used by vertex shader ...
static glm::mat4 s_cube_mvp_xform = glm::mat4(1.f);

/*                                                function template definitions
-----------------------------------------------------------------------------*/
/*! log_to_debug_file
This is a template function that writes the values of one or more parameters
to a file whose name is specified by the first function parameter.

@param std::string const&
Read-only reference to file name to which function will append debug
information

@param Args&& ...
template <typename... Args> represents a parameter pack (rather than a single
template parameter), i.e., Args is a list of type parameters.
Args&& ... args means args represents a list of forwarding references.

@return bool
Returns true if the function was able to open the specified file and log the
debug information. Otherwise, the function will return false if it was
unable to open the specified file.
*/
template <typename... Args>
bool log_to_debug_file(std::string file_name, Args&&... args) {
  std::ofstream ofs(file_name.c_str(), std::ofstream::app);
  if (!ofs) {
    std::cerr << "ERROR: could not open log file "
      << file_name << " for writing" << std::endl;
    return false;
  }
  int dummy[sizeof...(Args)] = {(ofs << std::forward<Args>(args) << ' ', 1)...};
  ofs << std::endl;
  ofs.close();
  return true;
}

/*  _________________________________________________________________________*/
/*! main - implements the basic game loop

@param none

@return int

Indicates how the program exited. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/

/******************************************************************************
 *                            Change Log
 * splits viewport into 4 quadrants for drawing
 * updates each viewport with a different rotation axis
 *****************************************************************************/
int main() {
  // initialization of OpenGL context using GLFW and GLEW ...
  create_log_file();
  GLFWwindow *pWindow = create_gl_context(2400, 1350, g_window_title);
  if (false == init_gl_loader()) {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  query_gl_context();
  init_gl_state(pWindow);

  // initialize geometry and associated shaders
  init_cube(); // set up vao, vio
  compile_and_link_shaders(); // set up shader pgm object
  
  // GAME LOOP ...
  // window's close flag is set by 'Esc' or clicking close widget or Alt+F4
  while (!glfwWindowShouldClose(pWindow)) 
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //changed to include 4 viewports
    GLsizei width, height;
    glfwGetFramebufferSize(pWindow, &width, &height);
    GLsizei halfWidth = width / 2;
    GLsizei halfHeight = height / 2;

    GLsizei viewports[][4] =
    {
      {0,         halfHeight, halfWidth,  halfHeight},//top left
      {halfWidth, halfHeight, halfWidth,  halfHeight},//top right
      {0,         0,          halfWidth,  halfHeight},//bottom left
      {halfWidth, 0,          halfWidth,  halfHeight} //bottom right
    };

    for (size_t i = 0; i < _countof(viewports); i++)
    {
      glViewport(viewports[i][0], viewports[i][1], viewports[i][2], viewports[i][3]);
      update(pWindow);  // create graphics task i+1
      draw(pWindow);
    }

    glfwSwapBuffers(pWindow);
  }
  // return (acquired) resources back to the system
  cleanup(pWindow);
}

/*  _________________________________________________________________________*/
/*! draw

@param GLFWwindow*
Handle to window that defines the OpenGL context

@return none

For now, there's only the cube that needs to be displayed
*/
static void draw(GLFWwindow *pWindow) {
  // at the top of each frame, the color (frame) buffer must be cleared of
  // the previous image's pixel values.
  // similarly, the depth buffer which contains the depths - from the
  // previous frame - of the closest points (to the camera) in the scene
  // must also be cleared
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // write window title with current fps ...
  std::stringstream sstr;
  sstr << std::fixed << std::setprecision(2) << g_window_title
                      << ": " << s_frames_per_second;
  glfwSetWindowTitle(pWindow, sstr.str().c_str());

  // draw cube ...

  // load vertex and fragment programs to corresponding cores
  glUseProgram(s_shaderpgm_hdl);
  // load "model-to-world-to-view-to-clip" matrix to uniform variable named 
  // "uMVP" in vertex shader
  GLint uniform_var_loc = glGetUniformLocation(s_shaderpgm_hdl, "uMVP");
  if (uniform_var_loc >= 0) {
    glUniformMatrix4fv(uniform_var_loc, 1, GL_FALSE, &s_cube_mvp_xform[0][0]);
  } else {
    std::cout << "Uniform variable with name uMVP doesn't exist!!!" 
              << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // transfer vertices from server (GPU) buffers to vertex shader core which 
  // must (at the very least) compute the clip frame coordinates followed by 
  // assembly into triangles ...
  glBindVertexArray(s_vao_hdl);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ebo_hdl);
  glDrawElements(GL_TRIANGLES, s_index_element_count, GL_UNSIGNED_SHORT, 0);

  // programming tip: always reset state - application will be easier to debug ...
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);

  // put the stuff we've been drawing onto the display
  //glfwSwapBuffers(pWindow);
}

/*  _________________________________________________________________________*/
/*! update

@param GLFWwindow*
Handle to window that defines the OpenGL context

@return none

Update stuff - that is, figure out where and how to draw ...
For now, there are no objects to animate nor any other parameters to update.
We just use GLFW to process events (keyboard, mouse button click, mouse
movement, and mouse scroller) that have occurred and call the appropriate
callback function.
*/

/******************************************************************************
 *                            Change Log
 * created static variable to contain the rotations for each quadrant
 *****************************************************************************/
static void update(GLFWwindow *pWindow) 
{
  static int rotation = 0;
  glm::vec3 rotations[] = 
  {
    glm::vec3(1.0f, 0.0f, 0.0f), //x axis : top left
    glm::vec3(0.0f, 1.0f, 0.0f), //y axis : top right
    glm::vec3(0.0f, 0.0f, 1.0f), //z axis : bot left
    glm::vec3(1.0f, 1.0f, 1.0f)  //diagon : bot right
  };

  // time between previous and current frame
  double delta_time = update_time(s_frames_per_second, 1.0); 
  // update other events like input handling 
  glfwPollEvents();

  // to compute perspective transform, get viewport's current width and height
  // note: the user can always change the window/viewport size and therefore
  // these values must be obtained every frame ...
  GLsizei fb_width, fb_height;
  glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);

  // position in world frame is fixed ...
  const glm::vec3 cube_world_position(0.f, 0.f, -10.f); 
  // scale parameters are fixed ...
  const glm::vec3 cube_scale_factors(3.f, 3.f, 3.f);
  // orientation axis is fixed ...
  const glm::vec3 s_cube_orientation_axis = 
                            glm::normalize(rotations[rotation]);
  rotation++;
  rotation %= 4;

  // angular speed is fixed at 8 rpm / minute ...
  const GLfloat s_ang_speed_per_second = (8.f*360.f) / 60.f;

  // how much to rotate?
  static GLfloat s_angular_displacement = 0.f; // current angular displacement
  // angular displacement = time * angular velocity ...
  if (s_cube_animate_flag) {
    s_angular_displacement += 
                    static_cast<float>(delta_time)*s_ang_speed_per_second;
  }
  // compute model-to-world-to-view-clip transformation matrix ...
  // compute perspective transform matrix: https://bit.ly/2NNesfW

  glm::mat4 proj_mtx = glm::perspective(glm::radians(100.f), (GLfloat)fb_width / fb_height, 1.f, 200.f);
                                        
  // compute view transform matrix: https://bit.ly/2p580mr
  glm::mat4 view_mtx = glm::lookAt(glm::vec3(0.f, 0.f, 0.1f), 
                                   glm::vec3(0.f, 0.f, 0.f),
                                   glm::vec3(0.f, 1.f, 0.f));
  s_cube_mvp_xform = proj_mtx * view_mtx;
  s_cube_mvp_xform = glm::translate(s_cube_mvp_xform, cube_world_position);
  s_cube_mvp_xform = glm::rotate(s_cube_mvp_xform, glm::radians(s_angular_displacement), s_cube_orientation_axis);
  s_cube_mvp_xform = glm::scale(s_cube_mvp_xform, cube_scale_factors);
}

/*  _________________________________________________________________________*/
/*! cleanup

@param GLFWwindow*
Handle to window that defines the OpenGL context

@return none

Return resources back to GL, glew and GLFW
For now, there are no resources allocated by the application program.
The only task is to have GLFW return resources back to the system and
gracefully terminate.
*/
static void cleanup(GLFWwindow *pwin) {
  // delete all vbo's attached to s_vao_hdl
  GLint max_vtx_attrib = 0;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vtx_attrib);
  glBindVertexArray(s_vao_hdl);
  for (int i = 0; i < max_vtx_attrib; ++i) {
    GLuint vbo_handle = 0;
    glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, 
                        &vbo_handle);
    if (vbo_handle > 0) {
      glDeleteBuffers(1, &vbo_handle);
    }
  }
  glDeleteBuffers(1, &s_vao_hdl);
  glDeleteBuffers(1, &s_ebo_hdl);
  glDeleteProgram(s_shaderpgm_hdl);
  glfwDestroyWindow(pwin);
  glfwTerminate();
}

/*  _________________________________________________________________________*/
/*! init_cube

@param none

@return none

First, the function uses a VBO to transfer cube geometry (vertex position
and color) from client to server memory.
Second, the function updates s_vao_hdl - an object defined at file scope -
with the VAO associated with this cube.
Third, the function uses an element buffer object to transfer cube
triangle information from client to serve memory. s_ebo_hdl - an object 
defined at file scope - is updated with the element buffer information.
*/
static void init_cube() {
  // client-side position coordinates and corresponding colors of the eight vertices ...
  GLuint const vertex_count = 8;
  glm::vec3* ptr_cube_pos = new glm::vec3[vertex_count];
  ptr_cube_pos[0] = glm::vec3(1.f, -1.f, 1.f);  
  ptr_cube_pos[1] = glm::vec3(1.f, 1.f, 1.f);
  ptr_cube_pos[2] = glm::vec3(-1.f, 1.f, 1.f);  
  ptr_cube_pos[3] = glm::vec3(-1.f, -1.f, 1.f);
  ptr_cube_pos[4] = glm::vec3(1.f, -1.f, -1.f); 
  ptr_cube_pos[5] = glm::vec3(1.f, 1.f, -1.f);
  ptr_cube_pos[6] = glm::vec3(-1.f, 1.f, -1.f); 
  ptr_cube_pos[7] = glm::vec3(-1.f, -1.f, -1.f);

  glm::vec3* ptr_cube_clr = new glm::vec3[8];
  ptr_cube_clr[0] = glm::vec3(1.f, 0.f, 0.f); 
  ptr_cube_clr[1] = glm::vec3(0.f, 1.f, 0.f);
  ptr_cube_clr[2] = glm::vec3(0.f, 0.f, 1.f); 
  ptr_cube_clr[3] = glm::vec3(0.f, 0.f, 1.f);
  ptr_cube_clr[4] = glm::vec3(0.f, 1.f, 1.f); 
  ptr_cube_clr[5] = glm::vec3(0.f, 1.f, 1.f);
  ptr_cube_clr[6] = glm::vec3(1.f, 0.f, 1.f); 
  ptr_cube_clr[7] = glm::vec3(1.f, 1.f, 0.f);

  // triangle topology of cube specified in element array buffer object ...
  GLushort* ptr_cube_idx = new GLushort[s_index_element_count];
  GLuint i = 0;
  ptr_cube_idx[i++] = 0;  ptr_cube_idx[i++] = 1;  ptr_cube_idx[i++] = 2;
  ptr_cube_idx[i++] = 0;  ptr_cube_idx[i++] = 2;  ptr_cube_idx[i++] = 3;
  ptr_cube_idx[i++] = 1;  ptr_cube_idx[i++] = 0;  ptr_cube_idx[i++] = 4;
  ptr_cube_idx[i++] = 1;  ptr_cube_idx[i++] = 4;  ptr_cube_idx[i++] = 5;

  ptr_cube_idx[i++] = 2; ptr_cube_idx[i++] = 1; ptr_cube_idx[i++] = 5;
  ptr_cube_idx[i++] = 2; ptr_cube_idx[i++] = 5; ptr_cube_idx[i++] = 6;
  ptr_cube_idx[i++] = 7; ptr_cube_idx[i++] = 3; ptr_cube_idx[i++] = 2;
  ptr_cube_idx[i++] = 7; ptr_cube_idx[i++] = 2; ptr_cube_idx[i++] = 6;

  ptr_cube_idx[i++] = 5; ptr_cube_idx[i++] = 4; ptr_cube_idx[i++] = 7;
  ptr_cube_idx[i++] = 5; ptr_cube_idx[i++] = 7; ptr_cube_idx[i++] = 6;
  ptr_cube_idx[i++] = 3; ptr_cube_idx[i++] = 7; ptr_cube_idx[i++] = 4;
  ptr_cube_idx[i++] = 4; ptr_cube_idx[i++] = 0; ptr_cube_idx[i++] = 3;

#if 0 // works for all OpenGL versions
  // A buffer object is memory that the OpenGL server allocates and owns.
  // Almost all data passed into OpenGL is done by storing the data in
  // a buffer object.
  GLuint vbo_hdl;
  glGenBuffers(1, &vbo_hdl);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_hdl);
  // allocate server-side memory
  glBufferData(GL_ARRAY_BUFFER,
    sizeof(glm::vec3) * vertex_count + sizeof(glm::vec3) * vertex_count,
    nullptr, GL_STATIC_DRAW);
  // copy position coordinates from client to server
  glBufferSubData(GL_ARRAY_BUFFER, 0,
    sizeof(glm::vec3) * vertex_count, ptr_cube_pos);
  // copy color coordinates from client to serve
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count,
    sizeof(glm::vec3) * vertex_count, ptr_cube_clr);
  // since vertex data has now been transferred to server, memory storage
  // can be released
  delete[] ptr_cube_pos;
  delete[] ptr_cube_clr;

  // A vertex array object (VAO) encapsulates information about vertex data
  // stored in buffer objects:
  // what is the format of the vertex attribute associated with a particular
  // index?
  // where is the vertex attribute located in the buffer object?
  // what is the layout of the vertex attribute data in the buffer object?
  glGenVertexArrays(1, &s_vao_hdl);
  glBindVertexArray(s_vao_hdl);

  glEnableVertexAttribArray(0); // enable index 0 - vertex position attribute
  // this function specifies the data format of the vertex atrribute (0)
  // associated with the buffer currently bound (which in our case is
  // GL_ARRAY_BUFFER)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(1); // enable index 1 - vertex color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
          reinterpret_cast<GLvoid*>(sizeof(glm::vec3) * vertex_count));

  // programming tip: good habit to reset any state changes - 
  // will help a lot with debugging ...
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // now, create buffer object to store triangle (or vertex index)
  // information
  glGenBuffers(1, &s_ebo_hdl);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ebo_hdl);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    sizeof(GLushort) * s_index_element_count,
    reinterpret_cast<GLvoid*>(ptr_cube_idx), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  // don't need this data anymore on the client since it has been transferred
  // to the server ...
  delete[] ptr_cube_idx;

#else // works only for OpenGL version 4.5 or greater
  // use vertex buffer object to transfer position and color coordinates 
  // to server-side memory ...
  GLuint vbo_hdl;
  glCreateBuffers(1, &vbo_hdl); // glGenBuffers + glBindBuffer
  // allocate server-side memory for both vertex attribute and vertex indices
  glNamedBufferStorage(vbo_hdl, // same as glBufferData
    sizeof(glm::vec3) * vertex_count +  sizeof(glm::vec3) * vertex_count,
    nullptr, GL_DYNAMIC_STORAGE_BIT);

  // transfer vertex position coordinates from client to server memory
  glNamedBufferSubData(vbo_hdl, 0,  // same as glBufferSubData
    sizeof(glm::vec3) * vertex_count, ptr_cube_pos);
  // transfer vertex position coordinates from client to server memory
  glNamedBufferSubData(vbo_hdl, sizeof(glm::vec3) * vertex_count,
    sizeof(glm::vec3) * vertex_count, ptr_cube_clr);
  // since vertex data has now been transferred to server, memory storage
  // can be released
  delete[] ptr_cube_pos;
  delete[] ptr_cube_clr;

  // A vertex array object (VAO) encapsulates information about vertex data
  // stored in buffer objects:
  // what is the format of the vertex attribute associated with a particular
  // index?
  // where is the vertex attribute located in the buffer object?
  // what is the layout of the vertex attribute data in the buffer object?
  glCreateVertexArrays(1, &s_vao_hdl); // glGenVertexArrays + glBindVertexArray

  // attach VBO with vertex position attribute to VAO binding 0
  glVertexArrayVertexBuffer(s_vao_hdl, 0, vbo_hdl, 0, sizeof(glm::vec3));
  // attach VBO with vertex color attribute to VAO binding 1
  glVertexArrayVertexBuffer(s_vao_hdl, 1, vbo_hdl,
    sizeof(glm::vec3) * vertex_count, sizeof(glm::vec3));
  glEnableVertexArrayAttrib(s_vao_hdl, 0); // enable index 0: position attribute
  glEnableVertexArrayAttrib(s_vao_hdl, 1); // enable index 1: color attribute
  // data format for vertex position attributes
  glVertexArrayAttribFormat(s_vao_hdl, 0, 3, GL_FLOAT, GL_FALSE, 0);
  // data format for vertex color attributes
  glVertexArrayAttribFormat(s_vao_hdl, 1, 3, GL_FLOAT, GL_FALSE, 0);
  // associate VAO binding 0 to vertex attribute index 0
  glVertexArrayAttribBinding(s_vao_hdl, 0, 0); 
  // associate VAO binding 1 to vertex attribute index 1
  glVertexArrayAttribBinding(s_vao_hdl, 1, 1);

  // buffer for vertex indices 
  glCreateBuffers(1, &s_ebo_hdl);
  glNamedBufferData(s_ebo_hdl, sizeof(GLushort)*s_index_element_count,
    reinterpret_cast<GLvoid*>(ptr_cube_idx), GL_STATIC_DRAW);
  glVertexArrayElementBuffer(s_vao_hdl, s_ebo_hdl);

  // don't need this data anymore on the client since it has been transferred
  // to the server ...
  delete[] ptr_cube_idx;

#endif
}

/*  _________________________________________________________________________*/
/*! update_time

@param double&
fps: computed frames per second

@param double
fps_calc_interval: the interval (in seconds) at which fps is to be
calculated

@return double
Return time interval (in seconds) between previous and current frames

This function is first called in init() and once each game loop by update(). 
It uses GLFW's time functions to:
1. compute interval in seconds between each frame
2. compute the frames per second every "fps_calc_interval" seconds
*/
double update_time(double &fps, double fps_calc_interval) {
  // get elapsed time (in seconds) between previous and current frames
  static double prev_time = glfwGetTime();
  double curr_time = glfwGetTime();
  double delta_time = curr_time - prev_time;
  prev_time = curr_time;

  // fps calculations
  static double count = 0.0; // number of game loop iterations
  static double start_time = glfwGetTime();
  // get elapsed time since very beginning (in seconds) ...
  double elapsed_time = curr_time - start_time;

  ++count;

  // update fps at least every 10 seconds ...
  fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
  fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;
  if (elapsed_time > fps_calc_interval) {
    fps = count / elapsed_time;
    start_time = curr_time;
    count = 0.0;
  }

  // done calculating fps ...
  return delta_time;
}

/*  _________________________________________________________________________ */
/*  _________________________________________________________________________ */
/*! compile_and_link_shaders

@param none

@return none

This function updates s_shaderpgm_hdl - the shader program handle defined at
file scope. If compilation of source file or linking of object code or the
executable cannot be validated, diagnostic messages are printed. Otherwise,
s_shaderpgm_hdl is updated with a handle to a shader program object that
contains executable vertex and fragment shader programs.
*/
void compile_and_link_shaders() {
  // create and bind vertex shader object
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  // source code for vertex shader as array of one C-string constant
  // the size of the rendered point (in pixels) is specified by setting the
  // value of the gl_PointSize member of the gl_PerVertex structure ...
  // Both versions work - in the first, we have a single C-string while the
  // 2nd version has many C-strings - one per line of the shader source.
  static GLchar const* vertex_shader_source[] = {
    "#version 450 core                                   \n"
    "                                                    \n"
    "layout (location=0) in vec3 vVertexPosition;        \n"
    "layout (location=1) in vec3 vVertexColor;           \n"
    "                                                    \n"
    "out vec3 vSmoothColor;                              \n"
    "                                                    \n"
    "uniform mat4 uMVP;                                  \n"
    "                                                    \n"
    "void main () {                                      \n"
    "  gl_Position = uMVP * vec4(vVertexPosition, 1.0f);\n"
    "  vSmoothColor = vVertexColor;                      \n"
    "}                                                   \n"
  };

  // copy vertex shader source from array (of one string) to vertex shader object
  glShaderSource(vertex_shader, 
                 sizeof(vertex_shader_source)/sizeof(GLchar const*),
                 vertex_shader_source, NULL);
  // compile vertex shader source ...
  glCompileShader(vertex_shader);

  // verify compilation status of vertex shader source
  std::string diag_msg;
  GLboolean status_flag{ shader_compile_status(vertex_shader, diag_msg) };
  if (GL_FALSE == status_flag) {
    std::cout << "Vertex shader failed - diagnostic message: " << diag_msg << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // source code for fragment shader as array of one C-string constant
  // both versions are correct
  // however, the second version is shorter
  static GLchar const* fragment_shader_source[] = {
    "#version 450 core                            \n"
    "                                             \n"
    "in vec3 vSmoothColor;                        \n"
    "                                             \n"
    "void main () {                               \n"
    "	gl_FragColor = vec4(vSmoothColor, 1.0);     \n"
    "}                                            \n"
  };

  // create and bind fragment shader object
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  // copy fragment shader source from array (of one string) to fragment shader object
  glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
  // compile fragment shader source ...
  glCompileShader(fragment_shader);

  // verify compilation status of fragment shader source
  status_flag = shader_compile_status(fragment_shader, diag_msg);
  if (GL_FALSE == status_flag) {
    std::cout << "Fragment shader failed - diagnostic message: " << diag_msg << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // create (uninitialized) pointer to linked shader program object
  s_shaderpgm_hdl = glCreateProgram();
  // attach vertex and fragment shader objects to this shader program object
  glAttachShader(s_shaderpgm_hdl, vertex_shader);
  glAttachShader(s_shaderpgm_hdl, fragment_shader);
  // now link compiled shaders into a program
  glLinkProgram(s_shaderpgm_hdl);
  
  // query status of link operation
  status_flag = shader_pgm_link_status(s_shaderpgm_hdl, diag_msg);
  if (GL_FALSE == status_flag) {
    std::cout << "Shader program failed - diagnostic message: " << diag_msg << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // now that we have linked the object code, delete the object code ...
  glDetachShader(s_shaderpgm_hdl, vertex_shader);
  glDeleteShader(vertex_shader);
  glDetachShader(s_shaderpgm_hdl, fragment_shader);
  glDeleteShader(fragment_shader);

  // now validate the shader program ...
  glValidateProgram(s_shaderpgm_hdl);
  GLint status;
  glGetProgramiv(s_shaderpgm_hdl, GL_VALIDATE_STATUS, &status);
  if (GL_FALSE == status) {
    std::cout << "Failed to validate shader program for current OpenGL context"
      << std::endl;
    GLint log_len;
    glGetProgramiv(s_shaderpgm_hdl, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) {
      char *log_str = new char[log_len];
      GLsizei written_log_len;
      glGetProgramInfoLog(s_shaderpgm_hdl, log_len, &written_log_len, log_str);
      std::cout << "Log information: " << log_str << std::endl;
      delete[] log_str;
      std::exit(EXIT_FAILURE);
    }
  }

  // now, print vertex attributes (user-defined input values to vertex shaders)
  GLint max_attr_length, num_attribs;
  glGetProgramiv(s_shaderpgm_hdl,
                  GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,  &max_attr_length);
  glGetProgramiv(s_shaderpgm_hdl, GL_ACTIVE_ATTRIBUTES, &num_attribs);
  GLchar *vtx_attr_name = new GLchar[max_attr_length];
  std::cout << "Index\t|\tName\n";
  std::cout << "-----------------------------------------------------------\n";
  for (int i = 0; i < num_attribs; ++i) {
    GLsizei written;
    GLint size;
    GLenum type;
    glGetActiveAttrib(s_shaderpgm_hdl, i, max_attr_length,
                      &written, &size, &type, vtx_attr_name);
    GLint loc = glGetAttribLocation(s_shaderpgm_hdl, vtx_attr_name);
    std::cout << loc << "\t\t" << vtx_attr_name << std::endl;
  }
  std::cout << "-----------------------------------------------------------\n";
  delete[] vtx_attr_name;

  // finally, print the uniforms passed to vertex shaders by the application
  GLint max_uni_length;
  glGetProgramiv(s_shaderpgm_hdl, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uni_length);
  GLchar *vtx_uni_name = new GLchar[max_uni_length];
  GLint num_uniforms;
  glGetProgramiv(s_shaderpgm_hdl, GL_ACTIVE_UNIFORMS, &num_uniforms);
  std::cout << "Location\t|\tName\n";
  std::cout << "-----------------------------------------------------------\n";
  for (int i = 0; i < num_uniforms; ++i) {
    GLsizei written;
    GLint size;
    GLenum type;
    glGetActiveUniform(s_shaderpgm_hdl, i, max_uni_length, 
                      &written, &size, &type, vtx_uni_name);
    GLint loc = glGetUniformLocation(s_shaderpgm_hdl, vtx_uni_name);
    std::cout << loc << "\t\t" << vtx_uni_name << std::endl;
  }
  std::cout << "-----------------------------------------------------------\n";
  delete[] vtx_uni_name;
}

/*  _________________________________________________________________________*/
/*! shader_compile_status

@param GLuint
Handle to compiled shader code

@param std::string&
Reference to std::string object that will be updated with (any) diagnostic
messages from the information log associated with the shader object

@return GLboolean
Returns GL_TRUE if the shader source was successfully compiled, otherwise
the function returns GL_FALSE and add any diagnostic messages from the
GLSL compiler into the 2nd parameter.
*/
static GLboolean shader_compile_status(GLuint shader_hdl, std::string& diag_msg) {
  GLint result;
  glGetShaderiv(shader_hdl, GL_COMPILE_STATUS, &result);
  if (GL_FALSE == result) {
    GLint log_len;
    glGetShaderiv(shader_hdl, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) {
      char *error_log_str = new GLchar[log_len];
      GLsizei written_log_len;
      glGetShaderInfoLog(shader_hdl, log_len, &written_log_len, error_log_str);
      diag_msg = error_log_str;
      delete[] error_log_str;
    }
    return GL_FALSE;
  }
  return GL_TRUE;
}

/*  _________________________________________________________________________*/
/*! shader_pgm_link_status

@param GLuint
Handle to compiled shader program

@param std::string&
Reference to std::string object that will be updated with (any) diagnostic
messages from the information log associated with the shader program

@return GLboolean
Returns GL_TRUE if the shader objects were successfully linked, otherwise
the function returns GL_FALSE and add any diagnostic messages from the
GLSL linker into the 2nd parameter.
*/
static GLboolean shader_pgm_link_status(GLuint program_hdl, std::string& diag_msg) {
  GLint result;
  glGetProgramiv(program_hdl, GL_LINK_STATUS, &result);
  if (GL_FALSE == result) {
    GLint log_len;
    glGetProgramiv(program_hdl, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) {
      char *error_log_str = new GLchar[log_len];
      GLsizei written_log_len;
      glGetProgramInfoLog(program_hdl, log_len, &written_log_len, error_log_str);
      diag_msg = error_log_str;
      delete[] error_log_str;
    }
    return GL_FALSE;
  }
  return GL_TRUE;
}

/*  _________________________________________________________________________ */
/*! create_gl_context

@param GLuint
width of the framebuffer attached to the new OpenGL context

@param GLuint
height of the framebuffer attached to the new OpenGL context

@param std::string
application title to be printed to the window attached to the new
OpenGL context

@return GLFWwindow* - return a handle to a window of size width x height pixels
and its associated OpenGL context that matches a core profile that is
compatible with OpenGL 4.5 and doesn't support "old" OpenGL, has 32-bit RGBA,
double-buffered color buffer, 24-bit depth buffer and 8-bit stencil buffer

Uses GLFW to create an OpenGL context.
GLFW's initialization follows from here: http://www.glfw.org/docs/latest/quick.html
*/
static GLFWwindow* create_gl_context(GLuint fbwd,
                                    GLuint fbht,
                                    std::string wintitle) {
  if (!glfwInit()) {
    log_to_debug_file(g_logfilename,
      "ERROR: Initialization of GLFW has failed - program aborted");
    std::exit(EXIT_FAILURE);
  } else {
    // write GLFW3 version number to debug log file
    log_to_debug_file(g_logfilename, "GLFW Version: ", glfwGetVersionString());
  }
  // In case a GLFW function fails, an error is reported to callback function
  glfwSetErrorCallback(glfw_error_cb);

  // Before asking GLFW to create an OpenGL context, we specify the minimum
  // constraints in that context:
  // specify OpenGL version 4.5
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  // specify modern OpenGL only - no compatibility with "old" OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // applications will be double-buffered ...
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  // default behavior: colorbuffer is 32-bit RGBA, depthbuffer is 24-bits

  // size of viewport: 2400 x 1350
  GLFWwindow *pwin = glfwCreateWindow(fbwd, fbht, wintitle.c_str(), NULL, NULL);
  if (!pwin) {
    log_to_debug_file(g_logfilename,
      "ERROR: GLFW is unable to create an OpenGL context.\n");
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // set callback for events associated with window size changes; keyboard;
  // mouse buttons, cursor position, and scroller
  glfwSetFramebufferSizeCallback(pwin, fbsize_cb);
  glfwSetKeyCallback(pwin, key_cb);
  glfwSetMouseButtonCallback(pwin, mousebutton_cb);
  glfwSetCursorPosCallback(pwin, mousepos_cb);
  glfwSetScrollCallback(pwin, mousescroll_cb);

  // make the previously created OpenGL context current ...
  glfwMakeContextCurrent(pwin);

  // this is the default setting ...
  glfwSetInputMode(pwin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  return pwin;
}

/*  _________________________________________________________________________*/
/*! init_gl_loader

@param none

@return bool
Return true if system is able to support appropriate GL version.
Otherwise, return false.

Uses GLEW API to expose OpenGL core and extension functionality
GLEW's initialization follows from here: http://glew.sourceforge.net/basic.html

Within this function, debug information is logged to a file whose name is
defined at file scope by object g_logfilename (of type std::string)
*/
static bool init_gl_loader() {
  // initialize OpenGL (and extension) function loading library
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    log_to_debug_file(g_logfilename,
      "ERROR: Unable to initialize GLEW ", glewGetErrorString(err));
    return false;
  }
  if (GLEW_VERSION_4_5) { // check support for core GL 4.5
    log_to_debug_file(g_logfilename, "GLEW Version: ", glewGetString(GLEW_VERSION));
    log_to_debug_file(g_logfilename, "Graphics driver supports OpenGL version 4.5\n");
    return true;
  } else {
    log_to_debug_file(g_logfilename, "ERROR: System doesn't support GL 4.5 API\n");
    return false;
  }
}

/*  _________________________________________________________________________*/
/*! glfw_error_cb

@param int
GLFW error code

@parm char const*
Human-readable description of the code

@return none

The error callback receives a human-readable description of the error and
(when possible) its cause.
*/
static void glfw_error_cb(int error, char const* description) {
  log_to_debug_file(g_logfilename,
    "GLFW Error id: ",
    error,
    " | description: ",
    description);
}

/*  _________________________________________________________________________*/
/*! fbsize_cb

@param GLFWwindow*
Handle to window that is being resized

@parm int
Width in pixels of new window size

@parm int
Height in pixels of new window size

@return none

This function is called when the window is resized - it receives the new size
of the window in pixels.
*/
static void fbsize_cb(GLFWwindow *pwin, int width, int height) {
  // use the entire framebuffer as drawing region
  glViewport(0, 0, width, height);
  // later, we'll have to set the projection matrices here ...
}

/*  _________________________________________________________________________*/
/*! key_cb

@param GLFWwindow*
Handle to window that is receiving event

@param int
the keyboard key that was pressed or released

@parm int
Platform-specific scancode of the key

@parm int
GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
action will be GLFW_KEY_UNKNOWN if GLFW lacks a key token for it,
for example E-mail and Play keys.

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when keyboard buttons are pressed.
When the ESC key is pressed, the close flag of the window is set.
*/
void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod) {
  if (GLFW_PRESS == action) {
#ifdef _DEBUG
    std::cout << "Key pressed" << std::endl;
#endif
  } else if (GLFW_REPEAT == action) {
#ifdef _DEBUG
    std::cout << "Key repeatedly pressed" << std::endl;
#endif
  } else if (GLFW_RELEASE == action) {
#ifdef _DEBUG
    std::cout << "Key released" << std::endl;
#endif
  }

  if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
    glfwSetWindowShouldClose(pwin, GLFW_TRUE);
  }
  if (key == GLFW_KEY_U && action == GLFW_PRESS) {
    s_cube_animate_flag ^= true;
  }
}

/*  _________________________________________________________________________*/
/*! mousebutton_cb

@param GLFWwindow*
Handle to window that is receiving event

@param int
the mouse button that was pressed or released
GLFW_MOUSE_BUTTON_LEFT and GLFW_MOUSE_BUTTON_RIGHT specifying left and right
mouse buttons are most useful

@parm int
action is either GLFW_PRESS or GLFW_RELEASE

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when mouse buttons are pressed.
*/
static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod) {
  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT:
#ifdef _DEBUG
    std::cout << "Left mouse button ";
#endif
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
#ifdef _DEBUG
    std::cout << "Right mouse button ";
#endif
    break;
  }
  switch (action) {
  case GLFW_PRESS:
#ifdef _DEBUG
    std::cout << "pressed!!!" << std::endl;
#endif
    break;
  case GLFW_RELEASE:
#ifdef _DEBUG
    std::cout << "released!!!" << std::endl;
#endif
    break;
  }
}

/*  _________________________________________________________________________*/
/*! mousepos_cb

@param GLFWwindow*
Handle to window that is receiving event

@param double
new cursor x-coordinate, relative to the left edge of the client area

@param double
new cursor y-coordinate, relative to the top edge of the client area

@return none

This functions receives the cursor position, measured in screen coordinates but
relative to the top-left corner of the window client area.
*/
static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos) {
#ifdef _DEBUG
  std::cout << "Mouse cursor position: (" << xpos << ", " << ypos << ")" << std::endl;
#endif
}

/*  _________________________________________________________________________*/
/*! mousescroll_cb

@param GLFWwindow*
Handle to window that is receiving event

@param double
Scroll offset along X-axis

@param double
Scroll offset along Y-axis

@return none

This function is called when the user scrolls, whether with a mouse wheel or
touchpad gesture. Although the function receives 2D scroll offsets, a simple
mouse scroll wheel, being vertical, provides offsets only along the Y-axis.
*/
static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset) {
#ifdef _DEBUG
  std::cout << "Mouse scroll wheel offset: ("
    << xoffset << ", " << yoffset << ")" << std::endl;
#endif
}

/*  _________________________________________________________________________*/
/*! create_log_file
Starts a new debug log file with current time and data stamps followed by
timestamp of application build.

@param none

@return bool
Returns true if debug log file was successfully created, false otherwise.

The function writes to a file whose name is defined at file scope by
object g_logfilename (of type std::string)
*/
bool create_log_file() {
  std::ofstream ofs(g_logfilename.c_str(), std::ofstream::out);
  if (!ofs) {
    std::cerr << "ERROR: could not open log file "
      << g_logfilename << " for writing" << std::endl;
    return false;
  }

  std::time_t curr_time = time(nullptr); // get current time
  // convert current time to C-string format
  ofs << "OpenGL Application Log File local time: " << std::ctime(&curr_time);
  ofs << "Build version: " << __DATE__ << " " << __TIME__ << std::endl << std::endl;
  ofs.close();
  return true;
}

/*  _________________________________________________________________________*/
/*! query_gl_context

@param none

@return none

Prints current hardware capabilities relevant to OpenGL and GLSL.
The function writes to a file whose name is defined at file scope by
object g_logfilename (of type std::string)
*/
static void query_gl_context() {
  std::pair<GLenum, std::string> const params[] = {
    { GL_VENDOR, std::string("GL_VENDOR") },
  { GL_RENDERER, std::string("GL_RENDERER") },
  { GL_VERSION, std::string("GL_VERSION") },
  { GL_SHADING_LANGUAGE_VERSION, std::string("GL_SHADING_LANGUAGE_VERSION") }, // 3

  { GL_MAJOR_VERSION, std::string("GL_MAJOR_VERSION") },
  { GL_MINOR_VERSION, std::string("GL_MINOR_VERSION") },
  { GL_MAX_ELEMENTS_VERTICES, std::string("GL_MAX_ELEMENTS_VERTICES") },
  { GL_MAX_ELEMENTS_INDICES, std::string("GL_MAX_ELEMENTS_INDICES") },
  { GL_MAX_GEOMETRY_OUTPUT_VERTICES, std::string("GL_MAX_GEOMETRY_OUTPUT_VERTICES") },
  { GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, std::string("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS") },
  { GL_MAX_CUBE_MAP_TEXTURE_SIZE, std::string("GL_MAX_CUBE_MAP_TEXTURE_SIZE") },
  { GL_MAX_DRAW_BUFFERS, std::string("GL_MAX_DRAW_BUFFERS") },
  { GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, std::string("GL_MAX_FRAGMENT_UNIFORM_COMPONENTS") },
  { GL_MAX_TEXTURE_IMAGE_UNITS, std::string("GL_MAX_TEXTURE_IMAGE_UNITS") },
  { GL_MAX_TEXTURE_SIZE, std::string("GL_MAX_TEXTURE_SIZE") },
  { GL_MAX_VARYING_FLOATS, std::string("GL_MAX_VARYING_FLOATS") },
  { GL_MAX_VERTEX_ATTRIBS, std::string("GL_MAX_VERTEX_ATTRIBS") },
  { GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, std::string("GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS") },
  { GL_MAX_VERTEX_UNIFORM_COMPONENTS, std::string("GL_MAX_VERTEX_UNIFORM_COMPONENTS") }, // 18
  { GL_MAX_VIEWPORT_DIMS, std::string("GL_MAX_VIEWPORT_DIMS") }, // 19
  { GL_STEREO, std::string("GL_STEREO") }
  };

  // C-strings for 1st four parameters
  log_to_debug_file(g_logfilename, "GL version information and context parameters:");
  int i = 0;
  for (i = 0; i < 4; ++i) {
    log_to_debug_file(g_logfilename, params[i].second, glGetString(params[i].first));
  }

  // one integer for next set of fifteen parameters
  for (; i < 19; ++i) {
    GLint val;
    glGetIntegerv(params[i].first, &val);
    log_to_debug_file(g_logfilename, params[i].second, val);
  }

  // two integers for next parameter
  GLint dim[2];
  glGetIntegerv(params[19].first, dim);
  log_to_debug_file(g_logfilename, params[19].second, dim[0], dim[1]);

  // bool for next parameter
  GLboolean flag;
  glGetBooleanv(params[20].first, &flag);
  log_to_debug_file(g_logfilename, params[20].second, static_cast<GLint>(flag));

  log_to_debug_file(g_logfilename, "-----------------------------");

  // enumerate extensions
  GLint maxExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &maxExtensions);
  for (int i = 0; i < maxExtensions; ++i) {
    log_to_debug_file(g_logfilename, i + 1, ": ", glGetStringi(GL_EXTENSIONS, i));
  }
}

/*  _________________________________________________________________________*/
/*! init_gl_state

@param none

@return none

OpenGL is a complicated state machine - this function sets certain useful
aspects of the state that will remain unchanged throughout this program's
execution. More specifically, we set the size of the drawing region to be
the entire window and specify the color used to clear the color buffer.
*/
static void init_gl_state(GLFWwindow *pwin) {
  // we'll use the entire window as viewport ...
  GLint width, height;
  glfwGetFramebufferSize(pwin, &width, &height);
  fbsize_cb(pwin, width, height);
             
  // clear color (frame) buffer with this (background) color
  glClearColor(0.5f, 0.5f, 0.5f, 1.f);
  // enable depth-buffer (or, Z-buffer) algorithm: https://bit.ly/2v8FYrM
  glEnable(GL_DEPTH_TEST);
  // clear depth buffer with maximum possible depth 
  glClearDepth(1.0);

  // enable back-face culling - see: https://bit.ly/2NenwL5
  glEnable(GL_CULL_FACE);
  // we're telling the pipe that front-facing triangles will be counter-
  // clockwise oriented. This implies that a back-facing triangle will
  // be clockwise oriented. The pipe will use this idea of CCW oriented
  // triangles being front-facing (and therefore visible) and CW
  // oriented triangle being back-facing (and therefore invisible)
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);  // cull back faces


}
