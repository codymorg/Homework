#pragma once
#ifndef _INTIGRATION_
#define _INTEGRATION_
#include <afxwin.h>
#include <string>

class HotKey
{
public:
  HotKey(char key);
  ~HotKey();

  bool update();
  void screenShot();
  void WriteBMP(HBITMAP hBitMap, HDC hDC, std::string filename);


private:
  char key_;
};

#endif
