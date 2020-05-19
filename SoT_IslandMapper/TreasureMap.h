#ifndef _SMALLMAP_
#define _SMALLMAP_
#pragma once
#include <string>
#include "CImg-2.9.1_pre042420/CImg.h"
using namespace cimg_library;

const unsigned char red[] = { 255,0,0 };
const unsigned char green[] = { 0,255,0 };
const unsigned char blue[] = { 0,0,255 };
const unsigned char purple[] = { 255,0,255 };
const unsigned char black[] = { 0,0,0 };
const unsigned char white[] = { 255,255,255 };

class TreasureMap
{
public:
  TreasureMap(std::string file);
  
  void preprocess();
  void buildIslandMask();
  void findIsland();
  const std::string& getFile() const { return file_; };
  CImg<unsigned char> getMask() const { return mask_; };

private:
  std::string file_;
  std::string debugFile_;
  int maskLimit = 128;
  int whiteGapLimit = 100;
  CImg<unsigned char> mask_;
};

#endif

