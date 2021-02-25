#include "Timer.h"
using std::cout;

void Timer::start()
{
  t0 = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
  t1 = std::chrono::high_resolution_clock::now();
}

void Timer::print(std::string label, bool nl)
{
  auto sec = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0);
  auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
  if (sec.count() > 0)
  {
    auto rem = ms.count() - (sec.count() * 1000);
    cout << label << (label.size() == 0 ? "" : " : ") << sec.count() << "." << rem << (nl ? "sec\n" : "sec");
  }
  else
  {
    cout << label << (label.size() == 0 ? "" : " : ") << ms.count() << (nl ? "ms\n" : "ms");
  }
}

void Timer::tick(std::string label, bool nl, bool startAgain)
{
  stop();
  print(label, nl);
  if (startAgain)
    start();
}
