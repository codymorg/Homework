#ifndef KNAPSACK_BRUTE_FORCE_MINCHANGE
#define KNAPSACK_BRUTE_FORCE_MINCHANGE
#include <utility>   // std::pair
#include <vector>
#include <bitset>
#include "definitions.h"

class GreyCode 
{
  public:
    GreyCode( int s );

    // first subset is empty, this function tells what's next 2^n-1 subsets
    // ret value: is last, is add, positin
    std::pair<bool, std::pair<bool, int>> Next();
    std::vector<bool> getPermutation(int i);

  private:
    void printMaster();
    int getPosModified();
    bool isLast();

    std::vector<std::vector<bool>> master_;
    int currentIndex_ = -1;
};

class Sack
{
public:
  Sack(Weight const& maxWeight);
  void add(Item const& item, int perm);
  void remove(Item const& item, int perm);
  int maxPermutation = -1;

private:
  void validate(int perm);

  Weight maxWeight_;
  int currentValue_ = 0;
  Weight currentWeight_;
  int maxValue_ = 0;
  Weight filledWeight;
};

// brute-force knapsack
std::vector<bool> knapsack_brute_force( std::vector<Item> const& items, Weight const& W );
#endif
