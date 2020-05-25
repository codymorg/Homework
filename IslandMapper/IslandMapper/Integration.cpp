#include "Integration.h"
using std::string;
#include <iostream>
using std::cout;
#include <sstream>
#pragma warning( disable : 4996)



HotKey::HotKey(char key) : key_(key)
{
  if (RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 0x0 + key))  //0x42 is 'b'
  {
    cout << "hot key registered to [ALT + " << key << "]\n";
  }
  else
   printf("failed to registure hotkey\n");
}

bool HotKey::update()
{
  MSG msg = { 0 };
  while (GetMessage(&msg, NULL, 0, 0) != 0)
  {
    if (msg.message == WM_HOTKEY)
    {
      cout << "WM_HOTKEY " << key_ <<" received\n";
      screenShot();
    }
  }
  
  return 0;
}

void HotKey::screenShot()
{
  // get the device context of the screen
  HDC hScreenDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);

  // and a device context to put it in
  HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

  int width = GetDeviceCaps(hScreenDC, HORZRES) * 2;
  int height = GetDeviceCaps(hScreenDC, VERTRES) * 2;

  // maybe worth checking these are positive values
  HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

  // get a new bitmap
  HBITMAP oldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

  BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
  hBitmap = (HBITMAP)SelectObject(hMemoryDC, oldBitmap);
  
  // clean up
  DeleteDC(hMemoryDC);
  DeleteDC(hScreenDC);

  WriteBMP(hBitmap, hScreenDC, string("screenShot.bmp"));
}

void HotKey::WriteBMP(HBITMAP hBitMap, HDC hDC, string filename)
{
  CBitmap bmp;
  bmp.Attach((HBITMAP)hBitMap);

  BITMAP bitmap;
  bmp.GetBitmap(&bitmap);
  int size = bitmap.bmWidth * bitmap.bmHeight * bitmap.bmBitsPixel / 8;
  BYTE* lpBits = new BYTE[size];

  ::GetBitmapBits((HBITMAP)hBitMap, size, lpBits);
 
  BITMAPINFO Bmi = { 0 };
  Bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  Bmi.bmiHeader.biWidth = bitmap.bmWidth;
  Bmi.bmiHeader.biHeight = bitmap.bmHeight;
  Bmi.bmiHeader.biPlanes = 1;
  Bmi.bmiHeader.biBitCount = bitmap.bmBitsPixel;
  Bmi.bmiHeader.biCompression = BI_RGB;
  Bmi.bmiHeader.biSizeImage = bitmap.bmWidth * bitmap.bmHeight * bitmap.bmBitsPixel / 8;


  FILE* image = fopen(filename.c_str(), "wb");
  if (image == 0)
    return;
  int h = abs(Bmi.bmiHeader.biHeight);
  int w = abs(Bmi.bmiHeader.biWidth);
  Bmi.bmiHeader.biHeight = -h;
  Bmi.bmiHeader.biWidth = w;
  int sz = Bmi.bmiHeader.biSizeImage;

  BITMAPFILEHEADER bfh;
  bfh.bfType = ('M' << 8) + 'B';
  bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  bfh.bfSize = sz + bfh.bfOffBits;
  bfh.bfReserved1 = 0;
  bfh.bfReserved2 = 0;

  fwrite(&bfh, sizeof(bfh), 1, image);
  fwrite(&Bmi.bmiHeader, sizeof(BITMAPINFOHEADER), 1, image);
  fwrite(lpBits, sz, 1, image);
  fclose(image);

  delete[]lpBits;
}

HotKey::~HotKey()
{
}
