/******************************************************************************/
/*!
\file   Rummikub.cpp
\author Cody Morgan
\par    email: cody.morgan\@digipen.edu
\par    DigiPen login: cody.morgan
\par    Course: CS280
\par    Section A
\date   27 FEB 2019
\brief
  Solves a Rummikub hand if possible
*/
/******************************************************************************/

#include "rummikub.h"

<<<<<<< .mine
using std::vector;// containers for tiles
using std::cout;
//////////  Debugging  //////////
||||||| .r203
using std::vector;
using std::cout;
using std::for_each;
=======
using std::vector;// containers for tiles
>>>>>>> .r204

<<<<<<< .mine
#define debug false
#define COUT if(debug) std::cout
||||||| .r203
// debugging only
#define COUT if(true) cout
=======
//////////  Debugging  //////////
>>>>>>> .r204

<<<<<<< .mine
/******************************************************************************
* brief : DEBUG print current hand of tiles
*
* input :
*
* output:
******************************************************************************/
||||||| .r203
RummiKub::RummiKub()
{
}

void RummiKub::Add(const Tile& tile)
{
  handSize_++;
  handMatrix_[tile.color][tile.denomination]++;

  // sort by denomination
  if (hand_.empty() || tile.denomination < hand_.back().denomination)
  {
    hand_.push_back(tile);
  }
  else
  {
    for (unsigned i = 0; i < hand_.size(); i++)
    {
      if (tile.denomination > hand_[i].denomination)
      {
        hand_.insert(hand_.begin() + i, tile);
        break;
      }
    }
  }
  printHand();
}

// returns true if it is done checking - sets solved if it found an answer
bool RummiKub::validateSolution()
{
  if (solved)
    return true;

  // is this solution valid?
  if (hand_.size() == 0)
  {
    // check the size of every run
    for (unsigned run = 0; run < runs_.size(); run++)
    {
      if (!runs_[run].empty() && runs_[run].size() < 3)
      {
        COUT << "empty hand, no no valid solution found\n";
        return false;
      }
    }

    //check the size of every group
    for (unsigned group = 0; group < groups_.size(); group++)
    {
      if (!groups_[group].empty() && groups_[group].size() < 3)
      {
        COUT << "empty hand, no no valid solution found\n";
        return false;
      }
    }

    // runs and groups are valid
    solved = true;

    return true;
  }
  else
  {
    return false;
  }
}

=======
#define debug false
#define COUT if(debug) std::cout

/******************************************************************************
* brief : DEBUG print current hand of tiles
*
* input :
*
* output:
******************************************************************************/
>>>>>>> .r204
void RummiKub::printHand()
{
  COUT << "current hand : \n";
  for (size_t i = 0; i < hand_.size(); i++)
  {
    COUT << hand_[i] << " ";
  }
  COUT << "\n\n";
  COUT << "\n\n";

}

/******************************************************************************
* brief : DEBUG print current soltion
*
* input :
*
* output:
******************************************************************************/
void RummiKub::printSolution()
{
<<<<<<< .mine
  cout << "Current Solution: \n";
  cout << "---- Runs ----\n";
  for (size_t run = 0; run < runs_.size(); run++)
||||||| .r203
  COUT << "Current Solution: \n";
  COUT << "---- Runs ----\n";
  for (unsigned run = 0; run < runs_.size(); run++)
=======
  COUT << "Current Solution: \n";
  COUT << "---- Runs ----\n";
  for (size_t run = 0; run < runs_.size(); run++)
>>>>>>> .r204
  {
<<<<<<< .mine
    cout << "< ";
    for (size_t t = 0; t < runs_[run].size(); t++)
||||||| .r203
    COUT << "< ";
    for (unsigned t = 0; t < runs_[run].size(); t++)
=======
    COUT << "< ";
    for (size_t t = 0; t < runs_[run].size(); t++)
>>>>>>> .r204
    {
      cout << runs_[run][t] << " ";

    }
    cout << ">\n";
  }


<<<<<<< .mine
  cout << "\n---- Groups ----\n";
  for (size_t group = 0; group < groups_.size(); group++)
||||||| .r203
  COUT << "\n---- Groups ----\n";
  for (unsigned group = 0; group < groups_.size(); group++)
=======
  COUT << "\n---- Groups ----\n";
  for (size_t group = 0; group < groups_.size(); group++)
>>>>>>> .r204
  {
<<<<<<< .mine
    cout << "< ";
    for (size_t t = 0; t < groups_[group].size(); t++)
||||||| .r203
    COUT << "< ";
    for (unsigned t = 0; t < groups_[group].size(); t++)
=======
    COUT << "< ";
    for (size_t t = 0; t < groups_[group].size(); t++)
>>>>>>> .r204
    {
      cout << groups_[group][t] << " ";
    }
<<<<<<< .mine
    cout << " >\n";
||||||| .r203
    COUT << " >  ";
=======
    COUT << " >\n";
>>>>>>> .r204
  }
  cout << "\n";
}


//////////  Public Funcitons  //////////


/******************************************************************************
* brief : construct a new hand of tiles
*
* input :
*
* output:
******************************************************************************/
RummiKub::RummiKub()
{
}

/******************************************************************************
* brief : Add a tile to the hand (sorts big to small)
*
* input : tile - the tile to add to the hand
*
* output:
******************************************************************************/
void RummiKub::Add(const Tile& tile)
{
  // sort by denomination
  if (hand_.empty() || tile.denomination <= hand_.back().denomination)
  {
    hand_.push_back(tile);
  }
  else
  {
    for (size_t i = 0; i < hand_.size(); i++)
    {
      if (tile.denomination > hand_[i].denomination)
      {
        hand_.insert(hand_.begin() + i, tile);
        break;
      }
    }
  }
<<<<<<< .mine
||||||| .r203

  printHand();
  groups_.clear();
=======
  printHand();
>>>>>>> .r204
}

/******************************************************************************
* brief : get the groups solution
*
* input :
*
* output: the groups found to be valid
******************************************************************************/
vector<vector<Tile>> RummiKub::GetGroups() const
{
  return groups_;
}

/******************************************************************************
* brief : get the runs solution
*
* input :
*
* output: the runs found to be valid
******************************************************************************/
vector<vector<Tile>> RummiKub::GetRuns() const
{
  return runs_;

}

/******************************************************************************
* brief : kickstart the recursion
*
* input :
*
* output:
******************************************************************************/
void RummiKub::Solve()
{
<<<<<<< .mine
  handSize_ = hand_.size();
  
  bool solved = solve_rec();
  if (solved)
    cout << "\nsolved !!!\n\n\n";
  else
    cout << "\nunsolvable :(\n\n\n";
||||||| .r203
  solved = solve_rec(hand_.size());
=======
  handSize_ = hand_.size();
  solve_rec();
>>>>>>> .r204
}


//////////  Private Functions  //////////


/******************************************************************************
* brief : run the recursion
*
* input :
*
* output: true  - solution found
          false - not found
******************************************************************************/
bool RummiKub::solve_rec()
{
  COUT << "\n\n\n\n\n==========================================\n";
  printHand();

  //base case
  if (hand_.empty())
    return validateSolution();

  // identifies which range is valid for this action i.e runs range from 0-runs.size()
  size_t sequenceSize[] =
  {
    runs_.size(),
    groups_.size()
  };

  // for every action
  for (size_t action = 0; action < actionCount_; action++)
  {
    // do action if rec returns true pass it up
    for (size_t sequenceIndex = 0; sequenceIndex <= sequenceSize[action]; sequenceIndex++)
    {
      size_t index = hand_.size() - 1;
      COUT << "Performing action: " << debugNames_[action] << " on " << hand_[index] << " :[" << sequenceIndex << "]\n";

      // perform the action
      if ((this->*(actions[action]))(index, sequenceIndex))
      {
        // if a solution was found - return it
        if (solve_rec())
          return true;

        // undo otherwise
        (this->*(undo[action]))(sequenceIndex);
      }
    }
  }

  // no solution found in this banch
  return false;
}

/******************************************************************************
* brief : determines if this solution is correct
*
* input :
*
* output: true  - this solution is valid
*         false - this is not valid
******************************************************************************/
bool RummiKub::validateSolution()
{
  // check the size of every run
  for (size_t run = 0; run < runs_.size(); run++)
  {
    if (!runs_[run].empty() && runs_[run].size() < 3)
    {
      COUT << "empty hand, no no valid solution found\n";
      return false;
    }
  }

  //check the size of every group
  for (size_t group = 0; group < groups_.size(); group++)
  {
    if (!groups_[group].empty() && groups_[group].size() < 3)
    {
      COUT << "empty hand, no no valid solution found\n";
      return false;
    }
  }

  // runs and groups are valid
  return true;
}


//////////  Tile Functions  //////////


/******************************************************************************
* brief : add to a group if it meets these rules
*         3-4 tiles
*         same denominations
*         all different color
*
* input : index - index into the hand
*         group - group index
*         hand[index] ---> groups[group][back]
*
* output: true - only if it was able to add this tile to this group
******************************************************************************/
bool RummiKub::addToGroup(size_t index, size_t group)
{
  // dont give me out of range indices
  if (index >= hand_.size())
    return false;

  Tile tile = hand_[index];

  // are you inserting at the end?
  if (groups_.empty() || groups_.size() == group)
  {
    newGroup(index);
    return true;
  }

  // can you insert it where you want?                         
  if (groups_.size() > group &&                                // valid group index
    !groups_[group].empty() &&                               // valid group
    groups_[group].size() < 4 &&                             // legal group
    tile.color != groups_[group].front().color &&            // legal tile color
    tile.denomination == groups_[group].front().denomination)// legal tile denomination
  {
    COUT << "adding " << tile << " to group\n";

    groups_[group].push_back(tile);
    hand_.pop_back();

    return true;
  }

  COUT << tile << " cannot be added to any group\n";
  return false;
}

/******************************************************************************
* brief : add to a run if it meets these rules
*         3+ tiles
*         all the same color
*         denominations are consecutive
*
* input : index - hand index
*         run   - run index
*         hand[index] ---> runs[run][back]
*
* output: true - if it was able to add to that run
******************************************************************************/
bool RummiKub::addToRun(size_t index, size_t run)
{
  // try to add starting in slot s
  Tile& tile = hand_[index];

  // are you inserting at the end?
  if (runs_.empty() || runs_.size() == run)
  {
    newRun(index);

    return true;
  }

  // can it be inserted where you want?
  if (runs_.size() > run && runs_[run].front().color == tile.color)
  {
    // add to run if valid 
    if (runs_[run].back().denomination == tile.denomination - 1)
    {
      COUT << "adding " << tile << " to run \n";

      runs_[run].push_back(tile);
      hand_.erase(hand_.begin() + index);

      return true;
    }
  }

  COUT << tile << " cannot be added to any run\n";
  return false;
}

<<<<<<< .mine
/******************************************************************************
* brief : if a group was requested at the end of the groups a new group will
*         pushed tot he back
*
* input : index - hand index
*
* output:
******************************************************************************/
void RummiKub::newGroup(size_t index)
||||||| .r203

void RummiKub::newGroup(unsigned index )
=======
/******************************************************************************
* brief : if a group was requested at the end of the groups a new group will
*         pushed tot he back
*
* input : index - hand index
*
* output:
******************************************************************************/
void RummiKub::newGroup(size_t index )
>>>>>>> .r204
{
<<<<<<< .mine
  // make a new group
  vector<Tile>group = { hand_[index] };
||||||| .r203
  vector<Tile>group = {hand_.at(index)};
=======
  // make a new group
  vector<Tile>group = {hand_[index]};
>>>>>>> .r204
  groups_.push_back(group);

  // pop that tile
  hand_.erase(hand_.begin() + index);

  COUT << "creating group \n";
}

/******************************************************************************
* brief : creates a new run at the end of the runs vector
*
* input : index the hand index of this tile
*
* output:
******************************************************************************/
void RummiKub::newRun(size_t index)
{
<<<<<<< .mine
  //new run
  COUT << "creating run with " << hand_[index] << "\n";
  vector<Tile>run = { hand_[index] };
||||||| .r203
  COUT << "creating run with " << hand_.at(index) <<"\n";
  vector<Tile>run = {hand_.at(index)};
=======
  //new run
  COUT << "creating run with " << hand_[index] <<"\n";
  vector<Tile>run = {hand_[index]};
>>>>>>> .r204
  runs_.push_back(run);

  // pop that tile
  hand_.erase(hand_.begin() + index);

}

/******************************************************************************
* brief : undo a run
*
* input : run - the index of which run is going to be undone
*
* output:
******************************************************************************/
void RummiKub::undoRun(size_t run)
{
  COUT << "undoing run\n";
  Add(runs_[run].back());
  runs_[run].pop_back();

  //pop empties
  if (runs_[run].empty())
    runs_.erase(runs_.begin() + run);

  printHand();
}

/******************************************************************************
* brief : undo a group
*
* input : group - the group index
*
* output:
******************************************************************************/
void RummiKub::undoGroup(size_t group)
{
  COUT << "undoing group\n";

  Add(groups_[group].back());
  groups_[group].pop_back();

  //pop empties
  if (groups_[group].empty())
    groups_.erase(groups_.begin() + group);

  printHand();
}
