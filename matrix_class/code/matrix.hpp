#ifndef ADATALG_KHF01_MATRIX_HPP
#define ADATALG_KHF01_MATRIX_HPP

#include <filesystem>

#include <fstream>
#include <iostream>
#include <algorithm> // std::copy
#include <stdexcept> // std::out_of_range exception-höz kell
#include <exception>

///_______Exception(s)______________________

/*class InvalidCopyConsParameter : public std::exception
{
public:
    const char* what() const noexcept { return "The parameter of copy constructor is invalid (empty)!"; }
    // pre C++11, deprecated, removed in C++20
    // const char* what() const throw() { return "Alulcsordulas!"; }
};*/

///_____________________________________________

class matrix
{
private:
    size_t row, col, matSize;
    int *matArr;

public:
///______________ Segéd fv-ek __________________

    // paraméter nélküli konstruktor
    matrix();

    // paraméteres konstruktorok (initialisation)
    matrix(size_t r, size_t c);
    //matrix(size_t r, size_t c, const int* arr);

///______________________________________________

  //____________________ Rule of 5_____________
  // Destruktor
  ~matrix() noexcept;

  // copy konstruktor
  matrix(const matrix &m);

  // matrix assignment operator
  matrix &operator=(const matrix &m);

  // matrix move assignment operator
  matrix &operator=(matrix &&) noexcept;

  // matrix move konstruktor
  matrix(matrix &&) noexcept;
  //______________________________________________

  // paraméteres értékadó konstruktor
  explicit matrix(const std::filesystem::path &p);

  [[nodiscard]] matrix operator+(const matrix &m) const noexcept;
  [[nodiscard]] matrix operator*(const matrix &m) const noexcept;
  [[nodiscard]] int &at(size_t r, size_t c);
  [[nodiscard]] const int &at(size_t r, size_t c) const;
  [[nodiscard]] std::pair<size_t, size_t> size() const noexcept;
  void transpose() noexcept;

  void print() const noexcept;
};

///______ Segéd fv-ek definiálása: ________

matrix::matrix()
    : row(0), col(0), matSize(0), matArr(nullptr) {}

matrix::matrix(size_t r, size_t c)
    : row(r), col(c), matSize(r*c), matArr(new int[r*c]) {}

/*matrix::matrix(size_t r, size_t c, const int *arr)
    : row(r), col(c), matSize(r*c), matArr(new int[r*c])
{
    for (size_t i = 0; i < r*c; ++i)
    {
        matArr[i] = arr[i];
    }
}*/

///_______________________________________

matrix::~matrix() noexcept
{
    delete[] matArr;
}

matrix::matrix(const matrix &m)
{
    if (m.matArr != nullptr)
    {
        row = m.row;
        col = m.col;
        matSize = m.matSize;
        matArr = new int[matSize];

        for (size_t i = 0; i < matSize; ++i)
        {
            matArr[i] = m.matArr[i];
        }
    }
    else
    {
        row = 0;
        col = 0;
        matSize = 0;
        matArr = nullptr;
        //throw InvalidCopyConsParameter();
    }
}
                                            /// matrix1(m);
matrix &matrix::operator=(const matrix &m)  /// matrix1 = matrix2
{
    if (this != &m)
    {
        row = 0;
        col = 0;
        matSize = 0;
        delete[] matArr;
        matArr = nullptr;

        if (m.matArr != nullptr)
        {
            row = m.row;
            col = m.col;
            matSize = m.matSize;
            matArr = new int[matSize];

            for (size_t i = 0; i < matSize; ++i)
            {
                matArr[i] = m.matArr[i];
            }
        }
    }

    return *this;
}
                                                /// matrix mat3 = mat1 + mat2
matrix &matrix::operator=(matrix &&m) noexcept  /// matrix3 = matrix1 + matrix2
{
    if (this != &m)
    {
        row = 0;
        col = 0;
        matSize = 0;
        delete[] matArr;
        matArr = nullptr;

        if (m.matArr != nullptr)
        {
            row = m.row;
            col = m.col;
            matSize = m.matSize;

            /*for (size_t i = 0; i < matSize; ++i)
            {
                matArr[i] = m.matArr[i];
            }*/

            matArr = m.matArr;
        }
    }

    m.matArr = nullptr;

    return *this;
}

matrix::matrix(matrix &&m) noexcept
{
    row = m.row;
    col = m.col;
    matSize = m.matSize;
    matArr = m.matArr;

    m.matArr = nullptr;
}

matrix::matrix(const std::filesystem::path &p) : row(0), col(0)
{
    std::ifstream f(p); /// ../small.txt
    f >> row;
    f >> col;
    matSize = row * col;
    matArr = new int[matSize];

    // Tömb feltöltése oszlopfolytonosan
    for (size_t i = 0; i < row; ++i)
    {
        for (size_t j = 0; j < col; ++j)
        {
            f >> matArr[i + j * row];
        }
    }

    f.close();
}

matrix matrix::operator+(const matrix &m) const noexcept  /// matrix3 = matrix1 + matrix2
{
    matrix tmp(m.row, m.col);

    for (size_t i = 0; i < m.matSize; ++i)
    {
        tmp.matArr[i] = matArr[i] + m.matArr[i];
    }

    return tmp;
}

matrix matrix::operator*(const matrix &m) const noexcept  /// mat3 = mat1 * mat2
{
    matrix mTmp(row, m.col);
    int iTmp = 0;
    int mTmpIdx = 0;

    for (size_t m_col_step = 0; m_col_step < m.col; ++m_col_step)
    {
        for (size_t matArr_row_step = 0; matArr_row_step < row; ++matArr_row_step)
        {
            // col == m.row
            for (size_t j = 0; j < col; ++j)
            {
                iTmp += matArr[matArr_row_step + j * row] * m.matArr[m_col_step * m.row + j];
            }

            mTmp.matArr[mTmpIdx] = iTmp;
            iTmp = 0;
            mTmpIdx++;
        }
    }

    return mTmp;
}

int &matrix::at(size_t r, size_t c)
{
    if (r >= row || c >= col)
    {
        throw std::out_of_range("Index out of bounds");
    }

    // Visszaadom a mátrixom n. sor, m. oszlop elemét
    return matArr[r + c * row];
}

const int &matrix::at(size_t r, size_t c) const
{
    if (r >= row || c >= col)
    {
        throw std::out_of_range("Index out of bounds");
    }

    return matArr[r + c * row];
}

std::pair<size_t, size_t> matrix::size() const noexcept
{
    return std::make_pair(row, col);
}

/// Ezt lehet jobban is megírni !!! (Talán)
void matrix::transpose() noexcept
{
    int *tmpArr = new int[matSize];

    for (size_t i = 0; i < matSize; ++i)
    {
        tmpArr[i] = matArr[i];
    }

    for (size_t i = 0; i < row; ++i)
    {
        for (size_t j = 0; j < col; ++j)
        {
            matArr[i * col + j] = tmpArr[i + j * row];
        }
    }

    size_t tmp = row;
    row = col;
    col = tmp;

    delete[] tmpArr;
}

void matrix::print() const noexcept
{
    // Mátrix kiíratás
    for (size_t i = 0; i < row; ++i)
    {
        for (size_t j = 0; j < col; ++j)
        {
            std::cout << matArr[i + j * row] << ", ";
        }
        std::cout << "\n";
    }
}

#endif // ADATALG_KHF01_MATRIX_HPP
