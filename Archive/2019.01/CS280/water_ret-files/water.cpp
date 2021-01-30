/******************************************************************************
* Name : Cody Morgan
* Class: CS 280
* Assn : Water Retention
* Date : 4 APR 2019
* Brief: calculate the amount of water retained by a given 3d shape
******************************************************************************/

#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using std::stack;
using std::ifstream;
using std::string;
using std::vector;
using std::max;

#define COUT_FILE if(false) std::cout
#define COUT      if(false) std::cout

static unsigned mapWidth = 0;
static unsigned mapHeight = 0;

// contains all the values
typedef struct Node Node;
static Node** map;
static stack<Node*>nonzero;

/******************************************************************************
* brief : merge two stacks into one
*         destroys the concat stack
* 
* input : returnstack - merge into this stack
*         concat      - merge this stack (delete)
* 
* output: none
******************************************************************************/
static void MergeStacks(stack<Node*>& returnStack, stack<Node*>& concat)
{
  while (!concat.empty())
  {
    returnStack.push(concat.top());
    concat.pop();
  }
}

/******************************************************************************
* brief   : node struct contains the info for each number in the array
* members : height    - given height
*           volume    - calculated volume 
*           neighbors - all the adjacent members that I point to
******************************************************************************/
struct Node
{
  unsigned index;
  int height;
  int volume = -1;
  unsigned row = 0;
  unsigned col = 0;
  bool sameSizeChecked = false;

  //infinite guard
  void validateParents(stack<Node*>& parents)
  {
    //guard against infinite loops
    if (this->height == parents.top()->height)
    {
      if (this->sameSizeChecked || parents.top()->sameSizeChecked)
      {
        parents.top()->sameSizeChecked = false;
        this->sameSizeChecked = false;
        parents.pop();
      }
      else
      {
        parents.top()->sameSizeChecked = true;
        this->sameSizeChecked = true;
      }
    }
  }

  // is this index a neighbor?
  bool isNeighbor(unsigned indexB)
  {
    return (index + 1 == indexB ||
      index - 1 == indexB ||
      index + mapWidth == indexB ||
      index - mapWidth == indexB);
  }

  // these four return the neighbor to the up,left,right,down
  Node* up()
  {
    int h = (index / mapWidth) - 1;
    int w = index % mapWidth;
    if (h < 0 )
      return nullptr;
    else
      return &(map[h][w]);
  }
  Node* right()
  {
    int h = (index / mapWidth);
    int w = (index % mapWidth) + 1;
    if (w >= int(mapWidth))
      return nullptr;
    else
      return &map[h][w];
  }
  Node* down()
  {
    int h = (index / mapWidth) + 1;
    int w = index % mapWidth;
    if (h >= int(mapHeight))
      return nullptr;
    else
      return &map[h][w];
  }
  Node* left()
  {
    int h = (index / mapWidth);
    int w = (index % mapWidth) - 1;
    if (w < 0)
      return nullptr;
    else
      return &map[h][w];
  }

  // only those neighbors who have lower height than me
  stack<Node*> getChildren()
  {
    stack<Node*> kids;
    Node* upP     = up();
    Node* rightP  = right();
    Node* downP   = down();
    Node* leftP   = left();

    // stuff valid children in the vector
    if (upP && upP->height < this->height)
      kids.push(upP);
    if (rightP && rightP->height < this->height)
      kids.push(rightP);
    if ( downP && downP->height < this->height)
      kids.push(downP);
    if (leftP && leftP->height < this->height)
      kids.push(leftP);

    return kids;
  }

  stack<Node*> getNeighbors()
  {
    stack<Node*> kids;
    Node* upP = up();
    Node* rightP = right();
    Node* downP = down();
    Node* leftP = left();

    kids.push(upP);
    kids.push(rightP);
    kids.push(downP);
    kids.push(leftP);

    return kids;
  }

  // only those neighbors who are higher than me
  stack<Node*> getParents()
  {
    stack<Node*> parents;

    Node* upP     = up();
    Node* rightP  = right();
    Node* downP   = down();
    Node* leftP   = left();


    // stuff valid children in the vector
    if (upP && upP->height >= this->height)
    {
      parents.push(upP);
      validateParents(parents);
    }
    if (rightP && rightP->height >= this->height)
    {
      parents.push(rightP);
      validateParents(parents);
    }
    if (downP && downP->height >= this->height)
    {
      parents.push(downP);
      validateParents(parents);
    }
    if (leftP)
    {
      COUT << "left: " << leftP->height;
    }
    else
    {
      COUT << "no left\n";
    }
    if (leftP && leftP->height >= this->height)
    {
      parents.push(leftP);
      validateParents(parents);
    }



    return parents;
  }

  // set volumes of parents to zero
  void zeroizeParents()
  {
    // stack my parents
    stack<Node*> zeroParents(getParents());

    // my volume is now 0
    volume = 0;

    // zeroize all the parents who point to me
    while (!zeroParents.empty())
    {
      Node* zero = zeroParents.top();
      zeroParents.pop();

      zero->volume = 0;

      // add all of your parents to the stack
      stack<Node*> parents = zero->getParents();
      MergeStacks(zeroParents, parents);
    }
  }

  // get shortest node neighbor
  Node* getShortestNeighbor()
  {
    vector<Node*> kids;
    kids.push_back(left());
    kids.push_back(down());
    kids.push_back(right());
    kids.push_back(up());

    Node* smallestNode = kids[0];
    Node* smallestEvaled = nullptr;
    stack<Node*>evaluated;

    // stack all neighbors who have been evaluated
    for (size_t i = 0; i < kids.size(); i++)
    {
      // already evaluated node
      if (kids[i]->volume != -1)
      {
        evaluated.push(kids[i]);

        //keep track of smallest evaluated node
        if (!smallestEvaled || smallestEvaled->volume + smallestEvaled->height > kids[i]->volume + kids[i]->height)
        {
          smallestEvaled = kids[i];
        }
      }

      // keep track of the smallest height in caase we need it
      if (kids[i]->height < smallestNode->height)
        smallestNode = kids[i];
    }

    // all neighbors are unevaluated
    if (evaluated.empty())
    {
      return smallestNode;
    }
    else
    {
      return smallestEvaled;
    }
  }

  // update water levels that may be too high
  void updateWaterLevel()
  {
    Node* current = this;
    stack<Node*> path;
    stack<Node*> unchecked;
    vector<unsigned>used;

    while (current)
    {
      Node* neighbor = current->up();
      if (neighbor->volume > 0 && neighbor->volume + neighbor->height > height + volume)
      {
        bool update = true;
        for (size_t i = 0; i < used.size(); i++)
        {
          if (neighbor->index == used[i])
          {
            update = false;
            break;
          }
        }
        if (update)
        {
          path.push(neighbor);
          unchecked.push(neighbor);
          used.push_back(neighbor->index);
        }
      }

      neighbor = current->right();
      if (neighbor->volume > 0 && neighbor->volume + neighbor->height > height + volume)
      {
        bool update = true;
        for (size_t i = 0; i < used.size(); i++)
        {
          if (neighbor->index == used[i])
          {
            update = false;
            break;
          }
        }
        if (update)
        {
          path.push(neighbor);
          unchecked.push(neighbor);
          used.push_back(neighbor->index);
        }
      }

      neighbor = current->down();
      if (neighbor->volume > 0 && neighbor->volume + neighbor->height > height + volume)
      {
        bool update = true;
        for (size_t i = 0; i < used.size(); i++)
        {
          if (neighbor->index == used[i])
          {
            update = false;
            break;
          }
        }
        if (update)
        {
          path.push(neighbor);
          unchecked.push(neighbor);
          used.push_back(neighbor->index);
        }
      }

      neighbor = current->left();
      if (neighbor->volume > 0 && neighbor->volume + neighbor->height > height + volume)
      {
        bool update = true;
        for (size_t i = 0; i < used.size(); i++)
        {
          if (neighbor->index == used[i])
          {
            update = false;
            break;
          }
        }
        if (update)
        {
          path.push(neighbor);
          unchecked.push(neighbor);
          used.push_back(neighbor->index);
        }
      }
    
      if (unchecked.empty())
      {
        current = nullptr;
      }
      else
      {
        current = unchecked.top();
        COUT << "moving to " << current->index << "\n";
      }
      if(!unchecked.empty())
        unchecked.pop();
    }

    while (!path.empty())
    {
      path.top()->volume = volume + height - path.top()->height;
      path.pop();
    }

  }

};

// read input and put into an array

static void Load(const char* fname)
{
  string line;
  ifstream myfile(fname);
  if (myfile.is_open())
  {
    COUT_FILE << "BEGIN file INPUT \n";

    int row = 0;
    int col = 0;

    int number;
    while (myfile >> number)
      {
        // find the size of the map
        if (!mapHeight)
        {
          mapHeight = number;
        }

        // allocate a new array
        else if(!mapWidth)
        {
          mapWidth = number;

          map = new Node*[mapHeight];
          for (size_t i = 0; i < mapHeight; i++)
          {
            map[i] = new Node[mapWidth];
          }
        }

        // fill the map
        else
        {
          map[row][col].height = number;
          map[row][col].index = row * mapWidth + col;
          map[row][col].row = row;
          map[row][col].col = col;

          COUT_FILE << number << "\t";
          col++;
          if (unsigned(col) == mapWidth)
          {
            COUT_FILE << "\n";
            row++;
            col = 0;
          }
        }
      }
  }
}

// debug print volumes
static void PrintVolumes()
{
  COUT << "\nVolume map\n";
  for (size_t h = 0; h < mapHeight; h++)
  {
    for (size_t w = 0; w < mapWidth; w++)
    {
      COUT << map[h][w].volume << '\t';
    }
    COUT << "\n";
  }
  COUT << "\n END\n";
}

// debug print heights
static void PrintHeights()
{
  COUT << "Height map\n";
  for (size_t h = 0; h < mapHeight; h++)
  {
    for (size_t w = 0; w < mapWidth; w++)
    {
      Node* test = &(map[h][w]);
      COUT << test->height << '\t';
    }
    COUT << "\n";
  }
  COUT << "\n END\n";
}



// find all the zero paths
static void Zeroize() 
{
  // top and bottom
  for (size_t w = 0; w < mapWidth; w++)
  {
    map[0][w].volume = 0;
    map[mapHeight - 1][w].volume = 0;
  }

  // left and right
  for (size_t h = 1; h < mapHeight; h++)
  {
    map[h][0].volume = 0;
    map[h][mapWidth - 1].volume = 0;
  }

}

// calculate the volume of every valid height
static long int CalculateVolume()
{
  long int totalVolume = 0;
  for (size_t h = 1; h < mapHeight - 1; h++)
  {
    for (size_t w = 1; w < mapWidth - 1; w++)
    {
      // find lowest neighbor
      Node* current = &(map[h][w]);
      Node* smallest = current->getShortestNeighbor();

      COUT << "current:  " << current->height << " [" << h << "][" << w << "]\n";
      COUT << "smallest: " << smallest->height << " [" << smallest->index / mapWidth << "][" << smallest->index % mapWidth << "]\n";


      switch (smallest->volume)
      {
      case -1:
        current->volume = smallest->height - current->height;
        break;

      case 0:
        current->volume = max(0, smallest->height - current->height);
        current->updateWaterLevel();
        break;

      default:
        current->volume = smallest->height + smallest->volume - current->height;
        break;
      }
      nonzero.push(current);
      PrintVolumes();
    }
  }

  while (!nonzero.empty())
  {
    if(nonzero.top()->volume > 0)
      totalVolume += nonzero.top()->volume;
    nonzero.pop();
  }

  return totalVolume;
}

static void Unload()
{
  for (size_t i = 0; i < mapHeight; i++)
  {
    delete[] map[i];
  }
  mapHeight = 0;
  mapWidth = 0;
}

// total the sum and return the value
long int waterret(char const* filename)
{
  Load(filename);
  PrintHeights();
  Zeroize();
  PrintVolumes();
  long int vol = CalculateVolume();
  COUT << "finished calculation:" << vol << "\n";
  PrintVolumes();

  Unload();

  return vol;
}