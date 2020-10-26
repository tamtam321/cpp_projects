// your solution comes here
#include "matrix.hpp"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "woodpecker.hpp"
#include <iostream>

//matrix mat("small.txt");

TEST("matrix size", 1) {
  // check if size function gives back the correct result
  {
    matrix mat("small.txt");
    auto [n, m] = mat.size();
    CHECK_EQ(3, n);
    CHECK_EQ(4, m);
  }
  // size must be const
  {
    const matrix mat("small2.txt");
    auto [n, m] = mat.size();
    CHECK_EQ(4, n);
    CHECK_EQ(3, m);
  }
}

TEST("matrix at", 1) {
  // check if at function gives back the correct result
  {
    matrix mat("small.txt");
    auto [n, m] = mat.size();
    CHECK_EQ(3, n);
    CHECK_EQ(4, m);
    int idx = 0;
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < m; ++j) {
        idx++;
        CHECK_EQ(idx, mat.at(i, j));
      }
    }
    mat.at(0, 0) = 1;
    mat.at(2, 3) = 1;
    CHECK_EQ(1, mat.at(0, 0));
    CHECK_EQ(1, mat.at(2, 3));
  }
  // check const version of at
  {
    const matrix mat("small.txt");
    auto [n, m] = mat.size();
    CHECK_EQ(3, n);
    CHECK_EQ(4, m);
    int idx = 0;
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < m; ++j) {
        idx++;
        CHECK_EQ(idx, mat.at(i, j));
      }
    }
  }
}

TEST("matrix transpose", 1) {
  const matrix mat("small.txt");
  matrix mat2("small.txt");
  auto [n, m] = mat.size();
  CHECK_EQ(3, n);
  CHECK_EQ(4, m);
  mat2.transpose();
  auto [n2, m2] = mat2.size();
  CHECK_EQ(4, n2);
  CHECK_EQ(3, m2);
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      CHECK_EQ(mat2.at(j, i), mat.at(i, j));
    }
  }
}

TEST("addition", 1) {
  const matrix mat("small.txt"), mat2("small.txt");
  auto [n, m] = mat.size();
  CHECK_EQ(3, n);
  CHECK_EQ(4, m);
  const matrix res = mat + mat2;
  int idx = 0;
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      idx++;
      CHECK_EQ(2 * idx, res.at(i, j));
    }
  }
}

TEST("multiplication", 1) {
  const matrix mat("small.txt"), mat2("small2.txt");
  auto [n, m] = mat.size();
  CHECK_EQ(3, n);
  CHECK_EQ(4, m);
  auto [n2, m2] = mat2.size();
  CHECK_EQ(4, n2);
  CHECK_EQ(3, m2);
  {
    const matrix res = mat * mat2;
    int ref3x3[] = {70, 80, 90, 158, 184, 210, 246, 288, 330};
    std::pair<size_t, size_t> size = res.size();
    CHECK_EQ(n, size.first);
    CHECK_EQ(m2, size.second);
    int idx = 0;
    for (size_t i = 0; i < size.first; ++i) {
      for (size_t j = 0; j < size.second; ++j) {
        CHECK_EQ(ref3x3[idx], res.at(i, j));
        idx++;
      }
    }
  }

  {
    const matrix res = mat2 * mat;
    int ref4x4[] = {38,  44,  50,  56,  83,  98,  113, 128,
                    128, 152, 176, 200, 173, 206, 239, 272};
    std::pair<size_t, size_t> size = res.size();
    CHECK_EQ(n2, size.first);
    CHECK_EQ(m, size.second);
    int idx = 0;
    for (size_t i = 0; i < size.first; ++i) {
      for (size_t j = 0; j < size.second; ++j) {
        CHECK_EQ(ref4x4[idx], res.at(i, j));
        idx++;
      }
    }
  }
}

TEST("at with invalid indices", 1) {
  {
    const matrix mat("small.txt");
    auto [n, m] = mat.size();
    CHECK_EQ(3, n);
    CHECK_EQ(4, m);
    for (size_t i = 0; i < m; ++i) {
      CHECK_EXC(std::out_of_range, mat.at(n, i));
    }
    for (size_t i = 0; i < n; ++i) {
      CHECK_EXC(std::out_of_range, mat.at(i, m));
    }
    CHECK_EXC(std::out_of_range, mat.at(5, 90));
  }

  {
    matrix mat("small.txt");
    auto [n, m] = mat.size();
    CHECK_EQ(3, n);
    CHECK_EQ(4, m);
    for (size_t i = 0; i < m; ++i) {
      CHECK_EXC(std::out_of_range, mat.at(n, i));
    }
    for (size_t i = 0; i < n; ++i) {
      CHECK_EXC(std::out_of_range, mat.at(i, m));
    }
    CHECK_EXC(std::out_of_range, mat.at(5, 90));
  }
}

TEST("Copy constructor", 1) {
  {
    const matrix mat("small.txt");
    auto [n, m] = mat.size();
    CHECK_EQ(3, n);
    CHECK_EQ(4, m);
    matrix mat2(mat);
    std::pair<size_t, size_t> mat2_size = mat2.size();
    CHECK_EQ(n, mat2_size.first);
    CHECK_EQ(m, mat2_size.second);
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < m; ++j) {
        CHECK_EQ(mat.at(i, j), mat2.at(i, j));
      }
    }
  }
}

TEST("Copy assignment operator", 1) {
  const matrix mat("small.txt");
  auto [n, m] = mat.size();
  CHECK_EQ(3, n);
  CHECK_EQ(4, m);
  matrix mat2("small2.txt");
  std::pair<size_t, size_t> mat2_size = mat2.size();
  CHECK_EQ(4, mat2_size.first);
  CHECK_EQ(3, mat2_size.second);
  mat2 = mat;
  mat2_size = mat2.size();
  CHECK_EQ(n, mat2_size.first);
  CHECK_EQ(m, mat2_size.second);
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      CHECK_EQ(mat.at(i, j), mat2.at(i, j));
    }
  }
}

TEST("Self assignment", 1) {
  matrix mat("small.txt");
  auto [n, m] = mat.size();
  CHECK_EQ(3, n);
  CHECK_EQ(4, m);
  const matrix *matp = &mat;
  mat = *matp;
  std::pair<size_t, size_t> mat_size = mat.size();
  CHECK_EQ(n, mat_size.first);
  CHECK_EQ(m, mat_size.second);
}

WOODPECKER_TEST_MAIN(-1, -1)
