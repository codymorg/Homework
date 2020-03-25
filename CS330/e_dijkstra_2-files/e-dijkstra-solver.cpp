#include "e-dijkstra-solver.h"

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::istringstream;
#include <iostream>

#define COUT if(true)std::cout

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
        if (grid[name][*(currentPath.end() - 2)] > range)
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
    COUT << "car status <";
    for (auto i : currentPath)
      COUT << i << " ";
    COUT << ">" << " " << fuel_ << " " << recharges_ << "\n";

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
                  COUT << "[" << i << "]";
                }
                else
                  COUT << i << " ";
              }
              COUT << "\n";
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
  else
  {
    COUT << file << "failed to open\n";
  }
}

bool CanReachAll(int start, int rangeGuess)
{
  range = rangeGuess;
  isPossible = false;
  vector<int> path{ start };
  Node root(start, rangeGuess, recharges, path);
  return isPossible;
}

int e_dijkstra_solver(char const* input)
{
  string in = input;
  string file ="C:/Users/Cody/source/repos/Homework/CS330/e_dijkstra_2-files/" + in;
  LoadFile(file);
  for (auto i = 0; i < grid.size(); i++)
  {
    bool canReach = CanReachAll(i,300);
    COUT << ":::   Can Reach all nodes from " <<i << " : " << std::boolalpha << canReach << "\n\n";
  }

  return 0;
}
