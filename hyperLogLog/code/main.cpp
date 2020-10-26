// your solution comes here
#include "hyperloglog.hpp"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "woodpecker_precpp17.hpp"
#include <chrono>
#include <iostream>
#include <set>
#include <vector>

#include <random>

// slow solution using a red_black tree to store values and return the size of
// the tree
// WARNING: takes O(NlogN) time O(N) memory.. big test might take a while and
// use a lot of memory
// But if you do not believe the referene soutions you can try it :)
// to run it with ~2^27 element it took me ~360s while hyperloglog 0.5s both
// with -O3
template <typename ForwardIterator>
size_t reference(ForwardIterator begin, ForwardIterator end) {
  std::set<typename std::iterator_traits<ForwardIterator>::value_type> checker;
  for (; begin != end; ++begin) {
    checker.insert(*begin);
  }
  return checker.size();
}

TEST("1M element no duplicates", 1) {
  std::mt19937 gen(42); // fix seedfor random gen
  std::uniform_real_distribution<double> dist;
  // since the seed is fixed every time it will generate the same sequence..
  // so a little oraculum give me the number of unique elements which is
  // 1048576
  constexpr int log_stream_size = 20;
  std::vector<double> stream(
      1ul << log_stream_size); // stream with 2^20 = 1048576 elements
  for (size_t i = 0; i < 1ul << log_stream_size; ++i) {
    double rand = dist(gen);
    stream[i] = rand;
  }
  size_t res = hyperloglog(std::begin(stream), std::end(stream));
  // if this fails you probably use small number of bins/partitions, or wrong
  // alpha, or make a mistake during bit magic
  CHECK_EQ(true, 1048576 * 1.02 > res && 1048576 * 0.98 < res);
}

TEST("2M element half of which is duplicate", 1) {
  std::mt19937 gen(42); // fix seedfor random gen
  std::uniform_real_distribution<double> dist;
  // since the seed is fixed every time it will generate the same sequence..
  // so a little oraculum give me the number of unique elements which is
  // 1048576
  constexpr int log_stream_size = 20;
  std::vector<double> stream(
      1ul << (log_stream_size + 1)); // stream with 2^21 = 2096152 elements
  for (size_t i = 0; i < 1ul << log_stream_size; ++i) {
    double rand = dist(gen);
    stream[i] = rand;
    stream[i + (1ul << log_stream_size)] = rand;
  }
  size_t res = hyperloglog(std::begin(stream), std::end(stream));
  // if this fails and the first is not then... well I have no clue unless you
  // implemented a different algorithm
  // in other words this is a sanity check I gueess...
  // checks whethet duplicates hashed to the same value or not.
  CHECK_EQ(true, 1048576 * 1.02 > res && 1048576 * 0.98 < res);
}

TEST("Linear time 1M vs 2M vs 4M", 1) {
  // HLL should run in linear time.
  std::mt19937 gen(5);
  std::uniform_real_distribution<double> dist;
  constexpr int log_stream_size = 20;
  std::vector<double> stream(1ul << log_stream_size);
  std::vector<double> stream1(1ul << (log_stream_size + 1));
  std::vector<double> stream2(1ul << (log_stream_size + 2));
  for (size_t i = 0; i < 1ul << log_stream_size; ++i) {
    double rand = dist(gen);
    stream[i] = rand;
  }
  for (size_t i = 0; i < stream1.size(); ++i) {
    double rand = dist(gen);
    stream1[i] = rand;
  }
  for (size_t i = 0; i < stream2.size(); ++i) {
    double rand = dist(gen);
    stream2[i] = rand;
  }
  MEASURE("All measurement should run fast (remember -O3 and release mode)",
          100ms) {
    // measure the runtime of the small 1M stream as a baseline
    auto start = std::chrono::high_resolution_clock::now();
    size_t res = hyperloglog(stream.begin(), stream.end());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds timeout =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    CHECK_EQ(true, 1048576 * 1.02 > res && 1048576 * 0.98 < res);
    // in theory the 2M dataset should run 2 times as much just to be safe we
    // allow 10% overhead
    MEASURE("2M", 2.2 * timeout) {
      res = hyperloglog(stream1.begin(), stream1.end());
    }
    CHECK_EQ(true, 2097152 * 1.02 > res && 2097152 * 0.98 < res);
    // Again double the problem size and the runtime should double as well.
    MEASURE("4M", 4.4 * timeout) {
      res = hyperloglog(stream2.begin(), stream2.end());
    }
    CHECK_EQ(true, 4194304 * 1.02 > res && 4194304 * 0.98 < res);
  }
}

class random_generator_iterator {
  // this class behaves like an iterator
  // you can increment it and dereference it
  // basically we can generate the stream at the same time as we process it
  std::uniform_real_distribution<double> *dist = nullptr;
  std::mt19937 *gen = nullptr;
  size_t counter = 0;

public:
  random_generator_iterator(std::uniform_real_distribution<double> &dist,
                            std::mt19937 &gen)
      : dist(&dist), gen(&gen) {}
  // only used for end iterators. should not dereference
  random_generator_iterator(size_t end_idx) : counter(end_idx) {}

  using difference_type = std::ptrdiff_t;
  using value_type = double;
  using pointer = void;
  using reference = double;
  using iterator_category = std::input_iterator_tag;

  double operator*() { return (*dist)(*gen); }

  random_generator_iterator &operator++() {
    counter++;
    return *this;
  }

  random_generator_iterator operator++(int) {
    // to be honest this is broken, but I don't care
    // the iterator has no memory so dereferencing the old value results in
    // a new random number and not the same as the original
    // You mustn't use it like
    //  it1 = it2++;
    //  *it1;
    random_generator_iterator it(*this);
    counter++;
    return it;
  }

  bool operator==(const random_generator_iterator &it) {
    return counter == it.counter;
  }

  bool operator!=(const random_generator_iterator &it) {
    return counter != it.counter;
  }
};

TEST("Measure big test", 1) {
  // This test shows how one could use a streaming algorithm to process a
  // dataset that does not fit in memory. Imagine that you read some data from a
  // file each time you dereference an iterator.
  std::mt19937 gen(42);
  std::uniform_real_distribution<double> dist;
  constexpr size_t log_stream_size = 27;
  constexpr size_t log_small_stream_size = 20;
  constexpr size_t stream_size = 1ul << log_stream_size;
  constexpr size_t small_stream_size = 1ul << log_small_stream_size;
  auto start = std::chrono::high_resolution_clock::now();
  size_t res = hyperloglog(random_generator_iterator(dist, gen),
                           random_generator_iterator(small_stream_size));
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds timeout =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  CHECK_EQ(true, 1048576 * 1.02 > res && 1048576 * 0.98 < res);
  gen.seed(42);
  res = 0;
  // 2^27 double is ~1 GiB of data I hope you does not try to store them.
  MEASURE("2^27 elements",
          1.1 * (1ul << (log_stream_size - log_small_stream_size)) * timeout) {
    res = hyperloglog(random_generator_iterator(dist, gen),
                      random_generator_iterator(stream_size));
  }
  CHECK_EQ(true, 134217728 * 1.02 > res && 134217728 * 0.98 < res);
}

WOODPECKER_TEST_MAIN
