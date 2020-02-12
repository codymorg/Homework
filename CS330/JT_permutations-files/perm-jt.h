#ifndef PERM_JOHNSON_TROTTER_H
#define PERM_JOHNSON_TROTTER_H
#include <vector>

class PermJohnsonTrotter 
{
public:
	PermJohnsonTrotter(int size);
	bool Next();
	std::vector<int> const& Get() const;

private:

  bool isStuck(int largestIndex);
  void swap(int index);
  bool isNext();
  bool vectorContains(std::vector<int> const& vector, int value);

  std::vector<int> current_;
  std::vector<int> direction_;
};
#endif
