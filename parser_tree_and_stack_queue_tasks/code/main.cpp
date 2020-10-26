// your solution comes here
#include "train.hpp"
#include "tree_parser.hpp"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "woodpecker_precpp17.hpp"
#include <random>
#include <thread>

// Task 1
// minden TEST macro 1-1 fuggveny definicio lesz innentol
TEST("Train: 1 car", 1) {
  // 1 elemu listak mindig rendezhetoek
  std::vector<short> input = {4};
  CHECK_EQ(true, is_orderable(input, true, false));
  CHECK_EQ(true, is_orderable(input, false, true));
}
TEST("Train: 2 cars, asc", 1) {
  // eleve rendezett lista mindig rendezheto
  std::vector<short> input = {3, 4};
  CHECK_EQ(true, is_orderable(input, true, false));
  CHECK_EQ(true, is_orderable(input, false, true));
}
TEST("Train: 1 cars, desc", 1) {
  // 2 elemu lista is mindig rendezheto
  std::vector<short> input = {4, 3};
  CHECK_EQ(true, is_orderable(input, true, false));
  CHECK_EQ(true, is_orderable(input, false, true));
}
TEST("Train: 3 cars, desc", 1) {
  std::vector<short> input = {3, 2, 1};
  // csokkeno sorrendet a stack megforditja
  CHECK_EQ(true, is_orderable(input, true, false));
  // queueba bemegy a legnagyobb elem a 2. a kimenetre a 3. kisebb mind2nel
  // tehat nem rendezheto
  CHECK_EQ(false, is_orderable(input, false, true));
}
TEST("Train: 3 cars, both possible", 1) {
  // mind2 esetben a legnagyobb bekerul a segedvaganyra majd a tobbi a
  // kimenetre majd a segedrol az 1 elem.
  std::vector<short> input = {3, 1, 2};
  CHECK_EQ(true, is_orderable(input, true, false));
  CHECK_EQ(true, is_orderable(input, false, true));
}
TEST("Train: 3 cars, only queue", 1) {
  std::vector<short> input = {3, 4, 1};
  // stackre kerul a 3,4 de nem tudjuk jo sorrendben kivenni
  CHECK_EQ(false, is_orderable(input, true, false));
  // queue eseten viszont ez a jo sorrend
  CHECK_EQ(true, is_orderable(input, false, true));
}
TEST("Train: Multiple labels", 1) {
  // egyenlo elemek kozott a sorrend mind1
  std::vector<short> input = {1, 1, 3, 2, 3, 2, 4, 4};
  CHECK_EQ(true, is_orderable(input, true, false));
  CHECK_EQ(true, is_orderable(input, false, true));
}
TEST("Train: Medium test", 1) {
  std::vector<short> input = {1, 3, 3, 3, 3, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2,
                              3, 2, 2, 3, 3, 2, 2, 3, 2, 2, 3, 2, 2, 4};
  CHECK_EQ(true, is_orderable(input, true, false));
  CHECK_EQ(true, is_orderable(input, false, true));
}
TEST("Train: Medium test2", 1) {
  // vegen levo 1-esek miatt kb minden a seged vaganyra megy. A kerdes, h onnan
  // jo sorrendben jonnek-e ki.
  std::vector<short> input = {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 3,
                              1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 4,
                              1, 4, 1, 4, 4, 1, 4, 1, 4, 1, 4, 1, 4, 2, 4};
  CHECK_EQ(false, is_orderable(input, true, false));
  CHECK_EQ(true, is_orderable(input, false, true));
}
TEST("Train: Medium test3", 1) {
  // vegen levo 1-esek miatt kb minden a seged vaganyra megy. A kerdes, h onnan
  // jo sorrendben jonnek-e ki.
  std::vector<short> input = {4, 1, 4, 1, 4, 1, 4, 1, 4, 1, 4, 4, 1, 4, 1, 4,
                              1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3,
                              1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1};
  CHECK_EQ(true, is_orderable(input, true, false));
  CHECK_EQ(false, is_orderable(input, false, true));
}
TEST("Train: Medium test4", 1) {
  std::vector<short> input = {4, 2, 3, 1, 2, 3, 2, 2, 2, 4, 2, 4, 1, 2, 4, 3,
                              1, 1, 2, 1, 3, 3, 4, 3, 2, 2, 2, 3, 3, 1, 4, 4,
                              2, 4, 3, 2, 3, 3, 4, 4, 4, 4, 3, 1, 1, 2, 3};
  CHECK_EQ(false, is_orderable(input, true, false));
  CHECK_EQ(false, is_orderable(input, false, true));
}
TEST("Train: Large test", 1) {
  std::vector<short> input = {3, 2, 4, 1, 1, 2, 4, 4, 1, 1, 1, 2, 2, 4, 2, 1, 1,
                              2, 3, 1, 4, 1, 1, 2, 2, 4, 3, 1, 3, 2, 4, 3, 1, 1,
                              4, 4, 4, 3, 4, 2, 3, 1, 4, 1, 1, 4, 3, 4, 1, 2, 1,
                              3, 2, 4, 4, 2, 1, 4, 1, 1, 4, 4, 4, 3, 4, 3, 4, 2,
                              3, 1, 3, 3, 1, 4, 4, 4, 3, 4, 1, 2, 2, 3, 3, 3, 1,
                              1, 3, 2, 1, 4, 1, 3, 1, 4, 1, 3, 2, 2, 1, 3};
  CHECK_EQ(false, is_orderable(input, true, false));
  CHECK_EQ(false, is_orderable(input, false, true));
}
TEST("Train: Large test", 1) {
  std::vector<short> input = {4, 1, 1, 1, 4, 1, 3, 3, 3, 1, 4, 3, 2, 3, 4, 2, 1,
                              4, 2, 1, 4, 3, 3, 2, 2, 2, 1, 1, 2, 2, 3, 4, 4, 4,
                              4, 1, 4, 1, 2, 2, 3, 1, 3, 4, 1, 3, 4, 4, 4, 1, 4,
                              4, 1, 2, 1, 3, 3, 4, 4, 1, 2, 3, 1, 1, 4, 1, 2, 2,
                              2, 2, 4, 1, 1, 4, 3, 1, 4, 4, 1, 4, 3, 4, 2, 2, 4,
                              1, 1, 3, 1, 3, 4, 4, 2, 1, 2, 4, 1, 4, 2, 2};
  CHECK_EQ(false, is_orderable(input, true, false));
  CHECK_EQ(false, is_orderable(input, false, true));
}

// Task 2
TEST("Tree: 3 leaves", 1) {
  // 4 node, minimalis fa, jo sorrendben vannak-e az elemek
  TernaryTree tree("(1;2;3)");
  CHECK_EQ(1, tree.get_value({0}));
  CHECK_EQ(2, tree.get_value({1}));
  CHECK_EQ(3, tree.get_value({2}));
  // ha olyan utvonalat kerunk le ami nincs a faban ezception
  CHECK_EXC(TreeException &, tree.get_value({0, 0}));
}

TEST("Tree: 5 leaves", 1) {
  TernaryTree tree("((1;2;3);4;5)");
  // 1 szinttel lejjebb is jo a sorrend
  CHECK_EQ(1, tree.get_value({0, 0}));
  CHECK_EQ(2, tree.get_value({0, 1}));
  CHECK_EQ(3, tree.get_value({0, 2}));
  CHECK_EQ(4, tree.get_value({1}));
  CHECK_EQ(5, tree.get_value({2}));
  // ha olyan utvonalat kerunk le ami nincs a faban ezception
  CHECK_EXC(TreeException &, tree.get_value({0, 0, 0}));
  CHECK_EXC(TreeException &, tree.get_value({1, 0}));
  // ha koztes node-ot kerunk le akkor is dobjunk exceptiont
  CHECK_EXC(TreeException &, tree.get_value({0}));
}

// innentol csak random testek, hogy mindig jo iranyba lep-e a lekerdezes
TEST("Tree: Small example", 1) {
  TernaryTree tree("((1;(2;3;4);5);(6;7;8);(9;0;1))");
  CHECK_EQ(1, tree.get_value({0, 0}));
  CHECK_EQ(2, tree.get_value({0, 1, 0}));
  CHECK_EQ(3, tree.get_value({0, 1, 1}));
  CHECK_EQ(4, tree.get_value({0, 1, 2}));
  CHECK_EQ(5, tree.get_value({0, 2}));
  CHECK_EQ(6, tree.get_value({1, 0}));
  CHECK_EQ(7, tree.get_value({1, 1}));
  CHECK_EQ(8, tree.get_value({1, 2}));
  CHECK_EQ(9, tree.get_value({2, 0}));
  CHECK_EQ(0, tree.get_value({2, 1}));
  CHECK_EQ(1, tree.get_value({2, 2}));
}

TEST("Tree: Chain right", 1) {
  TernaryTree tree("(1;2;(3;4;(5;6;(7;8;(9;0;(1;2;3))))))");
  CHECK_EQ(1, tree.get_value({0}));
  CHECK_EQ(2, tree.get_value({1}));
  CHECK_EQ(3, tree.get_value({2, 0}));
  CHECK_EQ(4, tree.get_value({2, 1}));
  CHECK_EQ(5, tree.get_value({2, 2, 0}));
  CHECK_EQ(6, tree.get_value({2, 2, 1}));
  CHECK_EQ(7, tree.get_value({2, 2, 2, 0}));
  CHECK_EQ(8, tree.get_value({2, 2, 2, 1}));
  CHECK_EQ(9, tree.get_value({2, 2, 2, 2, 0}));
  CHECK_EQ(0, tree.get_value({2, 2, 2, 2, 1}));
  CHECK_EQ(1, tree.get_value({2, 2, 2, 2, 2, 0}));
  CHECK_EQ(2, tree.get_value({2, 2, 2, 2, 2, 1}));
  CHECK_EQ(3, tree.get_value({2, 2, 2, 2, 2, 2}));
}

TEST("Tree: Chain left", 1) {
  std::unique_ptr<TernaryTree> tree;
  // a measure csak annyit csinal h a kodot lemeri es ha sokaig tart akkor
  // failel.  Mivel idot mer erdemes nem Debug modeban forditani, hanem
  // magasabb optimalizalasi szinten Release-ben O3-al ezeknel a mereseknel.
  // CLionben a run configokna tudsz release buildet beallitani.
  MEASURE("constructor", 1ms) {
    tree.reset(new TernaryTree("((((((1;2;3);4;5);6;7);8;9);0;1);2;3)"));
  }
  MEASURE("get_value", 5us) {
    CHECK_EQ(1, tree->get_value({0, 0, 0, 0, 0, 0}));
  }
  CHECK_EQ(2, tree->get_value({0, 0, 0, 0, 0, 1}));
  CHECK_EQ(3, tree->get_value({0, 0, 0, 0, 0, 2}));
  CHECK_EQ(4, tree->get_value({0, 0, 0, 0, 1}));
  CHECK_EQ(5, tree->get_value({0, 0, 0, 0, 2}));
  CHECK_EQ(6, tree->get_value({0, 0, 0, 1}));
  CHECK_EQ(7, tree->get_value({0, 0, 0, 2}));
  CHECK_EQ(8, tree->get_value({0, 0, 1}));
  CHECK_EQ(9, tree->get_value({0, 0, 2}));
  CHECK_EQ(0, tree->get_value({0, 1}));
  CHECK_EQ(1, tree->get_value({0, 2}));
  CHECK_EQ(2, tree->get_value({1}));
  CHECK_EQ(3, tree->get_value({2}));
}

TEST("Tree: Chain left 2: stack overflow", 1) {
  // nagy teszt, 30000 melysegu a fa 2MB stack size a feladatkiiras szerint 
  // ami durva becsleskent is kb 70 byte-ot jelent stack framenkent ha 
  // rekurziot csinalunk, ami viszonylag kevesnek tunik
  std::unique_ptr<TernaryTree> tree;
  std::string repr = "(1;2;3)";
  int n = 29999;
  for (int i = 0; i < n; ++i) {
    repr = "(" + repr + ";0;" + std::to_string(i % 10) + ")";
  }
  std::vector<short> check_path(n + 1, 0);
  MEASURE("constructor", 0.1s) { tree.reset(new TernaryTree(repr)); }
  MEASURE("deep search", 0.1s) { CHECK_EQ(1, tree->get_value(check_path)); }
  check_path[n] = 1;
  CHECK_EQ(2, tree->get_value(check_path));
  check_path[n] = 2;
  CHECK_EQ(3, tree->get_value(check_path));
}

// ennek a macronak a kifejtese egy main ami sorra meghivja a fenti
// fuggvenyeket.
WOODPECKER_MAIN

