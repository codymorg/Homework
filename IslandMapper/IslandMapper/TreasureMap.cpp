#include "TreasureMap.h"
#include "IslandBox.h"
using namespace cimg_library;

TreasureMap::TreasureMap(std::string file) : file_(file)
{
  mask_ = CImg<>(file_.c_str()).normalize(0, 255).resize(-100, -100, 1, 3);
  for (int i = file_.size(); i >= 0; i--)
  {
    if (file_[i] == '.')
      debugFile_ = file_.substr(0, i) + "_out.bmp";
    if (file_[i] == '/' || file_[i] == '\\')
      debugFile_ = file_.substr(0, i) + "/out" + debugFile_.substr(i);
  }
}

void TreasureMap::preprocess()
{
  unsigned char maxPix[] = { 0,0,0 };
  unsigned char add = '/0';
  for (int y = 0; y < mask_.height(); y++)
  {
    for (int x = 0; x < mask_.width(); x++)
    {
      unsigned char thisPix[] = { 0,0,0 };
      for (int i = 0; i < 3; i++)
      {
        thisPix[i] = mask_(x, y, 0, i);
      }
      if ((thisPix[0] + thisPix[1] + thisPix[2]) / 3 > add)
      {
        add = (thisPix[0] + thisPix[1] + thisPix[2]) / 3;
      }
    }
  }

  for (int y = 0; y < mask_.height(); y++)
  {
    for (int x = 0; x < mask_.width(); x++)
    {
      for (int i = 0; i < 3; i++)
      {
        mask_(x, y, 0, i) += add;
      }
    }
  }

  mask_.save(debugFile_.c_str(), 0, 3);
  mask_.save(debugFile_.c_str());
}

void TreasureMap::buildIslandMask()
{
  IslandBox bounds(mask_.width() / 4, mask_.height() / 8, mask_.width() / 2, (3 * mask_.height()) / 4);
  mask_.crop(bounds.x, bounds.y, 0, 0, bounds.x + bounds.w, bounds.y + bounds.h, 0, 2);
  mask_.save(debugFile_.c_str(), 0, 3);


  int whiteGap = 0;
  bool inIsland = false;
  for (int y = 0; y < mask_.height(); y++)
  {
    for (int x = 0; x < mask_.width(); x++)
    {
      if (mask_(x, y, 0, 0) >= maskLimit && mask_(x, y, 0, 1) >= maskLimit && mask_(x, y, 0, 2) >= maskLimit)
      {
        inIsland = true;
        whiteGap = 0;
        mask_.draw_point(x, y, black, 1);
      }
      else
        mask_.draw_point(x, y, white, 1);
    }
    if(inIsland)
      whiteGap++;
    if ( whiteGap >= whiteGapLimit)
      break;
  }
  mask_.save(debugFile_.c_str());
}

void TreasureMap::findIsland()
{
  mask_ = CImg<>(debugFile_.c_str()).normalize(0, 255).resize(-100, -100, 1, 3);
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

  IslandBox bounds(TL[0], TL[1], BR[0] - TL[0], BR[1] - TL[1]);
  mask_.crop(bounds.x, bounds.y, 0, 0, bounds.x + bounds.w, bounds.y + bounds.h, 0, 2);

#ifdef _DEBUG
  mask_.save(debugFile_.c_str(), 2, 3);
#endif
}
