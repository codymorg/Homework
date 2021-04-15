#include "common.h"
#include "Camera.h"
#include "MyMaterial.h"
#include "Shape.h"

const float PI = 3.14159f;

class Scene
{
public:
  int            width, height;
  MyMaterial    currentMat;
  Vec3           ambientLight;
  vector<Shape*> shapes;
  vector<Shape*> lights;
  Camera         camera;


  Scene();

  // The scene reader-parser will call the Command method with the
  // contents of each line in the scene file.
  void Command(const std::vector<std::string>& strings, const std::vector<float>& f);

  // To read a model file into the scene via ASSIMP, call ReadAssimpFile.
  void ReadAssimpFile(const std::string& path, const Matrix4f& M);

  // The main program will call the TraceImage method to generate
  // and return the image.  This is the Ray Tracer!
  void TraceImage(Color* image, const int pass);
};
