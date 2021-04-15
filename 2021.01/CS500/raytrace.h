#include "Camera.h"
#include "MyMaterial.h"
#include "Shape.h"
#include "common.h"

class Scene
{
public:
  int            width, height;
  MyMaterial     currentMat;
  Vec3           ambientLight;
  vector<Shape*> shapes;
  vector<Shape*> lights;
  Camera         camera;
  Vector3f       X;
  Vector3f       Y;
  Vector3f       Z;
  float          conservationEnergyValue = 0.8f;

  Scene();

  // The scene reader-parser will call the Command method with the
  // contents of each line in the scene file.
  void Command(const std::vector<std::string>& strings, const std::vector<float>& f);

  // The main program will call the TraceImage method to generate
  // and return the image.  This is the Ray Tracer!
  void traceImage(Color* image);

  void tracePath(Color* image, const int pass);

  Ray          castRay(int x, int y);
  Intersection findIntersection(const Ray& ray);
  void         SetAxes();

  bool         russianRoulette(float leq = 0.8f);
  Vec3         sampleBRDF(const Vec3& N);
  Vec3         sampleLobe(const Vec3 N, float c, float phi);
  Vec3         evalScattering(const Intersection& ir, const Vec3& w);
  float        pdfBrdf(const Vec3& N, const Vec3& w);
  Vec3         evalRadiance(const Intersection& ir);
  Vec3         calculateLighting(const Intersection& inter);
  Intersection sampleLight();
  Intersection sampleSphere(const Vec3& C, float R);
  float        pdfLight(const Intersection& ir);
  float        geometryFactor(Intersection A, Intersection B);
};
