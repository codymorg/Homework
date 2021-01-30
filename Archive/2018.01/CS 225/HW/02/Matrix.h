// Matrix.h
// -- general matrix class, with C++11 improvements
// cs225 9/18

#ifndef CS225_MATRIX_H
#define CS225_MATRIX_H


#include <initializer_list>


class Matrix {
  public:
    Matrix(unsigned r, unsigned c);
    Matrix(const Matrix &M);
    ~Matrix(void);
    Matrix& operator=(const Matrix &M);
    const float* operator[](unsigned r) const;
    float* operator[](unsigned r);
    unsigned getRows(void) const { return rows; }
    unsigned getCols(void) const { return cols; }
    Matrix(Matrix &&M) noexcept;
    Matrix& operator=(Matrix &&M) noexcept;
    Matrix(std::initializer_list<std::initializer_list<float>> list);
  private:
    unsigned rows,
             cols;
    float *data;
};


Matrix operator+(const Matrix &M, const Matrix &N);
Matrix operator-(const Matrix &M, const Matrix &N);
Matrix operator*(const Matrix &M, const Matrix &N);
Matrix operator*(float s, const Matrix &M);


#endif