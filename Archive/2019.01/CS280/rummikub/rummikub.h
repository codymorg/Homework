#ifndef RUMMIKUB_H
#define RUMMIKUB_H
#pragma once

#include <fstream>
#include <vector>
#include <deque>
#include <iostream>

<<<<<<< .mine
enum Color { Red = 0, Green, Blue, Yellow };
||||||| .r203
#define DENOMINATION_MAX 13
=======
enum Color { Red=0, Green, Blue, Yellow};
>>>>>>> .r204


<<<<<<< .mine
struct Tile
||||||| .r203

struct Tile 
=======
struct Tile 
>>>>>>> .r204
{
  int     denomination;
  Color   color;
};

inline std::ostream& operator<<(std::ostream& os, Tile const& t) {
  os << "{ " << t.denomination << ",";
  switch (t.color) {
  case Red:      os << "Diamond"; break;
  case Green:    os << "Spades"; break;
  case Blue:     os << "Hearts"; break;
  case Yellow:   os << "Clubs"; break;
  }
  os << " }";
  return os;
}

class RummiKub
{
public:
  RummiKub(); // empty hand
  void Add(Tile const&); // add a tile to hand

  void Solve(); // solve

  // get solution - groups
  std::vector< std::vector< Tile > > GetGroups() const;

  // get solution - runs
  std::vector< std::vector< Tile > > GetRuns() const;

  void printSolution();
private:
  // if both vectors are empty - no solution possible
  std::vector<std::vector<Tile>> groups_;
  std::vector<std::vector<Tile>> runs_;
  std::deque<Tile> hand_;
  size_t handSize_ = 0;

<<<<<<< .mine
  bool addToGroup(size_t index, size_t openSlot);
  bool addToRun(size_t index, size_t openSlot);
  void newGroup(size_t index);
  void newRun(size_t index);
||||||| .r203
  int handMatrix_[ColorCount][DENOMINATION_MAX];
=======
  bool addToGroup(size_t index, size_t openSlot);
  bool addToRun  (size_t index, size_t openSlot);
  void newGroup  (size_t index);
  void newRun    (size_t index);
>>>>>>> .r204

<<<<<<< .mine
  void undoRun(size_t openSlot);
  void undoGroup(size_t openSlot);
||||||| .r203
  bool addToGroup(unsigned index, unsigned openSlot);
  bool addToRun  (unsigned index, unsigned openSlot);
  void newGroup  (unsigned index);
  void newRun    (unsigned index);
=======
  void undoRun  (size_t openSlot);
  void undoGroup(size_t openSlot);
>>>>>>> .r204

  bool validateSolution();
  bool solve_rec();

<<<<<<< .mine
  typedef bool(RummiKub::*action)(size_t, size_t);
  action actions[2] =
||||||| .r203
  // DEBUG //
  void printHand();
  void printSolution();
  const char* debugNames_[2] = {"add to Run","add to Group" };

  typedef bool(RummiKub::*action)(unsigned, unsigned);
  action actions[2] = 
=======
  typedef bool(RummiKub::*action)(size_t, size_t);
  action actions[2] = 
>>>>>>> .r204
  {
    &RummiKub::addToRun,
    &RummiKub::addToGroup,
  };
  size_t actionCount_ = sizeof(actions) / sizeof(action);

  typedef void(RummiKub::*undoAction)(size_t);
  undoAction undo[2] =
  {
    &RummiKub::undoRun,
    &RummiKub::undoGroup,
  };

<<<<<<< .mine
  // DEBUG //
  void printHand();
  const char* debugNames_[2] = { "add to Run","add to Group" };

||||||| .r203
=======
  // DEBUG //
  void printHand();
  void printSolution();
  const char* debugNames_[2] = { "add to Run","add to Group" };

>>>>>>> .r204
};


#endif
