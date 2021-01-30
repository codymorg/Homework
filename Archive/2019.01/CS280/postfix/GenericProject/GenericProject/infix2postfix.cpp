/******************************************************************************/
/*!
\file   infix2postfix
\author Cody Morgan
\par    email: cody.morgan\@digipen.edu
\par    DigiPen login: cody.morgan
\par    Course: CS280
\par    Section A
\par    Lab #2
\date   28 JAN 2019
\brief
  convert infix to postfix notation and sole postfix notation problems

*/
/******************************************************************************/

#define WRITE_OUT false

#include <stack>           // operators
#include <iostream>        // cout debug
#include "infix2postfix.h" // infix to postfix header

using std::stack;
using std::string;
using std::ostream;
using std::cout;

// Private Functions //

// this is used to process operators and to test for operators
class Operator
{
public:
  Operator(char character) : ansi(character)
  {
    //why couldn't ansi operators be in order??!
    switch (character)
    {
    case '(':
      priority = 1;
      break;

    case ')':
      priority = 1;
      break;

    case '*':
      priority = 4;
      break;

    case '/':
      priority = 4;
      break;

    case '+':
      priority = 3;
      break;

    case '-':
      priority = 2;
      break;

    default:
      break;
    }
  }

  //for comparing operators
  bool operator>(const Operator& right) const
  {
    if (right)
      return this->priority > right.priority;
    else
      return true;
  }

  bool operator>=(const Operator& right)const
  {
    if (right)
      return this->priority >= right.priority;
    else
      return true;
  }

  bool operator==(const char& right)const
  {
    return this->ansi == right;
  }

  bool operator!=(const char& right)const
  {
    return this->ansi != right;
  }

  //for determining if this is in fact an operator
  operator bool()const
  {
    return priority;
  }

  //for converting back to char
  operator char()const
  {
    return ansi;
  }

  //print this bad boy
  friend ostream& operator<<(ostream& os, const Operator& op)
  {
    return os << op.ansi;
  }

  // combine two numbers
  int combine(int left, int right)
  {
    switch (ansi)
    {
    case '*':
      return left * right;
    case '/':
      return right / left;
    case '+':
      return left + right;
    case '-':
      return right - left;
    default:
      cout << "you can't combine " << left << " and " << right << "\n";
      return 0;
    }
  }

private:
  char ansi = 0;
  unsigned priority = 0;
};


// Public Functions //

/******************************************************************************
* brief : converts infix notation to postfix
*
* input : an expression consisting of single digit characters
*         
* return: postfix notation equation
******************************************************************************/
string Infix2postfix(string const& infix)
{
  if (WRITE_OUT) cout << "====== infix: " << infix << "\n";

  string postfix;
  postfix.reserve(infix.size());
  stack<Operator> operations;

  //process all the characters of the expression
  for (size_t i = 0; i < infix.size(); i++)
  {
    Operator operation(infix[i]);
    if (WRITE_OUT) cout << "processing " << infix[i] << "\n";

    if (operation) //this is an operator
    {
      // stack low priority operators and openine parethesis
      if (operations.empty() || operation == '(' || operation >= operations.top())
      {
        if (WRITE_OUT) cout << "pushing " << operation << "\n";

        operations.push(operation);
      }

      //close the loop
      else if (operation == ')')
      {
        if (WRITE_OUT) cout << "closing loop " << "\n";

        //upload all operators inside the parenthisis
        while (operations.top() != '(')
        {
          if (WRITE_OUT) cout << "writing... " << operations.top() << "\n";

          postfix.push_back(operations.top());
          operations.pop();
        }

        //pop off that parethisis
        operations.pop();
      }

      // stack low priority operators and write high priority
      else
      {
        if (WRITE_OUT) cout << "writing high " << operations.top() << "\n";

        //write the high priority
        postfix.push_back(operations.top());
        operations.pop();

        if (WRITE_OUT) cout << "pushing low " << operation << "\n";

        //save the low priority for later
        operations.push(operation);
      }
    }

    else // this is a number, character or other thing
    {
      if (WRITE_OUT) cout << "writing number " << infix[i] << "\n";
      postfix.push_back(infix[i]);
    }
  }

  //write out any remaining operations
  while (!operations.empty())
  {
    if (WRITE_OUT) cout << "writing " << operations.top() << "\n";
    postfix.push_back(operations.top());
    operations.pop();
  }

  if (WRITE_OUT) cout << "postfix = " << postfix << "\n";
  return postfix;
}

/******************************************************************************
* brief : evalute postfix notation equations 
*
* input : a postfix notation equation
*
* return: the result of the equation
******************************************************************************/
int Evaluate(string const& postfix)
{
  if (WRITE_OUT) cout << "====== postfix: " << postfix << "\n";
  stack<int> operands;
  for (size_t i = 0; i < postfix.size(); i++)
  {
    Operator operation(postfix[i]);

    if (operation) //this is an operator
    {
      int top = operands.top();
      operands.pop();

      //in case only one digit is provided and an operator
      int bottom = 0;
      if (!operands.empty())
        bottom = operands.top();

      int combined = operation.combine(top, bottom);
      if (WRITE_OUT) cout << "combining " << top << " " << operation << " " << bottom << " = " << combined << "\n";

      //single operand case
      if (!operands.empty())
        operands.pop();
      operands.push(combined);
    }

    //just a number - push it out
    else
    {
      char number = postfix[i];

      if (WRITE_OUT) cout << "pushing number " << number << "\n";

      if(number >= '0' && number <= '9')
        operands.push(number - 48);
      else if (WRITE_OUT) 
        cout << "invalid input" << number << "\n";

    }
  }

  if (WRITE_OUT) cout << " = " << operands.top() << "\n";
  return operands.top();
}


