#include "phone2word.h"
#include <iostream>

using std::string;
using std::vector;
using std::map;

void Print(map<char, std::string>& keypad, string const& phoneNumber, vector<unsigned>& childStack, vector<string>& found)
{
  string parents;

  //don't put the last digit in parents
  for (size_t i = 0; i < childStack.size() - 1; i++)
  {
    char digit = phoneNumber[i];
    char letter = keypad[digit][childStack[i]];
    parents.push_back(letter);
  }

  // print all the children from this parent
  for (size_t i = 0; i < keypad[phoneNumber[phoneNumber.size()-1]].size(); i++)
  {
    // print out the match 
    found.push_back(parents + keypad[phoneNumber[phoneNumber.size() - 1]][i]);
  }
}

void Right(map<char, std::string>& keypad, string const& phoneNumber, unsigned pnIndex, vector<unsigned>& childStack, vector<string>& found)
{
  unsigned childIndex = 0;
  unsigned keyLength = unsigned(keypad[phoneNumber[pnIndex]].size());

  while (childIndex < keyLength)
  {
    childStack.push_back(childIndex);
    // top check?

    // end of the phone number
    if (pnIndex == phoneNumber.size() - 1)
    {
      Print(keypad, phoneNumber, childStack, found);

      childStack.pop_back();
      childStack.pop_back();

      return;
    }

    // move to the farthest right
    else
    {
      Right(keypad, phoneNumber, pnIndex + 1, childStack, found);
      childIndex++;
    }
  }

  if(!childStack.empty())
    childStack.pop_back();
}

std::vector< std::string > all_combinations(
  std::string const& phone_number,      // number to convert
  std::map<char, std::string> & keypad   // mapping on the keypad - I may use a non-standard mapping
)
{
  vector<unsigned> childStack;
  vector<string> found;
  Right(keypad, phone_number, 0, childStack, found);

  return found;
}