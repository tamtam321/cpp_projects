#include "interval_tree.hpp"

#include "woodpecker.hpp"

TEST("insert", 5) {
  interval_tree tree;
  // std::pair<int, int>/et lehet inicializalni 2 integerrel igy:
  // std::pair<int, int>{1, 2} de pl fgv hivasnal a parameterbe kapott pair-t
  // siman lehet igy is inicializalni: {1, 2}. Innentol igy fogom irni mert
  // rovidebb.
  tree.insert({1, 2}, "start");
  tree.insert({1, 3}, "start2"); // uaz a kezdo mas a vegpont
  tree.insert({2, 3}, "start");  // label nem szamit
  tree.insert({3, 4}, "b");
  tree.insert({4, 5}, "c");
  CHECK_EQ(5, tree.size());
}

TEST("insert duplicates", 5) {
  interval_tree tree;
  tree.insert({1, 2}, "start");
  tree.insert({1, 3}, "start2"); // uaz a kezdo mas a vegpont
  tree.insert({2, 3}, "start");  // label nem szamit
  tree.insert({3, 4}, "b");
  tree.insert({4, 5}, "c");
  CHECK_EQ(5, tree.size());
  tree.insert({1, 2},
              "yolo"); // {1, 2} mar benne van az a kulcs nem tortenik valtozas
  CHECK_EQ(5, tree.size());
}

TEST("remove", 5) {
  interval_tree tree;
  tree.insert({1, 2}, "start");
  tree.insert({2, 3}, "e");
  tree.insert({3, 4}, "b");
  tree.insert({4, 5}, "c");
  CHECK_EQ(4, tree.size());
  tree.remove({4, 5});
  CHECK_EQ(3, tree.size());
  tree.remove({3, 4});
  CHECK_EQ(2, tree.size());
  tree.remove({2, 3});
  CHECK_EQ(1, tree.size());
  tree.remove({1, 2});
  CHECK_EQ(0, tree.size());
}

TEST("remove duplicates", 5) {
  interval_tree tree;
  tree.insert({1, 2}, "start");
  tree.insert({2, 3}, "e");
  tree.insert({3, 4}, "b");
  tree.insert({4, 5}, "c");
  CHECK_EQ(4, tree.size());
  tree.remove({4, 5});
  CHECK_EQ(3, tree.size());
  tree.remove({4, 5}); // nincs a faban nem valtozik a meret
  tree.remove({5, 9}); // nincs a faban nem valtozik a meret
  tree.remove({1, 3}); // nincs a faban nem valtozik a meret
  CHECK_EQ(3, tree.size());
  tree.remove({3, 4});
  CHECK_EQ(2, tree.size());
  tree.remove({2, 3});
  CHECK_EQ(1, tree.size());
  tree.remove({1, 2});
  CHECK_EQ(0, tree.size());
}

// RB tree orai kod tesztekkent
TEST("rebalance_after_insert, 1. eset", 1) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {{10, 20}, {5, 7}, {11, 11},
                                             {1, 9},   {7, 8}, {0, 1}};
  for (auto e : elemek) {
    bal.insert(e, std::to_string(e.first));
    jobb.insert({-e.first, e.second}, std::to_string(-e.first));
  }
  CHECK_NOEXC(bal.validate());
  CHECK_NOEXC(jobb.validate());
}

TEST("rebalance_after_insert, 3. eset", 1) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {
      {10, 20}, {12, 12}, {5, 7}, {1, 9}, {0, 1}};
  for (auto e : elemek) {
    bal.insert(e, std::to_string(e.first));
    jobb.insert({-e.first, e.second}, std::to_string(e.first));
  }
  CHECK_NOEXC(bal.validate());
  CHECK_NOEXC(jobb.validate());
}
TEST("rebalance_after_insert, 2. eset", 1) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {{10, 20}, {12, 12}, {5, 7},
                                             {1, 9},   {2, 3},   {2, 4}};
  for (auto e : elemek) {
    bal.insert(e, "s");
    jobb.insert({-e.first, e.second}, "ee");
  }
  CHECK_NOEXC(bal.validate());
  CHECK_NOEXC(jobb.validate());
}

TEST("rebalance_after_remove, 4. eset", 1) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {{1, 2},  {2, 3},  {3, 4},  {4, 5},
                                             {5, 6},  {6, 49}, {7, 54}, {8, 42},
                                             {9, 42}, {10, 10}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, "s");
    jobb.insert({-e.first, e.second}, "s");
  }
  bal.remove({7, 54});
  jobb.remove({-7, 54});
  CHECK_NOEXC(bal.validate());
  CHECK_NOEXC(jobb.validate());
}

TEST("rebalance_after_remove, 3. eset", 1) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {
      {1, 2},  {2, 3},  {3, 4},  {4, 5},   {5, 6},
      {6, 49}, {7, 54}, {8, 42}, {10, 10}, {9, 42}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, "s");
    jobb.insert({-e.first, e.second}, "s");
  }
  bal.remove({7, 54});
  jobb.remove({-7, 54});
  CHECK_NOEXC(bal.validate());
  CHECK_NOEXC(jobb.validate());
}

TEST("rebalance_after_remove, 1. eset", 0) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {{1, 2},  {2, 3},  {3, 4},  {4, 5},
                                             {5, 6},  {6, 49}, {7, 54}, {8, 42},
                                             {9, 42}, {10, 10}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, "s");
    jobb.insert({-e.first, e.second}, "s");
  }
  bal.remove({5, 6});
  jobb.remove({-5, 6});
  CHECK_NOEXC(bal.validate());
  CHECK_NOEXC(jobb.validate());
}
// RB tree orai kod tesztek: max
TEST("max update: rebalance_after_insert, 1. eset", 3) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {{10, 20}, {5, 7}, {11, 11},
                                             {1, 9},   {7, 8}, {0, 1}};
  for (auto e : elemek) {
    bal.insert(e, std::to_string(e.first));
    jobb.insert({-e.first, e.second}, std::to_string(-e.first));
  }
  CHECK_EQ(1, bal.max({0, 1}));
  CHECK_EQ(9, bal.max({1, 9}));
  CHECK_EQ(9, bal.max({5, 7}));
  CHECK_EQ(8, bal.max({7, 8}));
  CHECK_EQ(20, bal.max({10, 20}));
  CHECK_EQ(11, bal.max({11, 11}));
  CHECK_EQ(1, jobb.max({0, 1}));
  CHECK_EQ(9, jobb.max({-1, 9}));
  CHECK_EQ(9, jobb.max({-5, 7}));
  CHECK_EQ(8, jobb.max({-7, 8}));
  CHECK_EQ(20, jobb.max({-10, 20}));
  CHECK_EQ(11, jobb.max({-11, 11}));
}

TEST("max update: rebalance_after_insert, 3. eset", 3) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {
      {10, 20}, {12, 12}, {5, 7}, {1, 9}, {0, 1}};
  for (auto e : elemek) {
    bal.insert(e, std::to_string(e.first));
    jobb.insert({-e.first, e.second}, std::to_string(e.first));
  }
  CHECK_EQ(1, bal.max({0, 1}));
  CHECK_EQ(9, bal.max({1, 9}));
  CHECK_EQ(7, bal.max({5, 7}));
  CHECK_EQ(20, bal.max({10, 20}));
  CHECK_EQ(12, bal.max({12, 12}));
  CHECK_EQ(1, jobb.max({0, 1}));
  CHECK_EQ(9, jobb.max({-1, 9}));
  CHECK_EQ(7, jobb.max({-5, 7}));
  CHECK_EQ(20, jobb.max({-10, 20}));
  CHECK_EQ(12, jobb.max({-12, 12}));
}
TEST("max update: rebalance_after_insert, 2. eset", 3) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {{10, 20}, {12, 12}, {5, 7},
                                             {1, 9},   {2, 3},   {2, 4}};
  for (auto e : elemek) {
    bal.insert(e, "s");
    jobb.insert({-e.first, e.second}, "ee");
  }
  CHECK_EQ(9, bal.max({1, 9}));
  CHECK_EQ(9, bal.max({2, 3}));
  CHECK_EQ(4, bal.max({2, 4}));
  CHECK_EQ(7, bal.max({5, 7}));
  CHECK_EQ(20, bal.max({10, 20}));
  CHECK_EQ(12, bal.max({12, 12}));
  CHECK_EQ(9, jobb.max({-1, 9}));
  CHECK_EQ(9, jobb.max({-2, 3}));
  CHECK_EQ(4, jobb.max({-2, 4}));
  CHECK_EQ(7, jobb.max({-5, 7}));
  CHECK_EQ(20, jobb.max({-10, 20}));
  CHECK_EQ(12, jobb.max({-12, 12}));
}

TEST("max update: rebalance_after_remove, 4. eset", 3) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {{1, 2},  {2, 3},  {3, 4},  {4, 5},
                                             {5, 6},  {6, 49}, {7, 54}, {8, 43},
                                             {9, 42}, {10, 10}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, "s");
    jobb.insert({-e.first, e.second}, "s");
  }
  CHECK_EQ(2, bal.max({1, 2}));
  CHECK_EQ(4, bal.max({2, 3}));
  CHECK_EQ(4, bal.max({3, 4}));
  CHECK_EQ(54, bal.max({4, 5}));
  CHECK_EQ(6, bal.max({5, 6}));
  CHECK_EQ(54, bal.max({6, 49}));
  CHECK_EQ(54, bal.max({7, 54}));
  CHECK_EQ(54, bal.max({8, 43}));
  CHECK_EQ(42, bal.max({9, 42}));
  CHECK_EQ(10, bal.max({10, 10}));
  CHECK_EQ(2, jobb.max({-1, 2}));
  CHECK_EQ(4, jobb.max({-2, 3}));
  CHECK_EQ(4, jobb.max({-3, 4}));
  CHECK_EQ(54, jobb.max({-4, 5}));
  CHECK_EQ(6, jobb.max({-5, 6}));
  CHECK_EQ(54, jobb.max({-6, 49}));
  CHECK_EQ(54, jobb.max({-7, 54}));
  CHECK_EQ(54, jobb.max({-8, 43}));
  CHECK_EQ(42, jobb.max({-9, 42}));
  CHECK_EQ(10, jobb.max({-10, 10}));
  bal.remove({7, 54});
  jobb.remove({-7, 54});
  CHECK_EQ(2, bal.max({1, 2}));
  CHECK_EQ(4, bal.max({2, 3}));
  CHECK_EQ(4, bal.max({3, 4}));
  CHECK_EQ(49, bal.max({4, 5}));
  CHECK_EQ(6, bal.max({5, 6}));
  CHECK_EQ(49, bal.max({6, 49}));
  CHECK_EQ(0, bal.max({7, 54}));
  CHECK_EQ(43, bal.max({8, 43}));
  CHECK_EQ(43, bal.max({9, 42}));
  CHECK_EQ(10, bal.max({10, 10}));
  CHECK_EQ(2, jobb.max({-1, 2}));
  CHECK_EQ(4, jobb.max({-2, 3}));
  CHECK_EQ(4, jobb.max({-3, 4}));
  CHECK_EQ(49, jobb.max({-4, 5}));
  CHECK_EQ(6, jobb.max({-5, 6}));
  CHECK_EQ(49, jobb.max({-6, 49}));
  CHECK_EQ(0, jobb.max({-7, 54}));
  CHECK_EQ(43, jobb.max({-8, 43}));
  CHECK_EQ(43, jobb.max({-9, 42}));
  CHECK_EQ(10, jobb.max({-10, 10}));
}

TEST("max update: rebalance_after_remove, 3. eset", 4) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {
      {1, 2},  {2, 3},  {3, 4},  {4, 5},   {5, 6},
      {6, 49}, {7, 54}, {8, 43}, {10, 10}, {9, 42}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, "s");
    jobb.insert({-e.first, e.second}, "s");
  }
  CHECK_EQ(2, bal.max({1, 2}));
  CHECK_EQ(4, bal.max({2, 3}));
  CHECK_EQ(4, bal.max({3, 4}));
  CHECK_EQ(54, bal.max({4, 5}));
  CHECK_EQ(6, bal.max({5, 6}));
  CHECK_EQ(54, bal.max({6, 49}));
  CHECK_EQ(54, bal.max({7, 54}));
  CHECK_EQ(54, bal.max({8, 43}));
  CHECK_EQ(42, bal.max({9, 42}));
  CHECK_EQ(42, bal.max({10, 10}));
  CHECK_EQ(2, jobb.max({-1, 2}));
  CHECK_EQ(4, jobb.max({-2, 3}));
  CHECK_EQ(4, jobb.max({-3, 4}));
  CHECK_EQ(54, jobb.max({-4, 5}));
  CHECK_EQ(6, jobb.max({-5, 6}));
  CHECK_EQ(54, jobb.max({-6, 49}));
  CHECK_EQ(54, jobb.max({-7, 54}));
  CHECK_EQ(54, jobb.max({-8, 43}));
  CHECK_EQ(42, jobb.max({-9, 42}));
  CHECK_EQ(42, jobb.max({-10, 10}));
  bal.remove({7, 54});
  jobb.remove({-7, 54});
  CHECK_EQ(2, bal.max({1, 2}));
  CHECK_EQ(4, bal.max({2, 3}));
  CHECK_EQ(4, bal.max({3, 4}));
  CHECK_EQ(49, bal.max({4, 5}));
  CHECK_EQ(6, bal.max({5, 6}));
  CHECK_EQ(49, bal.max({6, 49}));
  CHECK_EQ(0, bal.max({7, 54}));
  CHECK_EQ(43, bal.max({8, 43}));
  CHECK_EQ(43, bal.max({9, 42}));
  CHECK_EQ(10, bal.max({10, 10}));
  CHECK_EQ(2, jobb.max({-1, 2}));
  CHECK_EQ(4, jobb.max({-2, 3}));
  CHECK_EQ(4, jobb.max({-3, 4}));
  CHECK_EQ(49, jobb.max({-4, 5}));
  CHECK_EQ(6, jobb.max({-5, 6}));
  CHECK_EQ(49, jobb.max({-6, 49}));
  CHECK_EQ(0, jobb.max({-7, 54}));
  CHECK_EQ(43, jobb.max({-8, 43}));
  CHECK_EQ(43, jobb.max({-9, 42}));
  CHECK_EQ(10, jobb.max({-10, 10}));
}

TEST("max update: rebalance_after_remove, 1. eset", 4) {
  interval_tree bal, jobb;
  std::vector<std::pair<int, int>> elemek = {{1, 2},  {2, 3},  {3, 4},  {4, 5},
                                             {5, 6},  {6, 49}, {7, 54}, {8, 43},
                                             {9, 42}, {10, 10}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, "s");
    jobb.insert({-e.first, e.second}, "s");
  }
  CHECK_EQ(2, bal.max({1, 2}));
  CHECK_EQ(4, bal.max({2, 3}));
  CHECK_EQ(4, bal.max({3, 4}));
  CHECK_EQ(54, bal.max({4, 5}));
  CHECK_EQ(6, bal.max({5, 6}));
  CHECK_EQ(54, bal.max({6, 49}));
  CHECK_EQ(54, bal.max({7, 54}));
  CHECK_EQ(54, bal.max({8, 43}));
  CHECK_EQ(42, bal.max({9, 42}));
  CHECK_EQ(10, bal.max({10, 10}));
  CHECK_EQ(2, jobb.max({-1, 2}));
  CHECK_EQ(4, jobb.max({-2, 3}));
  CHECK_EQ(4, jobb.max({-3, 4}));
  CHECK_EQ(54, jobb.max({-4, 5}));
  CHECK_EQ(6, jobb.max({-5, 6}));
  CHECK_EQ(54, jobb.max({-6, 49}));
  CHECK_EQ(54, jobb.max({-7, 54}));
  CHECK_EQ(54, jobb.max({-8, 43}));
  CHECK_EQ(42, jobb.max({-9, 42}));
  CHECK_EQ(10, jobb.max({-10, 10}));
  bal.remove({5, 6});
  jobb.remove({-5, 6});
  CHECK_EQ(2, bal.max({1, 2}));
  CHECK_EQ(4, bal.max({2, 3}));
  CHECK_EQ(4, bal.max({3, 4}));
  CHECK_EQ(54, bal.max({4, 5}));
  CHECK_EQ(0, bal.max({5, 6}));
  CHECK_EQ(54, bal.max({6, 49}));
  CHECK_EQ(54, bal.max({7, 54}));
  CHECK_EQ(54, bal.max({8, 43}));
  CHECK_EQ(42, bal.max({9, 42}));
  CHECK_EQ(10, bal.max({10, 10}));
  CHECK_EQ(2, jobb.max({-1, 2}));
  CHECK_EQ(4, jobb.max({-2, 3}));
  CHECK_EQ(4, jobb.max({-3, 4}));
  CHECK_EQ(54, jobb.max({-4, 5}));
  CHECK_EQ(0, jobb.max({-5, 6}));
  CHECK_EQ(54, jobb.max({-6, 49}));
  CHECK_EQ(54, jobb.max({-7, 54}));
  CHECK_EQ(54, jobb.max({-8, 43}));
  CHECK_EQ(42, jobb.max({-9, 42}));
  CHECK_EQ(10, jobb.max({-10, 10}));
}
// RB tree orai tesztek vege

TEST("find", 5) {
  interval_tree tree;
  tree.insert({1, 2}, "start");
  tree.insert({2, 3}, "e");
  tree.insert({3, 4}, "b");
  tree.insert({4, 5}, "c");
  CHECK_EQ(4, tree.size());
  // itt ellenorzom a visszateresi tipust, innentol auto-t fogok hasznalni
  std::pair<const std::pair<int, int>, std::string> *value = tree.find({1, 2});
  CHECK_EQ(1, value->first.first);
  CHECK_EQ(2, value->first.second);
  CHECK_EQ("start", value->second);
  value = tree.find({2, 3});
  CHECK_EQ(2, value->first.first);
  CHECK_EQ(3, value->first.second);
  CHECK_EQ("e", value->second);
  value = tree.find({3, 4});
  CHECK_EQ(3, value->first.first);
  CHECK_EQ(4, value->first.second);
  CHECK_EQ("b", value->second);
  value = tree.find({4, 5});
  CHECK_EQ(4, value->first.first);
  CHECK_EQ(5, value->first.second);
  CHECK_EQ("c", value->second);
  // nem letezo csucs -> visszateres nullptr
  CHECK_EQ(nullptr, tree.find({1, 3}));
  CHECK_EQ(nullptr, tree.find({7, 8}));
  CHECK_EQ(nullptr, tree.find({1, 5}));
  tree.remove({4, 5});
  CHECK_EQ(nullptr, tree.find({4, 5}));
}

TEST("find with modification", 5) {
  interval_tree tree;
  tree.insert({1, 2}, "start");
  tree.insert({2, 3}, "e");
  tree.insert({3, 4}, "b");
  tree.insert({4, 5}, "c");
  CHECK_EQ(4, tree.size());
  auto value = tree.find({1, 2});
  CHECK_EQ(1, value->first.first);
  CHECK_EQ(2, value->first.second);
  CHECK_EQ("start", value->second);
  value->second = "modified start";
  value = tree.find({2, 3});
  CHECK_EQ(2, value->first.first);
  CHECK_EQ(3, value->first.second);
  CHECK_EQ("e", value->second);
  value = tree.find({1, 2});
  CHECK_EQ(1, value->first.first);
  CHECK_EQ(2, value->first.second);
  CHECK_EQ("modified start", value->second);
}

TEST("find const overload", 5) {
  interval_tree tree;
  tree.insert({1, 2}, "start");
  tree.insert({2, 3}, "e");
  tree.insert({3, 4}, "b");
  tree.insert({4, 5}, "c");
  CHECK_EQ(4, tree.size());
  // const interval_tree-re mutat a ptr, tehat csak konstans muveleteket lehet
  // meghivni ra.
  const interval_tree *tree_ptr = &tree;
  // itt ellenorzom a visszateresi tipust, innentol auto-t fogok hasznalni
  const std::pair<const std::pair<int, int>, std::string> *value =
      tree_ptr->find({1, 2}); // itt a const overload fog meghivodni
  CHECK_EQ(1, value->first.first);
  CHECK_EQ(2, value->first.second);
  CHECK_EQ("start", value->second);
  value = tree_ptr->find({2, 3});
  CHECK_EQ(2, value->first.first);
  CHECK_EQ(3, value->first.second);
  CHECK_EQ("e", value->second);
  value = tree_ptr->find({3, 4});
  CHECK_EQ(3, value->first.first);
  CHECK_EQ(4, value->first.second);
  CHECK_EQ("b", value->second);
  value = tree_ptr->find({4, 5});
  CHECK_EQ(4, value->first.first);
  CHECK_EQ(5, value->first.second);
  CHECK_EQ("c", value->second);
  // nem letezo csucs -> visszateres nullptr
  CHECK_EQ(nullptr, tree_ptr->find({1, 3}));
  CHECK_EQ(nullptr, tree_ptr->find({7, 8}));
  CHECK_EQ(nullptr, tree_ptr->find({1, 5}));
}

TEST("interval search", 20) {
  interval_tree tree;
  tree.insert({1, 2}, "start");
  tree.insert({3, 3}, "e");
  tree.insert({4, 5}, "b");
  tree.insert({5, 6}, "c");
  CHECK_EQ(4, tree.size());
  std::pair<const std::pair<int, int>, std::string> *value =
      tree.interval_search({1, 2});
  CHECK_EQ(1, value->first.first);
  CHECK_EQ(2, value->first.second);
  CHECK_EQ("start", value->second);
  value = tree.interval_search({0, 2});
  CHECK_EQ(1, value->first.first);
  CHECK_EQ(2, value->first.second);
  CHECK_EQ("start", value->second);
  value = tree.interval_search({4, 9});
  CHECK_EQ(4, value->first.first);
  CHECK_EQ(5, value->first.second);
  CHECK_EQ("b", value->second);
  // one point match:
  value = tree.interval_search({0, 1});
  CHECK_EQ(1, value->first.first);
  CHECK_EQ(2, value->first.second);
  CHECK_EQ("start", value->second);
  value = tree.interval_search({5, 9});
  CHECK_EQ(4, value->first.first);
  CHECK_EQ(5, value->first.second);
  CHECK_EQ("b", value->second);
}

// Iterator API

TEST("iterator, iterate through tree", 5) {
  interval_tree bal;
  std::vector<std::pair<int, int>> elemek = {{1, 2},  {1, 3},  {3, 4},  {4, 5},
                                             {5, 6},  {6, 49}, {7, 54}, {8, 42},
                                             {9, 42}, {10, 10}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, std::to_string(e.first));
  }
  // az iteratorokkal novekvo sorrendben kapjuk vissza az ertekeket.
  // ez most pont megegyezik a beszurasi sorrenddel
  // ami kell: dereference operator(*), ++, !=, begin(), end()
  // vegtelen ciklus: end: nullptr vs empty leaf
  int i = 0;
  for (interval_tree::iterator it = bal.begin(); it != bal.end(); ++it) {
    CHECK_EQ(elemek[i].first, (*it).first.first);
    CHECK_EQ(elemek[i].second, (*it).first.second);
    CHECK_EQ(std::to_string(elemek[i].first), (*it).second);
    i++;
  }
  // ugyanaz range based forral
  i = 0;
  for (std::pair<const std::pair<int, int>, std::string> &val : bal) {
    CHECK_EQ(elemek[i].first, val.first.first);
    CHECK_EQ(elemek[i].second, val.first.second);
    CHECK_EQ(std::to_string(elemek[i].first), val.second);
    i++;
  }
}

TEST("iterator, structure dereference operator", 5) {
  interval_tree bal;
  std::vector<std::pair<int, int>> elemek = {{1, 2},  {1, 3},  {3, 4},  {4, 5},
                                             {5, 6},  {6, 49}, {7, 54}, {8, 42},
                                             {9, 42}, {10, 10}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, std::to_string(e.first));
  }
  // az iteratorokkal novekvo sorrendben kapjuk vissza az ertekeket.
  // ez most pont megegyezik a beszurasi sorrenddel
  // ami kell: ami az elozoben es a -> operator
  int i = 0;
  for (interval_tree::iterator it = bal.begin(); it != bal.end(); ++it) {
    CHECK_EQ(elemek[i].first, it->first.first);
    CHECK_EQ(elemek[i].second, it->first.second);
    CHECK_EQ(std::to_string(elemek[i].first), it->second);
    i++;
  }
}

TEST("iterator, find", 5) {
  interval_tree bal;
  std::vector<std::pair<int, int>> elemek = {{1, 2},  {1, 3},  {3, 4},  {4, 5},
                                             {5, 6},  {6, 49}, {7, 54}, {8, 42},
                                             {9, 42}, {10, 10}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, std::to_string(e.first));
  }
  interval_tree::iterator it = bal.find_i({1, 2});
  CHECK_EQ(1, it->first.first);
  CHECK_EQ(2, it->first.second);
  CHECK_EQ("1", it->second);
  it = bal.find_i({99, 99});
  CHECK_EQ(true, bal.end() == it);
  it = bal.find_i({1, 4});
  CHECK_EQ(true, bal.end() ==
                     it); // ha nincs benne akkor az end iteratorral ter vissza
  it = bal.find_i({10, 10});
  CHECK_EQ(10, it->first.first);
  CHECK_EQ(10, it->first.second);
  CHECK_EQ("10", it->second);
  it->second = "utolso";
  it = bal.find_i({10, 10});
  CHECK_EQ(10, it->first.first);
  CHECK_EQ(10, it->first.second);
  CHECK_EQ("utolso", it->second);
}

TEST("iterator, interval_search", 5) {
  interval_tree bal;
  std::vector<std::pair<int, int>> elemek = {{1, 2},  {1, 3},  {3, 4},  {4, 5},
                                             {5, 6},  {6, 49}, {7, 54}, {8, 42},
                                             {9, 42}, {10, 10}};
  for (std::pair<int, int> e : elemek) {
    bal.insert(e, std::to_string(e.first));
  }
  interval_tree::iterator it = bal.interval_search_i({1, 2});
  CHECK_EQ(1, it->first.first);
  CHECK_EQ("1", it->second);
  it = bal.interval_search_i({99, 99});
  CHECK_EQ(true, bal.end() == it);
  it = bal.interval_search_i({1, 4});
  CHECK_EQ(false, bal.end() == it);
  it = bal.interval_search_i({10, 10});
  CHECK_EQ(false, bal.end() == it);
  it->second = "valami";
  it = bal.interval_search_i({10, 10});
  CHECK_EQ(false, bal.end() == it);
  CHECK_EQ("valami", it->second);
}

WOODPECKER_MAIN
