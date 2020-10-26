// your solution comes here
#include "heavy_hitters.hpp"
#include "rmq.hpp"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "woodpecker_precpp17.hpp"
#include <iterator>
#include <random>
#include <vector>

TEST("HeavyHitters: Simple case", 1) {
  std::vector<int> stream{1, 1, 1, 1, 1};
  std::vector<int> res = heavy_hitters(stream.begin(), stream.end(), 2);
  CHECK_EQ(1, res.size());
  CHECK_EQ(1, res[0]);
  stream.resize(1);
  res = heavy_hitters(stream.begin(), stream.end(), 2);
  CHECK_EQ(1, res.size());
  CHECK_EQ(1, res[0]);
}

TEST("HeavyHitters: One against all", 1) {
  std::vector<int> stream{1, 2, 2, 2, 2};
  std::vector<int> res = heavy_hitters(stream.begin(), stream.end(), 2);
  CHECK_EQ(1, res.size());
  CHECK_EQ(2, res[0]);
  stream.resize(1);
  res = heavy_hitters(stream.begin(), stream.end(), 2);
  CHECK_EQ(1, res.size());
  CHECK_EQ(1, res[0]);
}

TEST("HeavyHitters:  small test", 1) {
  std::vector<int> stream{1, 2, 3, 4, 5, 5, 5, 5, 5, 1, 1, 1, 5, 5, 5};
  std::vector<int> res = heavy_hitters(stream.begin(), stream.end(), 2);
  CHECK_EQ(1, res.size());
  CHECK_EQ(5, res[0]);
}

TEST("HeavyHitters: k=3", 1) {
  std::vector<int> stream{1, 1, 1, 2, 2, 2, 3};
  std::vector<int> res = heavy_hitters(stream.begin(), stream.end(), 3);
  // N = 7, N/k = 2 -> 2 megoldas van 1 es 2
  CHECK_EQ(2, res.size());
  // count fgv megszamolja a 2 iterator kozott hanyszor szerepel a 3.
  // parameterben kapott ertek.
  // Lenyegeben megnezi h mind2 szamot megtalaltuk es mind2 szam csak egyszer
  // szerepel a tombben
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 1));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 2));
}

TEST("HeavyHitters: Self-voting pirates", 1) {
  std::vector<int> stream{1, 2, 3, 4, 5};
  // mki magara szavaz mindenkire csak 1 szavazat erkezik a vegen az 5 egyedul
  // all de nem jo eredmeny.
  std::vector<int> res = heavy_hitters(stream.begin(), stream.end(), 2);
  CHECK_EQ(0, res.size());
  // meg N/4=1 nel sem fogadhatjuk el egyik megoldast se
  res = heavy_hitters(stream.begin(), stream.end(), 4);
  CHECK_EQ(0, res.size());
}

TEST("HeavyHitters: k=4", 1) {
  std::vector<int> stream{1, 1, 1, 2, 2, 2, 3, 3};
  std::vector<int> res = heavy_hitters(stream.begin(), stream.end(), 4);
  // N = 8, N/k = 2 -> 2 megoldas van 1 es 2
  CHECK_EQ(2, res.size());
  // count fgv megszamolja a 2 iterator kozott hanyszor szerepel a 3.
  // parameterben kapott ertek.
  // Lenyegeben megnezi h mind2 szamot megtalaltuk es mind2 szam csak egyszer
  // szerepel a tombben
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 1));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 2));
  // k=4 -> max 3 megoldas lehet, meg egy harmas es az is elfogadhato lesz
  stream.push_back(3);
  res = heavy_hitters(stream.begin(), stream.end(), 4);
  CHECK_EQ(3, res.size());
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 1));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 2));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 3));
}

TEST("HeavyHitters: Medium 1", 1) {
  std::mt19937 gen(42);
  std::uniform_int_distribution<int> dist(0, 10);
  std::vector<int> stream;
  // streambe belepushbackelek 100 random szamot.
  std::generate_n(std::back_inserter(stream), 100,
                  [&dist, &gen]() { return dist(gen); });
  std::vector<int> res = heavy_hitters(stream.begin(), stream.end(), 10);
  CHECK_EQ(4, res.size());
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 0));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 2));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 4));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 6));
  MEASURE("small run", 1ms) {
    res = heavy_hitters(stream.begin(), stream.end(), 7);
  }
  CHECK_EQ(0, res.size());
  MEASURE("small run 2", 1ms) {
    res = heavy_hitters(stream.begin(), stream.end(), 8);
  }
  CHECK_EQ(2, res.size());
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 0));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 6));
}

TEST("HeavyHitters: Measure 1M, 2M, 4M runtime linear", 1) {
  std::mt19937 gen(42);
  std::uniform_int_distribution<int> dist(0, 10);
  std::vector<int> stream;
  std::generate_n(std::back_inserter(stream), 1000000,
                  [&dist, &gen]() { return dist(gen); });
  // streambe belepushbackelek 1M random szamot.
  std::vector<int> res;
  auto start = std::chrono::high_resolution_clock::now();
  MEASURE("1M", 150ms) { res = heavy_hitters(stream.begin(), stream.end(), 11); }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds timeout =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  CHECK_EQ(7, res.size());
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 0));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 2));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 5));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 6));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 7));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 8));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 9));
  // streambe belepushbackelek 1M random szamot.
  std::generate_n(std::back_inserter(stream), 1000000,
                  [&dist, &gen]() { return dist(gen); });
  // mivel O(N) az algoritmus kb ketszer annzi ido ketszer annzi adatot
  // feldolgozni.
  MEASURE("2M", 2.2 * timeout) {
    res = heavy_hitters(std::begin(stream), std::end(stream), 11);
  }
  CHECK_EQ(6, res.size());
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 2));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 3));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 5));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 6));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 8));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 10));
  // streambe belepushbackelek meg 2M random szamot.
  std::generate_n(std::back_inserter(stream), 2000000,
                  [&dist, &gen]() { return dist(gen); });
  MEASURE("4M", 4.4 * timeout) {
    res = heavy_hitters(stream.begin(), stream.end(), 11);
  }
  CHECK_EQ(6, res.size());
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 0));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 3));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 5));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 6));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 8));
  CHECK_EQ(1, std::count(std::begin(res), std::end(res), 10));
}

/* TASK 2: RMQ */

TEST("RMQ: 1 element", 1) {
  std::vector<int> A{0};
  rmq table(A);
  CHECK_EQ(0, table.max_pos(0, 0));
}

TEST("RMQ: global max", 1) {
  std::vector<int> A{0, 3, 4, 1, 7, 9, 3, 5};
  rmq table(A);
  CHECK_EQ(5, table.max_pos(0, 7));
  // Max in first
  A[0] = 10;
  rmq table2(A);
  CHECK_EQ(0, table2.max_pos(0, 7));
  // MAx in last
  A[7] = 11;
  rmq table3(A);
  CHECK_EQ(7, table3.max_pos(0, 7));
}

TEST("RMQ: Equals", 1) {
  std::vector<int> A{0, 5, 2, 5, 4, 3, 1, 6, 3};
  rmq table(A);
  // in case of equal values the smallerindex is the solution
  CHECK_EQ(1, table.max_pos(0, 3));
  CHECK_EQ(1, table.max_pos(1, 4));
}

TEST("RMQ: ranges", 1) {
  std::vector<int> A{0, 5, 2, 5, 4, 3, 1, 6, 3};
  rmq table(A);
  // length 2 both both index should be in the boundary
  CHECK_EQ(1, table.max_pos(0, 1));
  CHECK_EQ(1, table.max_pos(1, 2));
  CHECK_EQ(3, table.max_pos(2, 3));
  CHECK_EQ(3, table.max_pos(3, 4));
  CHECK_EQ(4, table.max_pos(4, 5));
  CHECK_EQ(5, table.max_pos(5, 6));
  CHECK_EQ(7, table.max_pos(6, 7));
  CHECK_EQ(7, table.max_pos(7, 8));
  // length 4 both both index should be in the boundary
  CHECK_EQ(1, table.max_pos(0, 3));
  CHECK_EQ(1, table.max_pos(1, 4));
  CHECK_EQ(3, table.max_pos(2, 5));
  CHECK_EQ(3, table.max_pos(3, 6));
  CHECK_EQ(7, table.max_pos(4, 7));
  CHECK_EQ(7, table.max_pos(5, 8));
  // l = 8
  CHECK_EQ(7, table.max_pos(0, 7));
  CHECK_EQ(7, table.max_pos(1, 8));
}

TEST("RMQ: ranges with length which is not a power of 2", 1) {
  std::vector<int> A{0, 5, 2, 5, 4, 3, 1, 6, 3};
  rmq table(A);
  // l = 3
  // also lekerdezesbol
  CHECK_EQ(1, table.max_pos(0, 2));
  // also lekerdezesbol
  CHECK_EQ(1, table.max_pos(1, 3));
  // also lekerdezesbol
  CHECK_EQ(3, table.max_pos(2, 4));
  // also lekerdezesbol
  CHECK_EQ(3, table.max_pos(3, 5));
  // also lekerdezesbol
  CHECK_EQ(4, table.max_pos(4, 6));
  // felso lekerdezesbol
  CHECK_EQ(7, table.max_pos(5, 7));
  // also lekerdezesbol
  CHECK_EQ(7, table.max_pos(6, 8));
  // l = 5
  // also lekerdezesbol
  CHECK_EQ(1, table.max_pos(0, 4));
  // also lekerdezesbol
  CHECK_EQ(1, table.max_pos(1, 5));
  // also lekerdezesbol
  CHECK_EQ(3, table.max_pos(2, 6));
  // felso lekerdezesbol
  CHECK_EQ(7, table.max_pos(3, 7));
  // also lekerdezesbol
  CHECK_EQ(7, table.max_pos(4, 8));
  // l = 6
  CHECK_EQ(1, table.max_pos(0, 5));
  CHECK_EQ(1, table.max_pos(1, 6));
  CHECK_EQ(7, table.max_pos(2, 7));
  CHECK_EQ(7, table.max_pos(3, 8));
  // l = 7
  CHECK_EQ(1, table.max_pos(0, 6));
  CHECK_EQ(7, table.max_pos(1, 7));
  CHECK_EQ(7, table.max_pos(2, 8));
  // l = 9
  CHECK_EQ(7, table.max_pos(0, 8));
}

TEST("RMQ: MEASURE constructor: O(nlogn)", 1) {
  std::mt19937 gen(42);
  std::uniform_int_distribution<int> dist(0, 100);
  std::vector<int> A;
  // streambe belepushbackelek 1024 random szamot.
  std::generate_n(std::back_inserter(A), 1024,
                  [&dist, &gen]() { return dist(gen); });

  auto start = std::chrono::high_resolution_clock::now();
  MEASURE("1024", 10ms) {
    rmq table(A);
    CHECK_EQ(42, table.max_pos(42, 43));
    CHECK_EQ(33, table.max_pos(32, 36));
    CHECK_EQ(127, table.max_pos(125, 135));
    CHECK_EQ(138, table.max_pos(125, 150));
    CHECK_EQ(138, table.max_pos(125, 160));
    CHECK_EQ(217, table.max_pos(200, 270));
    CHECK_EQ(780, table.max_pos(770, 1024));
    CHECK_EQ(780, table.max_pos(512, 1023));
  }
  std::chrono::microseconds timeout =
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::high_resolution_clock::now() - start);
  // streambe belepushbackelek 1024 random szamot.
  std::generate_n(std::back_inserter(A), 1024,
                  [&dist, &gen]() { return dist(gen); });

  // NlogN: 1024*10 volt most 2048*11 tehat kb 2.2-szor annyi ido
  MEASURE("2048", 2.5 * timeout) {
    rmq table(A);
    CHECK_EQ(2, table.max_pos(1, 2));
    CHECK_EQ(33, table.max_pos(32, 36));
    CHECK_EQ(127, table.max_pos(125, 135));
    CHECK_EQ(1127, table.max_pos(1125, 1150));
    CHECK_EQ(1534, table.max_pos(1525, 1560));
    CHECK_EQ(2031, table.max_pos(1970, 2040));
    CHECK_EQ(1779, table.max_pos(1770, 2024));
    CHECK_EQ(780, table.max_pos(512, 1023));
  }
  // streambe belepushbackelek 1024 random szamot.
  std::generate_n(std::back_inserter(A), 2048,
                  [&dist, &gen]() { return dist(gen); });
  // NlogN: 1024*10 volt most 4096*12 tehat kb 4.8-szor annyi ido
  MEASURE("4096", 5.5 * timeout) {
    rmq table(A);
    CHECK_EQ(2, table.max_pos(1, 2));
    CHECK_EQ(3035, table.max_pos(3032, 3036));
    CHECK_EQ(127, table.max_pos(125, 135));
    CHECK_EQ(1127, table.max_pos(1125, 1150));
    CHECK_EQ(2525, table.max_pos(2525, 2560));
    CHECK_EQ(2031, table.max_pos(1970, 2040));
    CHECK_EQ(3778, table.max_pos(3770, 4024));
    CHECK_EQ(780, table.max_pos(512, 1023));
    CHECK_EQ(780, table.max_pos(512, 4095));
  }
}

TEST("RMQ: MEASURE querries: O(1)", 1) {
  std::mt19937 gen(54);
  std::uniform_int_distribution<int> dist(1000);
  std::vector<int> A;
  // note: N = 1024*1024, if rmq stores N^2 of data
  // then we got 1024*1024*1024*1024 as a matrix
  // which will exceed the 100 MiB, while N*logN
  // 1024*1024*20 leads to 80Mib of data.
  std::generate_n(std::back_inserter(A), 1024 * 1024,
                  [&]() { return dist(gen); });
  rmq table(A);
  MEASURE("request l=3", 50us) { CHECK_EQ(5, table.max_pos(5, 7)); }
  MEASURE("request l=7", 50us) { CHECK_EQ(31, table.max_pos(30, 36)); }
  MEASURE("request l=35", 50us) { CHECK_EQ(62, table.max_pos(60, 94)); }
  MEASURE("request l=70", 50us) { CHECK_EQ(125, table.max_pos(100, 169)); }
  MEASURE("request l=259", 50us) { CHECK_EQ(521, table.max_pos(200, 558)); }
  MEASURE("request l=519", 50us) { CHECK_EQ(5304, table.max_pos(5000, 5518)); }
  MEASURE("request l=1501", 50us) { CHECK_EQ(7507, table.max_pos(7000, 8500)); }
  MEASURE("request l=2049", 50us) {
    CHECK_EQ(10582, table.max_pos(9000, 11048));
  }
  MEASURE("request l=4100", 50us) {
    CHECK_EQ(22315, table.max_pos(20000, 24099));
  }
  MEASURE("request l=9000", 50us) {
    CHECK_EQ(102260, table.max_pos(100000, 108999));
  }
  MEASURE("request l=70k", 50us) {
    CHECK_EQ(514990, table.max_pos(500000, 570000));
  }
  MEASURE("request l=150k", 50us) {
    CHECK_EQ(711487, table.max_pos(700000, 850000));
  }
  MEASURE("request l=300k", 50us) {
    CHECK_EQ(466888, table.max_pos(200000, 500000));
  }
  MEASURE("request l=1024*1024", 50us) {
    CHECK_EQ(685700, table.max_pos(0, 1048575));
  }
}

TEST("RMQ: copy constructor and assignment operator", 1) {
  std::vector<int> A{3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5};
  rmq table(A);
  A[11] = 9;
  rmq table2(A);
  CHECK_EQ(5, table.max_pos(0, 11));
  CHECK_EQ(11, table2.max_pos(0, 11));
  // swap it and use assignment operators
  rmq tmp(table);
  table = table2;
  table2 = tmp;
  CHECK_EQ(5, table2.max_pos(0, 11));
  CHECK_EQ(5, tmp.max_pos(0, 11));
  CHECK_EQ(11, table.max_pos(0, 11));
}

WOODPECKER_MAIN
