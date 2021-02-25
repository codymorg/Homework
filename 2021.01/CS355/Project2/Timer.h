#pragma once
#include <chrono>
#include <iostream>
#include <string>

class Timer
{
public:
  typedef std::chrono::steady_clock::time_point Time;

  void start();
  void stop();
  void print(std::string label = "", bool nl = true);
  void tick(std::string label = "", bool nl = true, bool startAgain = true);

private:
  Time t0;
  Time t1;
};
