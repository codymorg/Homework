#include "TreasureMap.h"
#include "IslandBox.h"
#include <iostream>
using namespace cimg_library;

TreasureMap::TreasureMap(std::string file) : file_(file)
{
  mask_ = CImg<unsigned char>(file_.c_str()).normalize(0, 255).resize(-100, -100, 1, 3);
  for (int i = file_.size(); i >= 0; i--)
  {
    if (file_[i] == '.')
      debugFile_ = file_.substr(0, i) + "_out.bmp";
    if (file_[i] == '/' || file_[i] == '\\')
      debugFile_ = file_.substr(0, i) + "/out" + debugFile_.substr(i);
  }
}

// find the brightest pixel 
void TreasureMap::preprocess()
{
  IslandBox bounds(mask_.width() / 4, mask_.height() / 8, mask_.width() / 2, (3 * mask_.height()) / 4);
  mask_.crop(bounds.x, bounds.y, 0, 0, bounds.x + bounds.w, bounds.y + bounds.h, 0, 2);

  unsigned char maxPix[] = { 0,0,0 };
  unsigned int avgPix[] = { 0,0,0 };

  // find brightest pixle
  for (int y = 0; y < mask_.height(); y++)
  {
    for (int x = 0; x < mask_.width(); x++)
    {
      unsigned char thisPix[] = { 0,0,0 };
      for (int i = 0; i < 3; i++)
      {
        thisPix[i] = mask_(x, y, 0, i);
        avgPix[i] += mask_(x, y, 0, i);
      }

      if (thisPix[0] > maxPix[0] || thisPix[1] > maxPix[1] || thisPix[2] > maxPix[2])
      {
        memcpy(maxPix, thisPix, sizeof(unsigned char) * 3);
      }
    }
  }

  for (int i = 0; i < 3; i++)
  {
    avgPix[i] /= mask_.width() * mask_.height();
  }

  // add to every pixle to give a 255 upper limit
  for (int y = 0; y < mask_.height(); y++)
  {
    for (int x = 0; x < mask_.width(); x++)
    {
      for (int i = 0; i < 3; i++)
      {
        //mask_(x, y, 0, i) += 255 - maxPix[i];
      }

      // color average pixles red
      if (isWithinRange(x, y, avgPix, 32))
      {
        mask_.draw_point(x, y, red, 1);
      }
    }
  }
  std::cout << "eliminating pix " << avgPix[0] << ", " << avgPix[1] << ", " << avgPix[2] << "\n";
  std::cout << "preprocess mask crops and brightens color value by: [" << (int)maxPix[0] << ", " << (int)maxPix[1] << ", " << (int)maxPix[2] << "]\n";
  std::cout << "preprocess mask saved as 000\n";
  mask_.save(debugFile_.c_str(), 0, 3);
  mask_.save(debugFile_.c_str());
}

void TreasureMap::buildIslandMask()
{
  int whiteGap = 0;
  bool inIsland = false;
  int cropTop = 0;
  int cropBot = 0;
  bool readyBot = false;

  for (int y = 0; y < mask_.height(); y++)
  {
    for (int x = 0; x < mask_.width(); x++)
    {
      if (mask_(x, y, 0, 0) >= maskLimit && mask_(x, y, 0, 1) >= maskLimit && mask_(x, y, 0, 2) >= maskLimit)
      {
        if (cropTop != 0)
          readyBot = true;
        inIsland = true;
        whiteGap = 0;
        mask_.draw_point(x, y, black, 1);
      }
      else
        mask_.draw_point(x, y, white, 1);
    }
    if(inIsland)
      whiteGap++;
    if (whiteGap >= whiteGapLimit)
    {
      mask_.draw_circle(0, y, 2, purple, 1);
      if (readyBot)
        cropBot = y;
      else
        cropTop = y;
    }
  }
  std::cout << "b/w mask saved as 001\n";
  mask_.save(debugFile_.c_str(), 1, 3);
}

void TreasureMap::postprocess()
{
  unsigned int center[2] = { 0,0 };
  unsigned int blacks = 0;

  // find center of island
  for (size_t y = 0; y < mask_.height(); y++)
  {
    for (int x = 0; x < mask_.width(); x++)
    {
      if (mask_(x, y, 0, 0) == 255)
      {
        center[0] += x;
        center[1] += y;
        blacks++;
      }
    }
  }

  center[0] /= blacks;
  center[1] /= blacks;
  mask_.draw_circle(center[0], center[1], 2, purple, 1);

  // search outward
  int minY = 0;
  int maxY = 0;
  int minX = INT_MAX;
  int maxX = 0;
  int whiteCount = 0;
  for (size_t y = center[1]; y < mask_.height(); y++)
  {
    if (y % 100 == 0) std::cout << ".";
    for (int x = 0; x < mask_.width(); x++)
    {
      int thisX = hasBlack(0, y, mask_.width(), true, false);
      int thatX = hasBlack(mask_.width(), y, mask_.width(), true, true);
      if (thatX >= 0)
        maxX = std::max(thatX, maxX);
      if (thisX >= 0)
      {
        minX = std::min(thisX, minX);
        maxY = y;
        whiteCount = 0;
      }
      else if (++whiteCount > whiteGapLimit)
        goto A;
    }
  }
  std::cout << "\n";
A:;
  for (size_t y = center[1]; y >= 0; y--)
  {
    if (y % 100 == 0) std::cout << ".";
    for (int x = 0; x < mask_.width(); x++)
    {
      int thisX = hasBlack(0, y, mask_.width(), true, false);
      int thatX = hasBlack(mask_.width(), y, mask_.width(), true, true);
      if (thatX >= 0)
        maxX = std::max(thatX, maxX);
      if (thisX >= 0)
      {
        minX = std::min(thisX, minX);
        minY = y;
        whiteCount = 0;
      }
      else if (++whiteCount > whiteGapLimit)
        goto B;
    }
  }
B:;
  // recrop
  mask_.crop(minX, minY, 0, maxX, maxY, 0);

  // fill in white space on island

  std::cout << "postprocess finds center " << center[0] << ", " << center[1] << "\n";
  std::cout << "crops allowing for " << whiteGapLimit << "pix border. saved as 003\n";
  mask_.save(debugFile_.c_str(), 3, 3);
}

void TreasureMap::findIsland()
{
  //mask_ = CImg<>(debugFile_.c_str()).normalize(0, 255).resize(-100, -100, 1, 3);
  int TL[2] = {INT_MAX,-1};
  int BR[2] = {-1,-1};

  for (int y = 0; y < mask_.height(); y++)
  {
    for (int x = 0; x < mask_.width(); x++)
    {
      if (mask_(x, y, 0, 0) == 0)
      {
        TL[0] = std::min(TL[0], x);
        if (TL[1] == -1)
          TL[1] = y;
        BR[0] = std::max(BR[0], x);
        BR[1] = std::max(BR[1], y);
      }
    }
  }
  std::cout << "cropped island saved as 002\n";
  IslandBox bounds(TL[0], TL[1], BR[0] - TL[0], BR[1] - TL[1]);
  mask_.crop(bounds.x, bounds.y, 0, 0, bounds.x + bounds.w, bounds.y + bounds.h, 0, 2);

  mask_.save(debugFile_.c_str(), 2, 3);
}

bool TreasureMap::isWithinRange(int x, int y, unsigned int* value, unsigned char range)
{
   int min[3] = {
    std::max(int(0),int(value[0] - range)),
    std::max(int(0),int(value[1] - range)),
    std::max(int(0),int(value[2] - range))
  };
   int max[3] = {
    std::min( int(255),  int(value[0] + range)),
    std::min( int(255),  int(value[1] + range)),
    std::min( int(255),  int(value[2] + range))
  };
   bool isIn = (mask_(x, y, 0, 0) < max[0] &&
     mask_(x, y, 0, 1) < max[1] &&
     mask_(x, y, 0, 2) < max[2]) &&
     (mask_(x, y, 0, 0) > min[0] &&
       mask_(x, y, 0, 1) > min[1] &&
       mask_(x, y, 0, 2) > min[2]);
   return isIn;
}

bool TreasureMap::isBlack(int x, int y)
{
  if (y < 0)
    return false;
  unsigned char color[] =
  {
    mask_(x, y, 0, 0),
    mask_(x, y, 0, 1),
    mask_(x, y, 0, 2)
  };
  return color[0] < 16 && color[1] < 16 && color[2] < 16;
}

int TreasureMap::hasBlack(int startX, int startY, int count, bool alongX, bool reverse)
{
  if (alongX)
  {
    if (reverse)
    {
      for (int x = startX; x >= 0 && startX - x < count; x--)
      {
        if (isBlack(x, startY))
          return x;
      }
    }
    else
    {
      for (int x = startX; x < (startX + count) && x < mask_.width(); x++)
      {
        if (isBlack(x, startY))
          return x;
      }
    }
  }
  else
  {
    for (int y = startY; y < (startY + count) && y < mask_.height(); y++)
    {
      if (isBlack(startX, y))
        return y;
    }
  }

  return -1;
}
