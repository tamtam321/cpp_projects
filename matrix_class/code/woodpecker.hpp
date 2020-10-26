#ifndef WOODPECKER_HPP
#define WOODPECKER_HPP
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <utility>
#include <span>
#include <vector>

namespace woodpecker {

void usage(const char *name) {
  std::cerr << "Usage:" << std::endl;
  std::cerr << "\t" << name
            << " [-v] [-t test_no] [-f logfile] [-c] [--no-timeout] [-- other "
               "arguments]"
            << std::endl;
  std::cerr
      << "-v           Verbose output: print tests and results to stdout\n"
      << "               Default setting when no logfile is specified\n"
      << "-t test_no   Run only specific testcase\n"
      << "-f logifle   Write test results to file\n"
      << "-c           Configuration mode: print only test numbers and names\n"
      << "--no-timeout Don't check timeouts.\n";
}

struct woodpecker_controller;
struct test_controller;
using TEST_FUNC = std::function<void(test_controller &)>;

struct test_params_t {
  int points;
  long long mem_limit, stack_limit;
};

struct test_case {
  std::string test_name;
  test_params_t test_params;
  woodpecker::TEST_FUNC func;
  void run_test(woodpecker_controller & /*woodpecker_cntr*/);
};

struct woodpecker_controller {
private:
  static inline std::vector<test_case>
      testcases; /**< descriptors for all testcase */
public:
  int test_no =
      0; /**< id of the test that will be runned, 0 to run all tests */
  int counter = 0;            /**< test counter used in macros */
  int total = 0;              /**< total points achieved */
  int maximum = 0;            /**< maximum achievable points */
  std::ofstream test_log;     /**< logfile to write test results */
  bool verbose = false;       /**< print logs to stderr */
  bool config_mode = false;   /**< output test configs instead of running them*/
  bool check_timeouts = true; /**< check timeouts */
  test_params_t default_params{0, -1, -1};
  explicit woodpecker_controller(const std::span<const char *> &argv) noexcept {
    size_t arg_idx = 1;
    while (arg_idx < argv.size()) {
      if (!strcmp(argv[arg_idx], "-v")) {
        verbose = true;
      } else if (!strcmp(argv[arg_idx], "-t")) {
        test_no = std::stoi(argv[++arg_idx]);
      } else if (!strcmp(argv[arg_idx], "-f")) {
        test_log.open(argv[++arg_idx]);
      } else if (!strcmp(argv[arg_idx], "-c")) {
        config_mode = true;
      } else if (!strcmp(argv[arg_idx], "--no-timeout")) {
        check_timeouts = false;
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

  woodpecker_controller(const woodpecker_controller&) = delete;
  woodpecker_controller(woodpecker_controller&&) = delete;
  woodpecker_controller &operator=(const woodpecker_controller&) = delete;
  woodpecker_controller &operator=(woodpecker_controller&&) = delete;

  template <typename T> woodpecker_controller &operator<<(const T &printable) {
    if (test_log.is_open())
      test_log << printable;
    if (verbose)
      std::cerr << printable;
    return *this;
  }

  struct AutoReg {
    AutoReg(const std::string &name, int points,
            const woodpecker::TEST_FUNC &func, long long mem_limit,
            long long stack_limit) {
      woodpecker_controller::testcases.push_back(
          {name, {points, mem_limit, stack_limit}, func});
    }
  };
};

struct test_controller {
  bool enabled, error_free = true;
  std::string label;
  test_params_t test_params;
  bool ok;
  int &out;
  woodpecker_controller &woodpecker_cntr;
  test_controller(bool enabled, std::string &&label,
                  const test_params_t &test_params, int &out,
                  woodpecker_controller &woodpecker_cntr)
      : enabled(enabled), label(std::move(label)), test_params(test_params),
        ok(true), out(out), woodpecker_cntr(woodpecker_cntr) {
    if (woodpecker_cntr.config_mode) {
      this->enabled = false;
      // Output points, memory limit, etc.
      long long mem_limit = this->test_params.mem_limit;
      if (mem_limit < 0) {
        mem_limit = woodpecker_cntr.default_params.mem_limit;
      }
      long long stack_limit = this->test_params.stack_limit;
      if (stack_limit < 0) {
        stack_limit = woodpecker_cntr.default_params.stack_limit;
      }
      woodpecker_cntr << this->test_params.points << ";" << mem_limit << ";"
                      << stack_limit << ";" << this->label << "\n";
    }
    if (this->enabled) {
      woodpecker_cntr.maximum += this->test_params.points;
      woodpecker_cntr << ">>> Test: " << this->label << " ("
                      << test_params.points << ")\n";
    }
  }

  test_controller &operator<<(const std::string &msg) {
    if (msg.find("ERROR") != std::string::npos) {
      error_free = false;
      ok = false;
    }
    woodpecker_cntr << msg;
    return *this;
  }

  ~test_controller() {
    if (enabled && error_free) {
      out += ok ? test_params.points : 0;
      woodpecker_cntr << ">>> Result: " << (ok ? "PASS" : "FAIL") << "\n";
    }
  }
  test_controller(const test_controller&) = delete;
  test_controller(test_controller&&) = delete;
  test_controller &operator=(const test_controller&) = delete;
  test_controller &operator=(test_controller&&) = delete;
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
    if (!woodpecker_cntr.check_timeouts) {
      return;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds out =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if (out > timeout) {
      woodpecker_cntr << ">>> TIMEOUT: " << name << " " << out.count()
                      << "us vs " << timeout.count() << "us\n";
      test_cntr.ok = false;
    }
  }
  measurement_controller(const measurement_controller&) = delete;
  measurement_controller(measurement_controller&&) = delete;
  measurement_controller &operator=(const measurement_controller&) = delete;
  measurement_controller &operator=(measurement_controller&&) = delete;
};

void test_case::run_test(woodpecker_controller &woodpecker_cntr) {
  test_controller cntr(++woodpecker_cntr.counter == woodpecker_cntr.test_no ||
                       woodpecker_cntr.test_no == 0,
                       std::move(test_name), test_params, woodpecker_cntr.total,
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

#define OVERLOADIFY(_1, _2, _3, _4, NAME, ...) NAME

#define INTERNAL_WOODPECKER_TEST_CASE(test_func, label, points, mem_limit,     \
                                      stack_limit)                             \
  static void test_func(woodpecker::test_controller &);                        \
  namespace woodpecker {                                                       \
  const woodpecker_controller::AutoReg                                         \
      WOODPECKER_UNIQUE_NAME(auto_registrar)(label, points, test_func,         \
                                             mem_limit, stack_limit);          \
  }                                                                            \
  static void test_func(woodpecker::test_controller &cntr)

#define TEST2(label, points)                                                   \
  INTERNAL_WOODPECKER_TEST_CASE(WOODPECKER_UNIQUE_NAME(woodpecker_test),       \
                                label, points, -1, -1)
#define TEST3(label, points, mem_limit)                                        \
  INTERNAL_WOODPECKER_TEST_CASE(WOODPECKER_UNIQUE_NAME(woodpecker_test),       \
                                label, points, mem_limit, -1)
#define TEST4(label, points, mem_limit, stack_limit)                           \
  INTERNAL_WOODPECKER_TEST_CASE(WOODPECKER_UNIQUE_NAME(woodpecker_test),       \
                                label, points, mem_limit, stack_limit)
#define TEST(...)                                                              \
  /* suppress warning in CLion */                                              \
  /* NOLINTNEXTLINE(cert-err58-cpp): AutoReg shouldn't throw */                \
  OVERLOADIFY(__VA_ARGS__, TEST4, TEST3, TEST2, DUMMY)(__VA_ARGS__)

#define CHECK_EQ(val, expr)                                                    \
  do {                                                                         \
    if (cntr.ok) {                                                             \
      cntr.ok = (val) == (expr);                                               \
      if (!cntr.ok) {                                                          \
        cntr.woodpecker_cntr << ">>> " << #val << " == " << #expr              \
                             << " failed at " << __LINE__                      \
                             << ". Expr result: " << (expr) << ".\n";          \
      }                                                                        \
    }                                                                          \
  } while (false)

#define CHECK_CLOSE_TO(val, expr, relative_tolerance)                          \
  do {                                                                         \
    if (cntr.ok) {                                                             \
      auto actual = (expr);                                                    \
      const double error = std::abs((val)-actual);                             \
      constexpr double ABSOLUTE_TOLERANCE =                                    \
          std::numeric_limits<decltype(actual)>::epsilon() * 100;              \
      /* Similarly to Numpy's `allclose` */                                    \
      cntr.ok = error <= ABSOLUTE_TOLERANCE + (relative_tolerance) * (val);    \
      if (!cntr.ok) {                                                          \
        cntr.woodpecker_cntr                                                   \
            << ">>> " << #val << " == " << #expr << " failed at " << __LINE__  \
            << ". Actual result is " << actual << " instead of " << (val)      \
            << " (with tolerance " << (relative_tolerance) << ").\n";          \
      }                                                                        \
    }                                                                          \
  } while (false)

#define CHECK_EXC(exc, code)                                                   \
  do {                                                                         \
    if (cntr.ok) {                                                             \
      try {                                                                    \
        (void)(code);                                                          \
        cntr.ok = false;                                                       \
        cntr.woodpecker_cntr << ">>> expected exception\n";                    \
      } catch (exc&) { /* NOLINT(bugprone-macro-parentheses) */                \
      }                                                                        \
      if (!cntr.ok) {                                                          \
        cntr.woodpecker_cntr << #code << " failed at" << __LINE__ << "\n";     \
      }                                                                        \
    }                                                                          \
  } while (false)

#define CHECK_NOEXC(code)                                                      \
  do {                                                                         \
    if (cntr.ok) {                                                             \
      try {                                                                    \
        (void)(code);                                                          \
        cntr.ok = true;                                                        \
      } catch (std::exception & e) {                                           \
        cntr.woodpecker_cntr << ">>> " << #code                                \
                             << " throws unexpected exception at " << __LINE__ \
                             << " unexpected exception: " << e.what() << "\n"; \
        cntr.ok = false;                                                       \
      }                                                                        \
      if (!cntr.ok) {                                                          \
        cntr.woodpecker_cntr << #code << " failed at" << __LINE__ << "\n";     \
      }                                                                        \
    }                                                                          \
  } while (false)

#define MEASURE(label, timeout)                                                \
  using namespace std::chrono_literals;                                        \
  if (woodpecker::measurement_controller mcntr(                                \
          label,                                                               \
          std::chrono::duration_cast<std::chrono::microseconds>(timeout),      \
          cntr.woodpecker_cntr, cntr);                                         \
      mcntr.run)

#define WOODPECKER_INIT                                                        \
  woodpecker::woodpecker_controller woodpecker_cntr(                           \
      {argv, static_cast<std::span<const char *>::size_type>(argc)})

#define WOODPECKER_RUN_TESTS woodpecker_cntr.run_tests()

#define WOODPECKER_MAIN(mem_limit_, stack_limit_)                              \
  int main(int argc, const char *argv[]) {                                     \
    WOODPECKER_INIT;                                                           \
    woodpecker_cntr.default_params.mem_limit = mem_limit_;                     \
    woodpecker_cntr.default_params.stack_limit = stack_limit_;                 \
    WOODPECKER_RUN_TESTS;                                                      \
    return 0;                                                                  \
  }

#define WOODPECKER_TEST_MAIN(mem_limit_, stack_limit_)                         \
  int main(int argc, const char *argv[]) {                                     \
    WOODPECKER_INIT;                                                           \
    woodpecker_cntr.default_params.mem_limit = mem_limit_;                     \
    woodpecker_cntr.default_params.stack_limit = stack_limit_;                 \
    WOODPECKER_RUN_TESTS;                                                      \
    return woodpecker_cntr.maximum == woodpecker_cntr.total ? 0 : 1;           \
  }
#endif /* ifndef WOODPECKER_HPP */
