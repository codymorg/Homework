#include "e-dijkstra.h"
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::istringstream;
#include <iostream>

static vector<vector<int>> grid;
static int recharges;
static int range;
static bool isPossible;
static vector<int> visisted;

#define COUT if(0)std::cout

class Node
{
public:
  Node(int name, int fuel, int recharges, vector<int>& currentPath) :
    name_(name), fuel_(fuel), recharges_(recharges), valid_(true), connections_()
  {
    // is this node valid?
    if (fuel_ < 0)
    {
      if (recharges_ > 0)
      {
        if (grid[name][*(currentPath.end()-2)] > range)
        {
          valid_ = false;
          //COUT << "\ttoo far for vehicle to ever reach " << name << "\n";
        }
        else
        {
          recharges_--;
          fuel_ = range - grid[name][*(currentPath.end() - 2)];
        }
      }
      else
      {
        valid_ = false;
        //COUT << "\tinsufficient fuel to reach " << name << "\n";
      }
    }
    //COUT << "car status <";
    //for (auto i : currentPath)
    //  COUT << i << " ";
    //COUT << ">" << " " << fuel_ << " " << recharges_ << "\n";

    if (valid_)
    {
      visisted[name_]++;
      //COUT << "visited: ";
      bool isDone = true;
      for (auto i : visisted)
      {
        //std::cout << i << ' ';
        if (i == 0)
          isDone = false;
      }
      //std::cout << "\n";
      if (isDone)
      {
        isPossible = true;
        return;
      }

      // look at all the roads from here
      for (int i = 0; i < int(grid.size()); i++)
      {
        int road = grid[name_][i];
        // valid roads
        if (road > 0)
        {
          // no double backing
          bool isInPath = false;
          for (auto path : currentPath)
          {
            isInPath |= path == i;
          }
          if (!isInPath)
          {
            currentPath.push_back(i);
            connections_.push_back(Node(i, fuel_ - road, recharges_, currentPath));
            if (currentPath.size() == grid.size() && connections_.back().valid_)
            {
              isPossible = true;
              return;
            }
            else
            {
              isPossible = false;
              //for (auto i : currentPath)
              //{
              //  if (i == currentPath.back())
              //  {
              //    COUT << "[" << i << "]";
              //  }
              //  else
              //    COUT << i << " ";
              //}
              //COUT << "\n";
              currentPath.pop_back();
            }
          }
        }
      }
    }
  }
  
  bool isValid() 
  {
    return valid_; 
  }



private:
  int name_;
  int fuel_;
  int recharges_;
  bool valid_;
  vector<Node>connections_;
};

void LoadFile(const string& file)
{
  string line;
  ifstream myfile(file);

  if (myfile.is_open())
  {
    getline(myfile, line);
    int nodes;
    auto ss = istringstream(line);
    ss >> nodes;
    ss >> recharges;
    recharges--;

    // create grid
    grid.resize(nodes);
    for (auto& vec : grid)
    {
      vec.resize(nodes);
    }

    // load nodes
    while (getline(myfile, line))
    {
      int nodeA;
      int nodeB;
      int weight;
      auto ss = istringstream(line);
      ss >> nodeA;
      ss >> nodeB;
      ss >> weight;

      grid[nodeA][nodeB] = weight;
      grid[nodeB][nodeA] = weight;
    }
    myfile.close();
  }
}



bool e_dijkstra(char const* input, int rng)
{
  range = rng;
  LoadFile(input);
  
  for (int i = 0; i < int(grid.size()); i++)
  {
    isPossible = true;
    vector<int> path{i};
    visisted.clear();
    visisted.resize(grid.size());
    Node root(i, range, recharges, path);
    //COUT << "visited: ";
    bool isDone = true;
    for (auto i : visisted)
    {
      COUT << i << ' ';
      if (i == 0)
        isDone = false;
    }
    COUT << "\n";
    if (isDone)
    {
      isPossible = true;
    }

    //std::cout << "RESULT ::: from " << i << " " <<  std::boolalpha << isPossible << "\n";
    if (isPossible == false)
      break;
  }
  return isPossible;
}
