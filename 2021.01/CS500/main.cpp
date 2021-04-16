///////////////////////////////////////////////////////////////////////
// Provides the framework a raytracer.
//
// Gary Herron
//
// Copyright 2012 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#ifdef _WIN32
// Includes for Windows
#include <crtdbg.h>
#include <cstdlib>
#include <limits>
#include <windows.h>
#else
// Includes for Linux
#include <stdlib.h>
#include <time.h>
#endif

#include "geom.h"
#include "raytrace.h"

// Read a scene file by parsing each line as a command and calling
// scene->Command(...) with the results.
void ReadScene(const std::string inName, Scene* scene)
{
  std::ifstream input(inName.c_str());
  if (input.fail())
  {
    std::cerr << "File not found: " << inName << std::endl;
    fflush(stderr);
    exit(-1);
  }

  // For each line in file
  for (std::string line; getline(input, line);)
  {
    std::vector<std::string> strings;
    std::vector<float>       floats;

    // Parse as parallel lists of strings and floats
    std::stringstream lineStream(line);
    for (std::string s; lineStream >> s;)
    { // Parses space-separated strings until EOL
      float f;
      // std::stringstream(s) >> f; // Parses an initial float into f, or zero if illegal
      if (!(std::stringstream(s) >> f))
        f = nan(""); // An alternate that produced NANs
      floats.push_back(f);
      strings.push_back(s);
    }

    if (strings.size() == 0)
      continue; // Skip blanks lines
    if (strings[0][0] == '#')
      continue; // Skip comment lines

    // Pass the line's data to Command(...)
    scene->Command(strings, floats);
  }

  input.close();
}

// Write the image as a HDR(RGBE) image.
#include "rgbe.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#define PRINTCOLORS 1

bool HasNan(const Color& color)
{
  return (isnan(color[0]) || isnan(color[1]) || isnan(color[2]));
}
 Color SampleSurrounding(int x, int y, int width, int height, Color* image)
{
  int R = x + 1 >= width ? -1 : x + 1;
  int U = y + 1 >= height ? -1 : y + 1;
  int L = x - 1;
  int D = y - 1;

  Color newColor;
  int   i = 0;
  if (U >= 0 && R >= 0 && !HasNan(image[U * width + R]))
  {
    newColor += image[U * width + R];
    i++;
  }
  if (D >= 0 && R >= 0 && !HasNan(image[D * width + R]))
  {
    newColor += image[D * width + R];
    i++;
  }
  if (U >= 0 && L >= 0 && !HasNan(image[U * width + L]))
  {
    newColor += image[U * width + L];
    i++;
  }
  if (D >= 0 && L >= 0 && !HasNan(image[D * width + L]))
  {
    newColor += image[D * width + L];
    i++;
  }

  if (U >= 0 && !HasNan(image[U * width + x]))
  {
    newColor += image[U * width + x];
    i++;
  }
  if (D >= 0 && !HasNan(image[D * width + x]))
  {
    newColor += image[D * width + x];
    i++;
  }
  if (R >= 0 && !HasNan(image[y * width + R]))
  {
    newColor += image[y * width + R];
    i++;
  }
  if (L >= 0 && !HasNan(image[y * width + L]))
  {
    newColor += image[y * width + L];
    i++;
  }

  Color pixel = {
    newColor[0] / i,
    newColor[1] / i,
    newColor[2] / i,
  };
  image[y * width + x] = pixel;
  return pixel;
}

void WriteHdrImage(const std::string outName, const int width, const int height, Color* image, int passes)
{
#if PRINTCOLORS
  std::ofstream myfile;
  myfile.open("colors.txt", std::ios::trunc);
#endif
  // Turn image from a 2D-bottom-up array of Vector3D to an top-down-array of floats
  float* data = new float[width * height * 3];
  float* dp   = data;
  for (int y = height - 1; y >= 0; --y)
  {
    for (int x = 0; x < width; ++x)
    {
      Color pixel = image[y * width + x];
      if (HasNan(pixel))
      {
        pixel = SampleSurrounding(x, y, width, height, image);
        printf("nan detected, new color, [%f, %f, %f]\n", pixel[0] / passes, pixel[1] / passes, pixel[2] / passes);
      }
#if PRINTCOLORS
      int w = 8;
      myfile << "[" << std::fixed << std::setw(w) << std::setprecision(0) << pixel[0] << " " << std::fixed
             << std::setw(w) << pixel[1] << " " << std::fixed << std::setw(w) << pixel[2] << "]";
#endif
      *dp++ = pixel[0] / passes;
      *dp++ = pixel[1] / passes;
      *dp++ = pixel[2] / passes;
    }
#if PRINTCOLORS
    myfile << "\n";
#endif
  }

#if PRINTCOLORS
  myfile.close();
#endif

  // Write image to file in HDR (a.k.a RADIANCE) format
  rgbe_header_info info;
  char             errbuf[100] = {0};

  FILE* fp   = fopen(outName.c_str(), "wb");
  info.valid = false;
  int r      = RGBE_WriteHeader(fp, width, height, &info, errbuf);
  if (r != RGBE_RETURN_SUCCESS)
    printf("error: %s\n", errbuf);

  r = RGBE_WritePixels_RLE(fp, data, width, height, errbuf);
  if (r != RGBE_RETURN_SUCCESS)
    printf("error: %s\n", errbuf);
  fclose(fp);

  delete data;
}

static void printBasicImages(Scene* scene)
{
  for (int i = 0; i < Scene::All; i++)
  {
    Color* image = new Color[scene->width * scene->height];
    for (int y = 0; y < scene->height; y++)
      for (int x = 0; x < scene->width; x++)
        image[y * scene->width + x] = Color(0, 0, 0);

    std::cout << scene->imageTypeNames[i] << "===================\n";
    scene->TraceImage(image, (Scene::ImageType)i, 1);
    WriteHdrImage(scene->imageTypeNames[i] + ".hdr", scene->width, scene->height, image, 1);
    delete[] image;
  }
}

////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  Scene* scene = new Scene();

  // Read the command line argument
  std::string inName  = (argc > 1) ? argv[1] : "testscene.scn";
  std::string hdrName = inName;

  hdrName.replace(hdrName.size() - 3, hdrName.size(), "hdr");

  // Read the scene, calling scene.Command for each line.
  ReadScene(inName, scene);

  std::cout << "\nscene created with objects:\n";
  for (auto var : scene->shapes)
  {
    std::cout << "\t" << var->name() << "\n";
  }

  // Allocate and clear an image array
  Color* image = new Color[scene->width * scene->height];
  for (int y = 0; y < scene->height; y++)
    for (int x = 0; x < scene->width; x++)
      image[y * scene->width + x] = Color(0, 0, 0);

  // RayTrace the image
  Scene::ImageType makeImage = Scene::ImageType::LitOnly;
  int              pass      = 100;
  int              passes    = 0;
  int              stop      = 1000;
  printBasicImages(scene);
  while (passes < stop)
  {
    passes += pass;
    scene->TracePath(image, pass);

    // Write the image
    WriteHdrImage(hdrName, scene->width, scene->height, image, passes);
    std::cout << "\n" << passes << " / " << stop << "\n";
  }
  delete scene;
}
