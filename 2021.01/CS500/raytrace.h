///////////////////////////////////////////////////////////////////////
// A framework for a raytracer.
////////////////////////////////////////////////////////////////////////
#include <vector>
using std::vector;
#include "glm/glm/vec3.hpp"
using glm::vec3;
#include "Camera.h"
#include "Material.h"
#include "Shape.h"

const float PI      = glm::pi<const float>();
const float Radians = PI / 180.0f; // Convert degrees to radians

////////////////////////////////////////////////////////////////////////
// Material: encapsulates a BRDF and communication with a shader.
////////////////////////////////////////////////////////////////////////
class Material
{
public:
  Vector3f     Kd, Ks;
  float        alpha;
  unsigned int texid;

  virtual bool isLight()
  {
    return false;
  }

  Material() : Kd(Vector3f(1.0, 0.5, 0.0)), Ks(Vector3f(1, 1, 1)), alpha(1.0), texid(0)
  {
  }
  Material(const Vector3f d, const Vector3f s, const float a) : Kd(d), Ks(s), alpha(a), texid(0)
  {
  }
  Material(Material& o)
  {
    Kd    = o.Kd;
    Ks    = o.Ks;
    alpha = o.alpha;
    texid = o.texid;
  }

  void setTexture(const std::string path);
  // virtual void apply(const unsigned int program);
};

////////////////////////////////////////////////////////////////////////
// Data structures for storing meshes -- mostly used for model files
// read in via ASSIMP.
//
// A MeshData holds two lists (stl::vector) one for vertices
// (VertexData: consisting of point, normal, texture, and tangent
// vectors), and one for triangles (TriData: consisting of three
// indices into the vertex array).

////////////////////////////////////////////////////////////////////////
// Light: encapsulates a light and communiction with a shader.
////////////////////////////////////////////////////////////////////////
class Light : public Material
{
public:
  Light(const Vector3f e) : Material()
  {
    Kd = e;
  }
  virtual bool isLight()
  {
    return true;
  }
  // virtual void apply(const unsigned int program);
};

////////////////////////////////////////////////////////////////////////////////
// Scene
class Realtime;

class Scene
{
public:
  Realtime* realtime; // Remove this (realtime stuff)

  enum ImageType
  {
    TestOnly,
    DepthOnly,
    DifuseOnly,
    NormalOnly,
    LitOnly,
    All
  };
  std::vector<std::string> imageTypeNames = {"TestOnly", "DepthOnly", "DifuseOnly", "NormalOnly", "LitOnly", "All"};

  int            width, height;
  Camera         camera;
  glm::vec3      ambientLight;
  MyMaterial     myMaterial;
  vector<Shape*> shapes;
  vector<Shape*> lights;

  static void PrintVec3(std::string name, Vector3f vec, bool nl = true)
  {
    std::cout << name << " = (" << vec.x() << "," << vec.y() << "," << vec.z() << ")" << (nl ? "\n" : "");
  }

  static void PrintQuaternion(std::string name, Quaternionf q, bool nl = true)
  {
    std::cout << name << " = xyzw(" << q.x() << "," << q.y() << "," << q.z() << "," << q.w() << ")" << (nl ? "\n" : "");
  }

  Scene();
  ~Scene();

  void Finit();

  glm::vec3 calculateLighting(const glm::vec3& diffuse,
                              const glm::vec3& norm,
                              const glm::vec3& objPos,
                              const Shape&     shape);

  // The scene reader-parser will call the Command method with the
  // contents of each line in the scene file.
  void Command(const std::vector<std::string>& strings, const std::vector<float>& f);

  // To read a model file into the scene via ASSIMP, call ReadAssimpFile.
  void ReadAssimpFile(const std::string& path, const Matrix4f& M);

  // Once ReadAssimpFile parses the information from the model file,
  // it will call:
  void triangleMesh(MeshData* mesh);

  // The main program will call the TraceImage method to generate
  // and return the image.  This is the Ray Tracer!
  void TraceImage(Color* image, Scene::ImageType imageType, const int pass);

  void TraceTestImage(Color* image, const int pass);

  void TracePath(Color* image, const int pass);

  struct IntersectionRecord
  {
    vec3   point;
    vec3   norm;
    Shape* hit      = nullptr;
    bool   isLight  = false;
    float  shortest = INFINITY;
  };

  const float epsilon                 = 0.000001f;
  const float conservationEnergyValue = 0.8f;

  Ray                CastRay(int x, int y, Vector3f X, Vector3f Y, Vector3f Z);
  bool               russianRoulette(float leq = 0.8);
  glm::vec3          sampleBRDF(const glm::vec3& w0, const IntersectionRecord& ir);
  glm::vec3          sampleLobe(const glm::vec3& N, float c, float phi);
  glm::vec3          evalScattering(const glm::vec3& w0, const IntersectionRecord& ir, const glm::vec3& w);
  float              pdfBrdf(const glm::vec3& w0, const glm::vec3& wi, IntersectionRecord& ir);
  glm::vec3          evalRadiance(const IntersectionRecord& ir);
  IntersectionRecord findIntersection(const Ray& ray);

  // phase 2
  IntersectionRecord SampleLight();
  IntersectionRecord sampleSphere(const glm::vec3& C, float R);
  float              pdfLight(const IntersectionRecord& ir);
  float              geometryFactor(IntersectionRecord A, IntersectionRecord B);
};
