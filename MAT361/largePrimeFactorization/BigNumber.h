/******************************************************************************
  * Project Euler Solutions
  * Author : Cody Morgan
  * Brief  : contains utility funstions related to math not computer science
******************************************************************************/

#pragma once
#ifndef UTILITY_MATH_H
#define UTILITY_MATH_H

#include <List>
#include <string>
#include <vector>
#include <ostream>
#include <iostream>

/******************************************************************************
  gets all primes under a given amount
  slower than IsPrime for one prime, but faster for multiple primes
******************************************************************************/
template <class NaturalNumber>
std::vector<NaturalNumber> PrimeSieve(NaturalNumber input)
{
  std::vector<NaturalNumber> primes;
  bool* sieve = new bool[input];

  // save the next prime
  for (NaturalNumber prime = 2; prime < input / 2; prime++)
  {
    // we can skip this number as it has already be evaluated to be not prime
    if (sieve[prime] == false)
      continue;

    primes.push_back(prime);

    // stamp each multiple of this prime number
    for (NaturalNumber multiple = prime; multiple < input; multiple += prime)
    {
      sieve[multiple] = false;
    }
  }

  delete[] sieve;

  return primes;
}

/******************************************************************************
  tells you if this number is prime
  faster than Sieve for checking a single prime - not many
******************************************************************************/
template <class NaturalNumber>
bool IsPrime(NaturalNumber input)
{
  for (NaturalNumber i = 2; i < input; i++)
  {
    if (input % i == 0)
      return false;
  }

  return true;
}


class Huge
{
public:
  typedef unsigned char byte;

  //***** Construction *****//

  /****************************************************************************
      used for really big numbers - stored as a list internally
      probably a space hog - but you wanted a really big datatype right?
  ****************************************************************************/
  Huge(unsigned long long init = 0);
  Huge(const std::string& number);

  //***** Operators *****//

  void addString(std::string input);

  template <class NaturalNumber>
  void operator*=(NaturalNumber input)
  {
    std::list<byte>::iterator baseCurrent = baseArray_.begin();
    std::list<byte> basePrime;
    std::list<byte>::iterator primeCurrent = basePrime.begin();
    basePrime.resize(baseArray_.size());

    // build a vector of input digits
    std::vector<byte> inputArray;
    while (input)
    {
      inputArray.push_back(input % 10);
      input /= 10;
    }

    // multiply each value at the appropriate decimal level
    byte decimal = 0;
    byte carry = 0;
    for (byte inDigit : inputArray)
    {
      primeCurrent = basePrime.begin();
      baseCurrent = baseArray_.begin();

      // shift the sums down to the next decimal place
      byte currentDecimal = 0;
      while (currentDecimal < decimal)
      {
        primeCurrent++;
        currentDecimal++;
      }

      // multiply - but keep the values in out temp
      while (primeCurrent != basePrime.end())
      {
        // verify that our lists are big enough
        if (primeCurrent == basePrime.end())
        {
          baseArray_.push_back('\0');
          basePrime.push_back('\0');
          primeCurrent--;
          baseCurrent--;
        }

        // multiply
        (*primeCurrent) += inDigit * (*baseCurrent) + carry;

        //generate carry
        carry = (*primeCurrent) / 10;
        (*primeCurrent) %= 10;

        primeCurrent++;
        baseCurrent++;
      }

      // there could be one more carry
      while (carry)
      {
        if (primeCurrent == basePrime.end())
        {
          baseArray_.push_back(byte('\0'));
          basePrime.push_back(byte('\0'));
          primeCurrent--;
        }

        // carry bit
        (*primeCurrent) += carry;
        carry = (*primeCurrent) / 10;
        (*primeCurrent) %= 10;
        //cout << std::bitset<8>(*current) << '\n';

        primeCurrent++;
      }

      decimal++;
    }

    baseArray_.clear();
    while (!basePrime.empty())
    {
      baseArray_.push_back(basePrime.front());
      basePrime.pop_front();
    }
  }

  std::list<byte>::const_iterator begin() const;
  std::list<byte>::const_iterator end() const;

  friend std::ostream& operator<<(std::ostream& os, const Huge& huge)
  {
    if (huge.baseArray_.empty())
      os << "0";
    else
    {
      auto current = huge.baseArray_.end();
      while (current != huge.baseArray_.begin())
      {
        current--;
        os << char((*current) + '0');
      }
    }
    return os;
  }


  //***** Getters / Setters *****//

  /****************************************************************************
      Override current values with these values
  ****************************************************************************/
  void setValue(unsigned long long value);

  unsigned long size() const;

  bool isDivisible(unsigned div);
  unsigned long long getNumber();
  void subtractFromFront(unsigned value);

private:

  //***** Private Functions *****//

  std::list<byte> baseArray_;
};

#endif