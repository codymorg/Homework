/******************************************************************************
  Cody Morgan
  Lab 1

  pixel buffer object (PBO) draws to texture and to the screen 

  inplemented:
  * set_color          
      sets the pixel buffer with a given color

  * clear_color_buffer
      draws the PBO

  * draw_fullwindow_quad
      draw the mesh

******************************************************************************/

#include "..\include\pbo.h";
#include "..\include\glslshader.h";
#include <dpml.h>

#define STATIC_IMAGE false
bool staticImageRendered = false;


using std::pair;

GLuint      Pbo::pboid;   
GLuint      Pbo::texid;
GLuint      Pbo::vaoid;
Pbo::Color  Pbo::clear_clr;   
Pbo::Color* Pbo::ptr_to_pbo;  
GLsizei     Pbo::byte_cnt;    
GLsizei     Pbo::pixel_cnt;   
GLsizei     Pbo::width;
GLsizei     Pbo::height;
GLSLShader  Pbo::shdr_pgm;

Pbo::Color green(0, 255, 0, 255);
Pbo::Color red(255, 0, 0, 255);


void test(int run = -1)
{
  int width = 500;
  int height = 500;
  static int a = 0;
  static int b = 0;
  static int c = width;
  static int d = height;
  static int count = 0;
  if (run != -1)
    count = run;

  Pbo::render_linedda(a, b, c, d, red);
  Pbo::render_linebresenham(a, b, c, d, green);
  switch (count)
  {
  case 0: 
    d--; 
    break;
  case 1: 
    c--;
    break;
  case 2:
    a = width;
    b = 0;
    c--;
    break;
  case 3: 
    a = width;
    b = 0;
    c = 0;
    d--;
    break;
  case 4:
    a = width;
    b = height;
    c = 0;
    d--;
    break;
  case 5: 
    a++;
    break;
  case 6:
    a = 0;
    b = height;
    d = 0;
    c--;
    break;
  case 7: 
    a = 0;
    b = height;
    c = width;
    d--;
    break;

  default:
    //done
    std::cout << "=====================done\n";
    count = -1;
    a = -1;
    break;
  }
  if (a < 0 || b < 0 || c < 0 || d < 0 || a > width || b > height || c > width || d > height)
  {
    std::cout << "==================ending " << count << "\n";
    a = 0;
    b = 0;
    c = width;
    d = height;
    count++;
  }
}


Model* model = nullptr;

//transform every vert from norm coords to screen coords
void Pbo::transformToScreen()
{
  float halfW = width / 2.0f;
  float halfH = height / 2.0f;

  glm::mat4 scale
  {
    {halfW, 0,     0, 0},
    {0,     halfH, 0, 0},
    {0,     0,     1, 0},
    {0,     0,     0, 1}
  };
  glm::mat4 translate
  {
    {1,     0,     0, 0},
    {0,     1,     0, 0},
    {0,     0,     1, 0},
    {halfW, halfH, 0, 1}
  };
  glm::mat4 transform = translate * scale;


  for (size_t i = 0; i < model->verts.size(); i++)
  {
    model->screen.push_back(transform * glm::vec4(model->verts[i], 1));
  }
}


void Pbo::setup_quad_vao()
{
  glm::vec2 vertices[] =
  {
    glm::vec2(-1, -1), //LB
    glm::vec2(1, -1),  //RB
    glm::vec2(1, 1),   //RT
    glm::vec2(-1, 1),  //LT
    /*
    */
  };

  GLushort indices[] =
  {
    /*
    */
    0, 1, 2,
    0, 2, 3,
  };

  glm::vec2 UVs[] =
  {
    glm::vec2(0, 0), //LB
    glm::vec2(1, 0), //RB
    glm::vec2(1, 1), //RT
    glm::vec2(0, 1), //LT
    /*
    */
  };

  // setup VBO
  GLuint vbo;

  glCreateBuffers(1, &vbo);
  glNamedBufferStorage(
    vbo,                            // create this buffer    
    sizeof(vertices) + sizeof(UVs), // this much data
    nullptr,                        // nullptr
    GL_DYNAMIC_STORAGE_BIT);        // you may update the contents of this buffer 

  //position data
  glNamedBufferSubData(
    vbo,              // load data into this buffer
    0,                // at this offset
    sizeof(vertices), // this big
    vertices);        // this data

  //UV data
  glNamedBufferSubData(
    vbo,               // load data into this buffer
    sizeof(vertices),  // at this offset
    sizeof(UVs),       // this big
    UVs);              // this data

  // setup VAO
  glCreateVertexArrays(1, &vaoid);

  //position
  glVertexArrayVertexBuffer(
    vaoid,
    0,
    vbo,
    0,
    sizeof(glm::vec2));

  //UVs
  glVertexArrayVertexBuffer(
    vaoid,
    1,
    vbo,
    sizeof(vertices),
    sizeof(glm::vec2));

  glEnableVertexArrayAttrib(vaoid, 0); // enable index 0: pos attribute
  glEnableVertexArrayAttrib(vaoid, 1); // enable index 1: uv attribute
  glVertexArrayAttribFormat(
    vaoid,
    0,
    2,
    GL_FLOAT,
    GL_FALSE,
    0);

  glVertexArrayAttribFormat(
    vaoid,
    1,
    2,
    GL_FLOAT,
    GL_FALSE,
    0);

  glVertexArrayAttribBinding(
    vaoid,
    0,
    0);

  glVertexArrayAttribBinding(
    vaoid,
    1,
    1);

  // buffer for vertex indices
  GLuint ebo;
  glCreateBuffers(1, &ebo);

  glNamedBufferData(
    ebo,
    sizeof(indices),
    indices,
    GL_STATIC_DRAW);

  glVertexArrayElementBuffer(vaoid, ebo);

  std::cout << "vao error : " << glGetError() << "\n";
}

void Pbo::setup_shdrpgm()
{
  std::vector<std::pair<GLenum, std::string>> shaders =
  {
    {GL_VERTEX_SHADER, std::string("../shaders/pass-thru-pos2d-tex2d.vert")},
    {GL_FRAGMENT_SHADER, std::string("../shaders/basic-texture.frag")}
  };

  bool ok = shdr_pgm.CompileLinkValidate(shaders);
  if (ok)
    std::cout << "shaders ok\n";
  else
    std::cout << "shader failure\n";

}

void Pbo::init(GLsizei w, GLsizei h)
{
  //init variables in PBO
  width = w;
  height = h;
  pixel_cnt = w * h;
  byte_cnt = sizeof(Pbo::Color) * w * h;


  //init PBO
  glCreateBuffers(
    1,
    &pboid);

  glNamedBufferStorage(
    pboid,                                      // buffer location
    byte_cnt,                                   // size
    nullptr,                                    // copy data from
    GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT); // The contents of the data store may be updated after creation through calls to glBufferSubData.

  // clear PBO
  ptr_to_pbo = reinterpret_cast<Pbo::Color*>(glMapNamedBuffer(pboid, GL_WRITE_ONLY));
  glUnmapNamedBuffer(pboid);


  //init texid
  glCreateTextures(
    GL_TEXTURE_2D, // target
    1,             // count
    &texid);       // where go

  glTextureStorage2D(
    texid,     
    1,           // texture level
    GL_RGBA8,    // format
    width,       
    height);    

  glBindBuffer(
    GL_PIXEL_UNPACK_BUFFER, // type of buffer
    pboid);                 // this buffer

  glTextureSubImage2D(
    texid,            
    0,                // mip level
    0,                // x offset
    0,                // y offset
    width,            
    height,            
    GL_RGBA,          // format 
    GL_UNSIGNED_BYTE, // pixel format
    nullptr);         //nullptr   

  setup_quad_vao();
  setup_shdrpgm();

  // load in model data
  model = new Model;
  /*
  DPML::parse_obj_mesh("../meshes/one-eyed-monster.obj", model->verts, model->norms, model->uv, model->indices);
  DPML::parse_obj_mesh("../meshes/head.obj", model->verts, model->norms, model->uv, model->indices);
  */
  DPML::parse_obj_mesh("../meshes/lab3-test.obj", model->verts, model->norms, model->uv, model->indices);

  transformToScreen();
}

void Pbo::drawModel(Model& model)
{
  // draw every triangle
  for (size_t tri = 0; tri < model.indices.size(); tri+=3)
  {
    //line 1
    int x1 = int(model.screen[model.indices[tri + 0]].x);
    int y1 = int(model.screen[model.indices[tri + 0]].y);
    int x2 = int(model.screen[model.indices[tri + 1]].x);
    int y2 = int(model.screen[model.indices[tri + 1]].y);

   // render_linedda(x1, y1, x2, y2, red);
    render_linebresenham(x1, y1, x2, y2, green);


    //line 2
    x1 = int(model.screen[model.indices[tri + 1]].x);
    y1 = int(model.screen[model.indices[tri + 1]].y);
    x2 = int(model.screen[model.indices[tri + 2]].x);
    y2 = int(model.screen[model.indices[tri + 2]].y);

    //render_linedda(x1, y1, x2, y2, red);
    render_linebresenham(x1, y1, x2, y2, green);

    //line 3
    x1 = int(model.screen[model.indices[tri + 2]].x);
    y1 = int(model.screen[model.indices[tri + 2]].y);
    x2 = int(model.screen[model.indices[tri + 0]].x);
    y2 = int(model.screen[model.indices[tri + 0]].y);

    //render_linedda(x1, y1, x2, y2, red);
    render_linebresenham(x1, y1, x2, y2, green);
  }

}


void Pbo::set_pixel(int x, int y, Pbo::Color color)
{
  if (x < 0 || y < 0 || x >= width || y >= height)
    return;

  int index = x + (y * width);
  ptr_to_pbo[index] = color;
}

void Pbo::set_clear_color(Pbo::Color color)
{
  Pbo::set_clear_color(color.r, color.g, color.b, color.a);
}

//defaults a = 255
void Pbo::set_clear_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
  clear_clr.r = r;
  clear_clr.g = g;
  clear_clr.b = b;
  clear_clr.a = a;
}

void Pbo::clear_color_buffer()
{
  std::fill(Pbo::ptr_to_pbo, Pbo::ptr_to_pbo + pixel_cnt, clear_clr);
}


void Pbo::render()
{
  //update pbo
  ptr_to_pbo = reinterpret_cast<Pbo::Color*>(glMapNamedBuffer(pboid, GL_WRITE_ONLY));
  clear_color_buffer();
  Pbo::drawModel(*model);
  glUnmapNamedBuffer(pboid);

  //update texture
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboid);
  glTextureSubImage2D(
    texid,
    0,                // mip level
    0,                // x offset
    0,                // y offset
    width,
    height,
    GL_RGBA,          // format 
    GL_UNSIGNED_BYTE, // pixel format
    nullptr);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Pbo::draw_fullwindow_quad()
{
  int verts = 6;
  //shader stuff
  shdr_pgm.Use();
  glBindTextureUnit(verts, texid);
  shdr_pgm.SetUniform("uTex0", verts);

  //load VAO stuff
  glBindVertexArray(vaoid);
  
  glDrawElements(GL_TRIANGLES, verts, GL_UNSIGNED_SHORT, 0);

  glBindTextureUnit(verts, texid);
  glBindVertexArray(0);
  shdr_pgm.UnUse();

  GLenum error = glGetError();
}

void Pbo::cleanup()
{
  delete model;
  glDeleteBuffers(1, &pboid);
  glDeleteTextures(1, &texid);
  shdr_pgm.DeleteShaderProgram();
}

void Pbo::render_linedda(int x1, int y1, int x2, int y2, Pbo::Color color)
{
  //determine delta x,y
  int dx = x2 - x1;
  int dy = y2 - y1;
  float xStep;
  float yStep;
  float slope = float(dy) / dx;
  float inverse = float(dx) / dy;

  //vertical lines
  if (dx == 0) //vertical line
  {
    yStep = dy > 0 ? 1.0f : -1.0f;
    dy = dy > 0 ? dy : -dy;
    while (dy--)
    {
      set_pixel(x1, y1, color);
      y1 += int(yStep);
    }
  }

  // octant 0 & 4 lines
  else if (std::abs(slope) >= 0 && std::abs(slope) <= 1)
  {
    slope = (slope > 0.0f) ? slope : -slope;

    // determine how much we are stepping in x and y
    xStep = (dx > 0) ? 1.0f : -1.0f;
    dx = (dx > 0) ? dx : -dx;
    yStep = (dy > 0) ? slope : -slope;
    set_pixel(x1, y1, color);

    // walk the line
    float yEstimate = float(y1);
    while (--dx)
    {
      x1 += int(xStep);
      yEstimate += yStep;
      set_pixel(x1, int(yEstimate), color);
    }
  }

  // octant 1 and 5 lines
  else
  {
    // invert slope
    inverse = (inverse > 0.0f) ? inverse : -inverse;
    yStep = (dy > 0) ? 1.0f : - 1.0f;
    dy = (dy > 0) ? dy : -dy;
    xStep = (dx > 0) ? inverse : -inverse;

    set_pixel(x1, y1, color);

    float xEstimate = float(x1);
    while (--dy)
    {
      xEstimate += xStep;
      y1 += int(yStep);
      set_pixel(int(xEstimate), y1, color);
    }
  }
}

void Pbo::render_linebresenham(int x1, int y1, int x2, int y2, Pbo::Color color)
{
  // why are you giving a line of no length?
  if (x1 == x2 && y1 == y2)
  {
    set_pixel(x1, y1, color);
    return;
  }

  int dx = x2 - x1;
  int dy = y2 - y1;

  //vertical
  if(dx == 0)
  {
    int ystep = 1;
    if (y1 > y2)
      ystep = -1;
    while (ystep > 0 && y1 < y2)
    {
      set_pixel(x1, y1, color);
      y1 += ystep;
    }
    return;
  }

  //m > 1
  if (abs(dy) > abs(dx))
  {
    int xstep = (dx < 0) ? -1 : 1;
    int ystep = (dy < 0) ? -1 : 1;
    dx = (dx < 0) ? -dx : dx;
    dy = (dy < 0) ? -dy : dy;

    int d = 2 * dx - dy;

    int dmin = 2 * dx;
    int dmaj = dmin - (2 * dy);
    set_pixel(x1, y1, color);

    while (--dy > 0)
    {
      x1 += (d > 0) ? xstep : 0;
      d += (d > 0) ? dmaj : dmin;
      y1 += ystep;
      set_pixel(x1, y1, color);
    }
  }
  else
  {
    int xstep = (dx < 0) ? -1 : 1;
    int ystep = (dy < 0) ? -1 : 1;
    dx = (dx < 0) ? -dx : dx;
    dy = (dy < 0) ? -dy : dy;

    int d = 2 * dy - dx;

    int dmin = 2 * dy;
    int dmaj = dmin - (2 * dx);
    set_pixel(x1, y1, color);
    while (--dx > 0)
    {
      y1 += (d > 0) ? ystep : 0;
      d += (d > 0) ? dmaj : dmin;

      x1 += xstep;
      set_pixel(x1, y1, color);
    }
  }


}
