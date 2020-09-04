#ifndef _SMALLMAP_
#define _SMALLMAP_
#pragma once
#include <string>
#include "CImg-2.9.1_pre042420/CImg.h"


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
  bool isBlack(int x, int y);
  int hasBlack(int startX, int startY, int count, bool alongX, bool reverse);


  void preprocess();      // crop get rid of background
  void buildIslandMask(); // b/w image
  void postprocess();     // clean up image
  void findIsland(); 
  const std::string& getFile() const { return file_; };
  cimg_library::CImg<unsigned char> getMask() const { return mask_; };

private:
  bool isWithinRange(int x, int y, unsigned int* value, unsigned char range);
  
  std::string file_;
  std::string debugFile_;
  int maskLimit = 128;
  int whiteGapLimit = 100;
  cimg_library::CImg<unsigned char> mask_;

};

#endif

