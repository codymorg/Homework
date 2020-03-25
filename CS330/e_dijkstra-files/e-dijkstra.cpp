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

class Node
{
public:
  Node(int name, int fuel, int recharges, vector<int>& currentPath) :
    name_(name), fuel_(fuel), recharges_(recharges), valid_(true)
  {
    // is this node valid?
    if (fuel_ < 0)
    {
      if (recharges_ > 0)
      {
        if (grid[name][*(currentPath.end()-2)] > range)
        {
          valid_ = false;
          //std::cout << "\ttoo far for vehicle to ever reach " << name << "\n";
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
        //std::cout << "\tinsufficient fuel to reach " << name << "\n";
      }
    }
    std::cout << "car status <";
    for (auto i : currentPath)
      std::cout << i << " ";
    std::cout << ">" << " " << fuel_ << " " << recharges_ << "\n";

    if (valid_)
    {
      // look at all the roads from here
      for (int i = 0; i < grid.size(); i++)
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
              for (auto i : currentPath)
              {
                if (i == currentPath.back())
                {
                  std::cout << "[" << i << "]";
                }
                else
                  std::cout << i << " ";
              }
              std::cout << "\n";
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
  bool valid_;
  int fuel_;
  int recharges_;
  int name_;
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
  vector<int> path{ 0 };
  Node root(0, range, recharges, path);
  return isPossible;
}
