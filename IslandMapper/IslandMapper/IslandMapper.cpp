#include "Integration.h"
#include "BigMap.h"
#include <iostream>

int main()
{
  // hook up hot key

  // load big map
  BigMap map("Maps/BigMap2.bmp");
#ifdef _DEBUG
  //map.drawGrid();
#endif
  map.findIslands();
  HotKey hot('F');
  hot.update();


  // load treasure map
  TreasureMap treasure("Maps/test2.bmp");
  treasure.preprocess();
  treasure.buildIslandMask();
  treasure.findIsland();

  // compare treasure map to big map
  auto match = map.isMatch(treasure);
  std::cout << "(" << match.first << "," << match.second << ")\n";

  return 0;
}
