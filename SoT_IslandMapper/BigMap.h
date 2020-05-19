#ifndef _BIGMAP_
#define _BIGMAP_
#pragma once

#include <string>
#include <vector>

#include "IslandBox.h"
#include "TreasureMap.h"

class BigMap
{
public:
  BigMap(std::string file);

  auto isMatch(const TreasureMap& treasure)->std::pair<char, int>;
  void findIslands();
  void drawGrid();

private:
  int boxSize = 240;
  std::string file_;
  std::string debugFile_;
  std::string diff_;
  CImg<unsigned char> image_;
  std::vector<IslandBox> islandBoxes;

  bool isBlack(int x, int y);
  bool hasBlack(int startX, int startY, int count, bool alongX);
  int boxIsland(int xPos, int yPos);
  std::pair<char, int> convertCoords(int x, int y);
};

#endif

