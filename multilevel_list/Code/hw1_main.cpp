// your solution comes here
#include "multi_lvl_list.hpp"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "woodpecker.hpp"
#include <chrono>
#include <random>
#include <set>

TEST("Create an empty list. Size should be 0.", 5) {
  multilevel_list ml;
  CHECK_EQ(0, ml.size());
}

TEST("Add a few elements. Size should grow.", 10) {
  multilevel_list ml;
  CHECK_EQ(0, ml.size());
  ml.insert(3);
  CHECK_EQ(1, ml.size());
  ml.insert(42);
  CHECK_EQ(2, ml.size());
  ml.insert(0);
  CHECK_EQ(3, ml.size());
  ml.insert(-100);
  CHECK_EQ(4, ml.size());
  ml.insert(6);
  CHECK_EQ(5, ml.size());
}

TEST("Add a few elements. Elements should be in the list.", 10) {
  multilevel_list ml;
  ml.insert(3);
  ml.insert(-23123);
  ml.insert(2222);
  CHECK_EQ(true, ml.contains(3));
  CHECK_EQ(true, ml.contains(-23123));
  CHECK_EQ(true, ml.contains(2222));
  CHECK_EQ(false, ml.contains(0));
  CHECK_EQ(false, ml.contains(1));
  CHECK_EQ(false, ml.contains(5765));
}

TEST("Try to add duplicate elements. Size should not grow.", 10) {
  multilevel_list ml;
  ml.insert(9);
  ml.insert(7);
  ml.insert(21);
  ml.insert(9);
  ml.insert(9);
  ml.insert(24);
  ml.insert(7);
  CHECK_EQ(4, ml.size());
  CHECK_EQ(true, ml.contains(9));
  CHECK_EQ(true, ml.contains(7));
  CHECK_EQ(true, ml.contains(21));
  CHECK_EQ(true, ml.contains(24));
}

TEST("Delete a few elements. Size should shrink.", 10) {
  multilevel_list ml;
  ml.insert(12);
  ml.insert(89);
  ml.insert(100);
  ml.insert(56);
  ml.insert(86);
  ml.insert(5);
  CHECK_EQ(6, ml.size());
  ml.erase(12);
  CHECK_EQ(5, ml.size());
  ml.erase(5);
  CHECK_EQ(4, ml.size());
  ml.erase(100);
  CHECK_EQ(3, ml.size());
  ml.erase(89);
  CHECK_EQ(2, ml.size());
}

TEST("Delete a few elements. They should disappear.", 10) {
  multilevel_list ml;
  ml.insert(12);
  ml.insert(89);
  ml.insert(100);
  ml.insert(56);
  ml.insert(86);
  ml.insert(5);
  ml.erase(12);
  ml.erase(5);
  ml.erase(100);
  ml.erase(89);
  CHECK_EQ(false, ml.contains(12));
  CHECK_EQ(false, ml.contains(5));
  CHECK_EQ(false, ml.contains(100));
  CHECK_EQ(false, ml.contains(89));
  CHECK_EQ(true, ml.contains(86));
  CHECK_EQ(true, ml.contains(56));
}

TEST("Delete non-existent elements. Size should stay the same.", 10) {
  multilevel_list ml;
  ml.insert(3);
  ml.insert(4);
  ml.insert(5);
  ml.insert(9);
  CHECK_EQ(4, ml.size());
  ml.erase(0);
  ml.erase(7);
  CHECK_EQ(4, ml.size());
  CHECK_EQ(true, ml.contains(3));
  CHECK_EQ(true, ml.contains(4));
  CHECK_EQ(true, ml.contains(5));
  CHECK_EQ(true, ml.contains(9));
}

TEST("Do a lot of random operations. Multilevel list should behave the same as "
     "a std::set",
     20) {
  std::minstd_rand gen(42);
  std::uniform_int_distribution dist(-100, 100);
  std::uniform_int_distribution small_dist(3);
  std::set<int> ref;
  multilevel_list ml;
  for (size_t i = 0; i < 10000; ++i) {
    const int rnd = dist(gen);
    switch (small_dist(gen)) {
    case 0:
      ref.insert(rnd);
      ml.insert(rnd);
      break;
    case 1: CHECK_EQ(ref.count(rnd) > 0, ml.contains(rnd)); break;
    case 2:
      ref.erase(rnd);
      ml.erase(rnd);
      break;
    }
    CHECK_EQ(ref.size(), ml.size());
  }
}

TEST("Build a 2^5 and a 2^10-large set. Operation times on the two should be "
     "within a factor of ~2",
     15) {
  std::minstd_rand gen(42);
  std::uniform_int_distribution dist;
  multilevel_list small, big;

  while (small.size() < 32) {
    small.insert(dist(gen));
  }
  while (big.size() < 1024) {
    big.insert(dist(gen));
  }
  std::array<int, 10> vals;
  std::generate_n(vals.begin(), 10, [&gen, &dist]() { return dist(gen); });
  TIMER(small_contains) {
    for (size_t i = 0; i < 10; ++i) {
      small.contains(vals[i]);
    }
  }
  TIMER(small_insert) {
    for (size_t i = 0; i < 10; ++i) {
      small.insert(vals[i]);
    }
  }
  TIMER(small_erase) {
    for (size_t i = 0; i < 10; ++i) {
      small.erase(vals[i]);
    }
  }
  MEASURE("Query big set", 2.3 * small_contains) {
    for (size_t i = 0; i < 10; ++i) {
      big.contains(vals[i]);
    }
  }
  MEASURE("Insert into big set", 2.3 * small_insert) {
    for (size_t i = 0; i < 10; ++i) {
      big.insert(vals[i]);
    }
  }
  MEASURE("Erase from big set", 2.3 * small_erase) {
    for (size_t i = 0; i < 10; ++i) {
      big.erase(vals[i]);
    }
  }
}

WOODPECKER_MAIN(-1, -1)

// MY TEST
/*
#include <ctime> // random numb generate,
#include <cstdlib> // random numb generate,

int main()
{
    srand(time(nullptr));
    int chance_lvl_up;

    for (size_t i = 0; i < 10; i ++)
    {
        chance_lvl_up = rand();
        std::cout << chance_lvl_up << std::endl;
    }

    return 0;
}*/
