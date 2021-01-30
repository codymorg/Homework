/*!
@file    main.cpp
@author  pghali@digipen.edu
@date    10/11/2016

Note: The contents of this file must not be updated by students except for
those changes dictated by the assignment spec. Otherwise, something that works
for you will not work for me. If you want something to be modified, updated,
or altered and it is useful for the entire class, please speak to me.

This file uses functionality defined in type App to initialize an OpenGL
context and implement a game loop.


*//*__________________________________________________________________________*/

/*                                   application-specific preprocessor commands
----------------------------------------------------------------------------- */
#define NOT_USING_PB

/*                                                                   includes
----------------------------------------------------------------------------- */
#include "../include/app.h"
#include "../include/glslshader.h"
#include "../include/pbo.h"
#ifndef NOT_USING_PBO
#endif
#include <glm/glm.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <array>
#include <map>

/*                                                   type declarations
----------------------------------------------------------------------------- */

/*                                                      function declarations
----------------------------------------------------------------------------- */
static void draw(GLFWwindow*);
static void update(GLFWwindow*);

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
App gAPP;
Pbo gPBO;
#ifndef NOT_USING_PBO
#endif

/*                                                      function definitions
----------------------------------------------------------------------------- */
/*  _________________________________________________________________________ */
/*! main

@param none

@return int

Indicates how the program existed. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/
int main() {
  // start with a 16:9 aspect ratio
  if (!gAPP.init(1000,800, "Lab 2: Setting up CS 250 framework")) {
    std::cout << "Unable to create OpenGL context" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  gPBO.init(App::width, App::height);
#ifndef NOT_USING_PBO
#endif

  // window's close flag is set by clicking close widget or Alt+F4
  while (!glfwWindowShouldClose(App::ptr_window)) {
    draw(App::ptr_window);
    update(App::ptr_window);
  }

#ifndef NOT_USING_PBO
  gPBO.cleanup();
#endif
  gAPP.cleanup();
}

/*  _________________________________________________________________________ */
/*! draw

@param GLApp::GLFWwindow*
Handle to window that defines the OpenGL context

@return none

For now, there's nothing to draw - just paint colorbuffer with constant color
*/
static void draw(GLFWwindow *ptr_win) {
  // tell OpenGL to clear the colorbuffer with RGBA value supplied
  // by glClearColor ...
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.f, 0.f, 0.f, 1.f);

  gPBO.draw_fullwindow_quad();
#ifndef NOT_USING_PBO
#endif

  // swap buffers: front <-> back
  glfwSwapBuffers(ptr_win);
}

/*  _________________________________________________________________________ */
/*! update

@param GLFWwindow*
Handle to window that defines the OpenGL context

@return none

For now, there are no objects to animate nor keyboard, mouse button click,
mouse movement, and mouse scroller events to be processed.
The only event triggered is window resize.
*/
static void update(GLFWwindow *ptr_win) {
  // time between previous and current frame
  double delta_time = gAPP.update_time(1.0);
  // write window title with current fps ...
  std::stringstream sstr;
  sstr << std::fixed << std::setprecision(2) << App::title << " | fps: " << App::fps <<
    " | Rendering " << App::width << " x " << App::height << " pixels";
  glfwSetWindowTitle(App::ptr_window, sstr.str().c_str());

  glfwPollEvents();

#ifndef NOT_USING_PBO
  gPBO.render();
#endif
}
