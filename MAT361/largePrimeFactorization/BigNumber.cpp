#include "BigNumber.h"

/******************************************************************************
  * Project Euler Solutions
  * Author : Cody Morgan
  * Brief  : contains utility funstions related to math not computer science
******************************************************************************/

#include <bitset>
#include <iostream>
using std::cout;


//***** Huge class members *****//

Huge::Huge(unsigned long long init)
{
  setValue(init);
}

Huge::Huge(const std::string& number)
{
  setValue(0);
  addString(number);
}

void Huge::addString(std::string input)
{
  std::list<unsigned char>::iterator current = baseArray_.begin();
  byte carry = 0;

  for (long i = long(input.size() - 1); i >= 0; i--)
  {
    byte c = input[i];

    // size verification
    if (current == baseArray_.end())
    {
      baseArray_.push_back(byte('\0'));
      current--;
    }

    // add this digit
    (*current) += c - '0' + carry;
    //cout << std::bitset<8>(*current) << '\n';

    // carry bit
    carry = (*current) / 10;
    (*current) %= 10;
    //cout << std::bitset<8>(*current) << '\n';

    current++;
  }

  // there could be one more carry
  while (carry)
  {
    if (current == baseArray_.end())
    {
      baseArray_.push_back(byte('\0'));
      current--;
    }

    // carry bit
    (*current) += carry;
    carry = (*current) / 10;
    (*current) %= 10;
    //cout << std::bitset<8>(*current) << '\n';

    current++;
  }
}

std::list<Huge::byte>::const_iterator Huge::begin() const
{
  return baseArray_.begin();
}

std::list<Huge::byte>::const_iterator Huge::end() const
{
  return baseArray_.end();
}

void Huge::setValue(unsigned long long value)
{
  baseArray_.clear();
  std::list<unsigned char>::iterator current = baseArray_.begin();
  byte carry = 0;

  while (value)
  {
    byte c = value % 10;
    value /= 10;

    // size verification
    if (current == baseArray_.end())
    {
      baseArray_.push_back(byte('\0'));
      current--;
    }

    // add this digit
    (*current) = c;
    (*current) += carry;
    //cout << std::bitset<8>(*current) << '\n';

    // carry bit
    carry = (*current) / 10;
    (*current) %= 10;
    //cout << std::bitset<8>(*current) << '\n';

    current++;
  }

  // there could be one more carry
  while (carry)
  {
    if (current == baseArray_.end())
    {
      baseArray_.push_back(byte('\0'));
      current--;
    }

    // carry bit
    (*current) += carry;
    carry = (*current) / 10;
    (*current) %= 10;
    //cout << std::bitset<8>(*current) << '\n';

    current++;
  }
}

unsigned long Huge::size() const
{
  return unsigned long(baseArray_.size());
}

bool Huge::isDivisible(unsigned div)
{
  auto number = getNumber();
  if ( number == -1)
  {
    return number / div;
  }
  else
  {
    
  }
}

unsigned long long Huge::getNumber()
{
  unsigned long long number = 0;
  if (size() <= 19) // unsigned long long max is 1E19
  {
    unsigned digit = 1;
    for (auto byte : baseArray_)
    {
      number += unsigned(byte) * digit;
      digit *= 10;
    }

    return number;
  }

  // error
  return 0;
}

void Huge::subtractFromFront(unsigned value)
{
  if (value == 0)
    return;

  // find digit count of value
  auto valueDigits = 1;
  auto modValue = 10;
  while (value % modValue != value)
  {
    modValue *= 10;
    valueDigits++;
  }

  // set current forward
  auto current = baseArray_.begin();
  for (auto i = 0; i < size() - valueDigits; i++)
  {
    current++;
  }

  while (current != baseArray_.end())
  {
    auto currentValueDigit = value % 10;
    auto currentHugeDigit = unsigned(*current);

    // if borrowing is needed
    if (currentHugeDigit < currentValueDigit)
    {
      int borrowDigits = 0;
      auto borrowFrom = current;
      borrowFrom++;

      // find first digit we can borrow from
      while ((*borrowFrom) == 0)
      {
        borrowFrom++;
        borrowDigits++;
      }

      // do the borrowing
      while (borrowDigits-- >= 0)
      {
        (*borrowFrom)--;
        borrowFrom--;
        (*borrowFrom) += 10;
      }
    }

    // do subtraction
    *current -= currentValueDigit;
    current++;
    value -= currentValueDigit;
    value /= 10;
  }

  auto deleteIfEmpty = baseArray_.rbegin();
  while (*deleteIfEmpty == 0)
  {
    baseArray_.pop_back();
  }
}

