/* Matrix.cpp
*  Cody Morgan
*  Homework 2
*  03 OCT 2018
*/
#include "Matrix.h"
#include <stdexcept>
#include <initializer_list>


using std::initializer_list;
using std::runtime_error;

/*
* Combine Matrices
* Inputs: M       - Matrix of dim x by y
*         N       - Matrix of dim x by y
*         bScaler - multiply matrix N by this amount
*                   use -1 to subtract matrices 
* throws an error if they do not match
* 
*/
Matrix CombineMatrices(const Matrix &M, const Matrix &N, float nScaler)
{
  if (M.getCols() != N.getCols() ||
      M.getRows() != N.getRows())
  {
    throw runtime_error("dim mismatch");
  }

  //make a new matrix to store our shtuff
  Matrix newMat(M);

  //add the matrices
  for (int row = 0; row < M.getRows(); row++)
  {
    for (int col = 0; col < M.getCols(); col++)
    {
      float value = M[row][col] + (nScaler * N[row][col]);
      newMat[row][col] = value;
    }
  }

  return newMat;
}

/*
* Default constructor inits the matrix to r X c
* inputs: r - rows
*         c - columns
*/
Matrix::Matrix(unsigned r, unsigned c) : rows(r), cols(c)
{
  data = new float[r * c]();
};

/*
* Copy constructor
* deep copies the data 
* Inputs: M - matrix to copy
*/
Matrix::Matrix(const Matrix &M) : rows(M.rows), cols(M.cols)
{
  data = new float[rows * cols]();

  for (int i = 0; i < rows * cols; i++)
  {
    data[i] = M.data[i];
  }
}

/*
* Destructor - deletes internal array
*/
Matrix::~Matrix(void)
{
  delete[] data;
  data = nullptr;
}

/*
* Assignment operator
* Copy the old into the new
*/
Matrix &Matrix::operator=(const Matrix &M)
{
  if (M.getCols() * M.getRows() != this->cols * this->rows)
  {
    delete[] data;
    data = new float[M.getCols() * M.getRows()]();
  }
  cols = M.getCols();
  rows = M.getRows();

  for (int i = 0; i < rows * cols; i++)
  {
    this->data[i] = M.data[i];
  }

  return *this;
}

/*
* Subscript operator
* returns the row of the matrix
* Inputs: r - row index
*/
const float *Matrix::operator[](unsigned r) const
{
  const float *arrayIndex = &(data[r * this->cols]);

  return arrayIndex;
}

/*
* Subscript operator
* returns the row of the matrix
* Inputs: r - row index
*/
float *Matrix::operator[](unsigned r)
{
  return &(data[r * this->cols]);
}

/*
* Move Constructor
* moves the data from the right hand ref int othis constructed matrix
*/
Matrix::Matrix(Matrix &&M) noexcept : rows(M.rows), cols(M.cols)
{
  data = M.data;
  M.data = nullptr;
}

/*
* Move assignment operator
* moves the data from the right hand ref to this object
*/
Matrix &Matrix::operator=(Matrix &&M) noexcept
{
  this->rows = M.rows;
  this->cols = M.cols;

  //move the data to this new array
  if (rows * cols != M.getCols() * M.getRows())
  {
    delete[] data;
    data = new float[rows * cols];
  }
  delete[] data;
  data = M.data;
  M.data = nullptr;

  return *this;
}

/*
* Copy Constructor for Init Lists
* copies the values fron each list into the matrix only if a properly formatted
* set of init lists is provided. throws an exception otherwise
*/
Matrix::Matrix(std::initializer_list<std::initializer_list<float>> list)
{
  //check the size of every list to make sure they match
  int size = 0;
  for (initializer_list<float> eachList : list)
  {
    if (size && size != eachList.size())
    {
      throw runtime_error("bad initalizer list");
    }
    size = eachList.size();
  }

  this->rows = list.size();
  this->cols = size;
  this->data = new float[rows * cols]();

  int i = 0;
  //iterate over each list
  for (initializer_list<float> eachList : list)
  {
    //iterate through the list over each value
    for (float value : eachList)
    {
      this->data[i++] = value;
    }
  }
}

/*
* Addition operator
* adds 2 matrices of matching dimensions
* throws a runtime error if they dont match
* Inputs: M - matrix of dim x by y
          N - matrix of dim x by y
*/
Matrix operator+(const Matrix &M, const Matrix &N)
{
  //combine matrices and scale N by 1
  return CombineMatrices(M, N, 1);
}

/*
* Subtraction operator
* subtracts 2 matrices of matching dimensions
* throws a runtime error if they dont match
* Inputs: M - matrix of dim x by y
          N - matrix of dim x by y
*/
Matrix operator-(const Matrix &M, const Matrix &N)
{
  //combine matrices and scale N by -1
  return CombineMatrices(M, N, -1);
}

/*
* Multiplication operator
* multiplies these matrices and returns an error if their is a dim mismatch
*/
Matrix operator*(const Matrix &M, const Matrix &N)
{
  if (M.getCols() != N.getRows())
  {
    throw runtime_error("dim mismatch");
  }

  //M(r x c) * N(r x c) = P(Mr x Nc)
  Matrix newMat(M.getRows(), N.getCols());

  int mRow = 0; //represents the result's "active" row
  int nCol = 0; //represents the result's "active" col
  //run through the matrix Mr * Nc times resulting in a matrix with matching dim
  for (int count = 0; count < M.getRows() * N.getCols(); count++)
  {
    float value = 0.0f;
    //i = the "active" member in each matrix
    for (int i = 0; i < M.getCols(); i++)
    {
     value += M[mRow][i] * N[i][nCol];
    }
    newMat[mRow][nCol] = value;
    nCol = (nCol + 1) % N.getCols(); //move to the next column
    if (nCol % N.getCols() == 0)
      mRow++;
  }

  return newMat;
}

/*
* Scale Operator
* scale the given atrix by this value
*/
Matrix operator*(float s, const Matrix &M)
{
  //make a new matrix to store our shtuff
  Matrix newMat(M);

  //add the matrices
  for (int row = 0; row < M.getRows(); row++)
  {
    for (int col = 0; col < M.getCols(); col++)
    {
      float value = M[row][col] * s;
      newMat[row][col] = value;
    }
  }

  return newMat;
}
