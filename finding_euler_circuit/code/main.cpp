// your solution comes here
#include "plan.hpp"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "woodpecker.hpp"
#include <algorithm>
#include <array>
#include <memory>
#include <random>
#include <set>
#include <utility>

bool check(std::vector<Corridor> &&corridors_,
           const std::vector<std::int32_t> &result) {
  std::vector<Corridor> corridors{std::move(corridors_)};
  const size_t M = corridors.size();
  corridors.reserve(2 * M);
  for (size_t i = 0; i < M; ++i) {
    auto [from, to] = corridors[i];
    corridors.push_back(Corridor{to, from});
  }
  std::vector<Corridor> result_corridors{};
  result_corridors.reserve(2 * M);
  for (size_t i = 1; i < result.size(); ++i) {
    result_corridors.push_back(Corridor{result[i - 1], result[i]});
  }
  auto comp = [](Corridor a, Corridor b) {
    return a.from == b.from ? a.to < b.to : a.from < b.from;
  };
  std::sort(corridors.begin(), corridors.end(), comp);
  std::sort(result_corridors.begin(), result_corridors.end(), comp);
  auto eq = [](Corridor a, Corridor b) {
    return a.from == b.from && a.to == b.to;
  };
  return std::equal(corridors.begin(), corridors.end(),
                    result_corridors.begin(), result_corridors.end(), eq);
}

TEST("small line", 1) {
  constexpr size_t N = 3;
  std::vector<Corridor> corridors{{1, 2}, {2, 3}};
  const std::vector<std::int32_t> result =
      plan(corridors.begin(), corridors.end(), N);
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("longer line", 1) {
  constexpr size_t N = 5;
  std::vector<Corridor> corridors{{1, 2}, {3, 4}, {2, 3}, {5, 4}};
  const std::vector<std::int32_t> result =
      plan(corridors.begin(), corridors.end(), N);
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("star", 1) {
  constexpr size_t N = 6;
  std::vector<Corridor> corridors{{1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}};
  const std::vector<std::int32_t> result =
      plan(corridors.begin(), corridors.end(), N);
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("tree", 1) {
  constexpr size_t N = 11;
  std::vector<Corridor> corridors{{1, 2}, {1, 3}, {1, 4}, {1, 5},  {3, 6},
                                  {3, 7}, {4, 8}, {4, 9}, {5, 10}, {5, 11}};
  const std::vector<std::int32_t> result =
      plan(corridors.begin(), corridors.end(), N);
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("small cycle", 1) {
  constexpr size_t N = 6;
  std::vector<Corridor> corridors{{1, 2}, {2, 3}, {3, 4}, {5, 4}, {6, 5}};
  const std::vector<std::int32_t> result =
      plan(corridors.begin(), corridors.end(), N);
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("example in the text", 1) {
  constexpr size_t N = 5;
  std::vector<Corridor> corridors{{1, 2}, {2, 3}, {2, 4},
                                  {4, 3}, {4, 5}, {3, 5}};
  const std::vector<std::int32_t> result =
      plan(corridors.begin(), corridors.end(), N);
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("template test", 1) {
  constexpr size_t N = 5;
  const std::array<Corridor, 6> corridors{
      {{1, 2}, {2, 3}, {2, 4}, {4, 3}, {4, 5}, {3, 5}}};
  const std::vector<std::int32_t> result =
      plan(corridors.begin(), corridors.end(), N);
  CHECK_EQ(true,
           check(std::vector(corridors.begin(), corridors.end()), result));
}

TEST("small test", 1) {
  constexpr size_t N = 20;
  std::vector<Corridor> corridors{
      {8, 18},  {10, 11}, {4, 8},   {1, 6},   {1, 15},  {1, 3},   {1, 18},
      {17, 19}, {6, 20},  {8, 9},   {11, 15}, {2, 5},   {4, 5},   {4, 12},
      {10, 17}, {3, 16},  {3, 8},   {9, 14},  {15, 19}, {16, 20}, {11, 20},
      {11, 16}, {4, 11},  {8, 15},  {3, 13},  {14, 16}, {15, 20}, {1, 16},
      {13, 18}, {11, 17}, {18, 19}, {3, 9},   {4, 20},  {8, 10},  {1, 14},
      {5, 16},  {8, 12},  {12, 15}, {6, 15},  {2, 16},  {5, 6},   {11, 12},
      {16, 18}, {7, 8},   {19, 20}, {14, 17}, {5, 12},  {17, 20}, {1, 7},
      {7, 20},  {1, 5},   {5, 10},  {14, 20}, {2, 6},   {7, 14},  {9, 16},
      {3, 20},  {2, 19}};
  const std::vector<std::int32_t> result =
      plan(corridors.begin(), corridors.end(), N);
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("large star", 1) {
  constexpr size_t N = 1000000;
  std::vector<Corridor> corridors{};
  for (size_t i = 1; i < N; ++i) {
    corridors.push_back(Corridor{1, static_cast<std::int32_t>(i + 1)});
  }
  std::vector<std::int32_t> result{};
  MEASURE("plan", 5s) { result = plan(corridors.begin(), corridors.end(), N); }
  result.shrink_to_fit();
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("large chain", 1) {
  constexpr size_t N = 1000000;
  std::vector<Corridor> corridors{};
  for (size_t i = 0; i < N - 1; ++i) {
    corridors.push_back(Corridor{static_cast<std::int32_t>(i + 1),
                                 static_cast<std::int32_t>(i + 2)});
  }
  std::vector<std::int32_t> result{};
  MEASURE("plan", 5s) { result = plan(corridors.begin(), corridors.end(), N); }
  result.shrink_to_fit();
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("large ladder", 1) {
  constexpr size_t N = 1000000;
  std::vector<Corridor> corridors{};
  corridors.reserve(N * 15);
  std::minstd_rand gen(1);
  std::uniform_int_distribution<int> dist(3, 100);
  for (size_t i = 0; i < N - 1; ++i) {
    corridors.push_back(Corridor{static_cast<std::int32_t>(i + 1),
                                 static_cast<std::int32_t>(i + 2)});
    if (150 < i) {
      std::set<int> s{};
      for (int j = 0; j < 10; ++j) {
        int r;
        while (s.count(r = dist(gen)) != 0) {
        }
        s.insert(r);
        std::int32_t to = i - r;
        corridors.push_back(Corridor{static_cast<std::int32_t>(i + 1), to + 1});
      }
    }
  }
  std::vector<std::int32_t> result{};
  MEASURE("plan", 5s) { result = plan(corridors.begin(), corridors.end(), N); }
  result.shrink_to_fit();
  CHECK_EQ(true, check(std::move(corridors), result));
}

TEST("large random", 1) {
  constexpr size_t N = 5000;
  std::vector<Corridor> corridors{};
  std::minstd_rand gen(2);
  std::bernoulli_distribution dist(0.7);
  for (std::int32_t i = 0; i < static_cast<std::int32_t>(N) - 1; ++i) {
    for (std::int32_t j = i + 1; j < static_cast<std::int32_t>(N); ++j) {
      if (dist(gen)) {
        corridors.push_back(Corridor{i + 1, j + 1});
      }
    }
  }
  std::vector<std::int32_t> result{};
  CHECK_EQ(true, corridors.size() < 10000000);
  MEASURE("plan", 5s) { result = plan(corridors.begin(), corridors.end(), N); }
  CHECK_EQ(true, check(std::move(corridors), result));
}


/// My tests
/*#include <list>

TEST("my tests", 1)
{
    std::vector<Corridor> corridors{{1, 2}, {2, 3}};

    //std::cout <<  corridors.end() - corridors.begin() << std::endl;
    plan(corridors.begin(), corridors.end(), 3);

    const std::vector<std::int32_t> result = {1, 2, 3, 2, 1};
    CHECK_EQ(true, check(std::move(corridors), result));
}*/

WOODPECKER_TEST_MAIN(700 * 1024 * 1024, 8 * 1024 * 1024)