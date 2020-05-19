
#include "BigMap.h"
#include <iostream>

int main()
{
  // load big map
  BigMap map("Maps/BigMap.bmp");
#ifdef _DEBUG
  map.drawGrid();
#endif
  map.findIslands();

  // load treasure map
  TreasureMap treasure("Maps/ashenMap.bmp");
  treasure.preprocess();
  treasure.buildIslandMask();
  treasure.findIsland();

  // compare treasure map to big map
  auto match = map.isMatch(treasure);
  std::cout << "(" << match.first << "," << match.second << ")\n";

  return 0;
}
