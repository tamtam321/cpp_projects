#ifndef WOODPECKER_HPP
#define WOODPECKER_HPP
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

namespace woodpecker {

void usage(const char *name) {
  std::cerr << "Usage:" << std::endl;
  std::cerr << "\t" << name
            << " [-v] [-t test_no] [-f logfile] [-c] [-- other arguments]"
            << std::endl;
  std::cerr
      << "-v           Verbose output: print tests and results to stdout\n"
      << "               Default setting when no logfile is specified\n"
      << "-t test_no   Run only specific testcase\n"
      << "-f logifle   Write test results to file\n"
      << "-c           Configuration mode: print only test numbers and names\n";
}

struct woodpecker_controller;
struct test_controller;
using TEST_FUNC = std::function<void(test_controller &)>;

struct test_case {
  std::string test_name;
  int points;
  TEST_FUNC func;
  void run_test(woodpecker_controller &);
};

struct woodpecker_controller {
private:
  static std::vector<test_case> testcases; /**< descriptors for all testcase */
public:
  int test_no =
      0; /**< id of the test that will be runned, 0 to run all tests */
  int counter = 0;          /**< test counter used in macros */
  int total = 0;            /**< total points achieved */
  int maximum = 0;          /**< maximum achievable points */
  std::ofstream test_log;   /**< logfile to write test results */
  bool verbose = false;     /**< print logs to stderr */
  bool config_mode = false; /**< output test configs instead of running them*/
  woodpecker_controller(int argc, const char *argv[]) {
    int arg_idx = 1;
    while (arg_idx < argc) {
      if (!strcmp(argv[arg_idx], "-v")) {
        verbose = true;
      } else if (!strcmp(argv[arg_idx], "-t")) {
        test_no = std::atoi(argv[++arg_idx]);
      } else if (!strcmp(argv[arg_idx], "-f")) {
        test_log.open(argv[++arg_idx]);
      } else if (!strcmp(argv[arg_idx], "-c")) {
        config_mode = true;
      } else if (!strcmp(argv[arg_idx], "--")) {
        break;
      } else {
        usage(argv[0]);
        exit(2);
      }
      arg_idx++;
    }
    if (!test_log.is_open() && !verbose) {
      verbose = true;
    }
  }

  void run_tests() {
    for (auto &testcase : testcases) {
      testcase.run_test(*this);
    }
  }

  ~woodpecker_controller() {
    if (!config_mode) {
      *this << ">>> TOTAL POINTS: " << total << "\n";
    }
    if (test_log.is_open()) {
      test_log.close();
    }
  }

  template <typename T> woodpecker_controller &operator<<(const T &printable) {
    if (test_log.is_open())
      test_log << printable;
    if (verbose)
      std::cerr << printable;
    return *this;
  }

  struct AutoReg {
    AutoReg(std::string name, int points, TEST_FUNC func) {
      woodpecker_controller::testcases.push_back({name, points, func});
    }
  };
};
// define static var
std::vector<test_case> woodpecker_controller::testcases;

struct test_controller {
  bool enabled, run;
  bool error_free = true;
  std::string label;
  int points;
  bool ok;
  int &out;
  woodpecker_controller &woodpecker_cntr;
  test_controller(bool enabled, std::string &&label, int points, int &out,
                  woodpecker_controller &woodpecker_cntr)
      : enabled(enabled), run(enabled), label(std::move(label)), points(points),
        ok(true), out(out), woodpecker_cntr(woodpecker_cntr) {
    if (woodpecker_cntr.config_mode) {
      this->enabled = false;
      this->run = false;
      // Output timeout, memory limit, etc.
      // Just the name and points for now.
      woodpecker_cntr << this->points << ";" << this->label << "\n";
    }
    if (this->enabled) {
      woodpecker_cntr.maximum += this->points;
      woodpecker_cntr << ">>> Test: " << this->label << " (" << points << ")\n";
    }
  }

  test_controller &operator<<(std::string msg) {
    if (msg.find("ERROR") != std::string::npos) {
      error_free = false;
      ok = false;
    }
    woodpecker_cntr << msg;
    return *this;
  }

  ~test_controller() {
    if (enabled && error_free) {
      out += ok ? points : 0;
      woodpecker_cntr << ">>> Result: " << (ok ? "PASS" : "FAIL") << "\n";
    }
  }
};

struct measurement_controller {
  std::string name;
  std::chrono::microseconds timeout;
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  bool run;
  woodpecker_controller &woodpecker_cntr;
  test_controller &test_cntr;
  measurement_controller(std::string &&name, std::chrono::microseconds timeout,
                         woodpecker_controller &woodpecker_cntr,
                         test_controller &test_cntr)
      : name(std::move(name)), timeout(timeout),
        start(std::chrono::high_resolution_clock::now()), run(true),
        woodpecker_cntr(woodpecker_cntr), test_cntr(test_cntr) {}
  ~measurement_controller() {
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds out =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if (out > timeout) {
      woodpecker_cntr << ">>> TIMEOUT: " << name << " " << out.count()
                      << "us vs " << timeout.count() << "us\n";
      test_cntr.ok = false;
    }
  }
};

void test_case::run_test(woodpecker_controller &woodpecker_cntr) {
  test_controller cntr(++woodpecker_cntr.counter == woodpecker_cntr.test_no ||
                           woodpecker_cntr.test_no == 0,
                       std::move(test_name), points, woodpecker_cntr.total,
                       woodpecker_cntr);
  try {
    if (cntr.enabled) {
      func(cntr);
    }
  } catch (std::bad_alloc &) {
    cntr << ">>> Result: RUNTIME ERROR: MEMORY LIMIT EXCEEDED\n";
  } catch (const std::exception &e) {
    cntr << ">>> Result: RUNTIME ERROR: " << e.what() << "\n";
  } catch (...) {
    cntr << ">>> Result: RUNTIME ERROR\n";
  }
}

} // end namespace woodpecker

#define GLUE_HELPER(x, y) x##y
#define GLUE(x, y) GLUE_HELPER(x, y)
#define WOODPECKER_UNIQUE_NAME(name) GLUE(name, __LINE__)

#define INTERNAL_WOODPECKER_TEST_CASE(test_func, label, points)                \
  static void test_func(woodpecker::test_controller &);                        \
  namespace woodpecker {                                                       \
  woodpecker_controller::AutoReg                                               \
      WOODPECKER_UNIQUE_NAME(auto_registrar)(label, points, test_func);        \
  }                                                                            \
  static void test_func(woodpecker::test_controller &cntr)

#define TEST(label, points)                                                    \
  INTERNAL_WOODPECKER_TEST_CASE(WOODPECKER_UNIQUE_NAME(woodpecker_test),       \
                                label, points)

#define CHECK_EQ(val, expr)                                                    \
  if (cntr.ok) {                                                               \
    cntr.ok = val == (expr);                                                   \
    if (!cntr.ok) {                                                            \
      cntr.woodpecker_cntr << ">>> " << #val << " == " << #expr                \
                           << " failed at " << __LINE__                        \
                           << " Expr result: " << (expr) << "\n";              \
    }                                                                          \
  }

#define CHECK_EXC(exc, code)                                                   \
  if (cntr.ok) {                                                               \
    try {                                                                      \
      code;                                                                    \
      cntr.ok = false;                                                         \
      cntr.woodpecker_cntr << ">>> expected exception\n";                      \
    } catch (exc) {                                                            \
    }                                                                          \
    if (!cntr.ok) {                                                            \
      cntr.woodpecker_cntr << #code << " failed at" << __LINE__ << "\n";       \
    }                                                                          \
  }

#define CHECK_NOEXC(code)                                                      \
  if (cntr.ok) {                                                               \
    try {                                                                      \
      code;                                                                    \
      cntr.ok = true;                                                          \
    } catch (std::exception & e) {                                             \
      cntr.woodpecker_cntr << ">>> " << #code                                  \
                           << " throws unexpected exception at " << __LINE__   \
                           << " unexpected exception: " << e.what() << "\n";   \
      cntr.ok = false;                                                         \
    }                                                                          \
    if (!cntr.ok) {                                                            \
      cntr.woodpecker_cntr << #code << " failed at" << __LINE__ << "\n";       \
    }                                                                          \
  }

#define MEASURE(label, timeout)                                                \
  using namespace std::chrono_literals;                                        \
  for (woodpecker::measurement_controller mcntr(                               \
           label,                                                              \
           std::chrono::duration_cast<std::chrono::microseconds>(timeout),     \
           cntr.woodpecker_cntr, cntr);                                        \
       mcntr.run; mcntr.run = false)

#define WOODPECKER_INIT                                                        \
  woodpecker::woodpecker_controller woodpecker_cntr(argc, argv)

#define WOODPECKER_RUN_TESTS woodpecker_cntr.run_tests()

#define WOODPECKER_MAIN                                                        \
  int main(int argc, const char *argv[]) {                                     \
    WOODPECKER_INIT;                                                           \
    WOODPECKER_RUN_TESTS;                                                      \
    return 0;                                                                  \
  }

#define WOODPECKER_TEST_MAIN                                                   \
  int main(int argc, const char *argv[]) {                                     \
    WOODPECKER_INIT;                                                           \
    WOODPECKER_RUN_TESTS;                                                      \
    return woodpecker_cntr.maximum == woodpecker_cntr.total ? 0 : 1;           \
  }

#endif /* ifndef WOODPECKER_HPP */
