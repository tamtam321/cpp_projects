// your solution should be in problem1.hpp and problem2.hpp
#include "fixedSizePool.hpp"
#include "dynamicPool.hpp"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <chrono>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <list>
#include <algorithm>
//____________________WOODPECKER____________________
namespace woodpecker {
void usage(const char* name)
{
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

struct woodpecker_controller {
    int test_no = 0; /**< id of the test that will be runned, 0 to run all tests */
    int counter = 0; /**< test counter used in macros */
    int total = 0; /**< total points achieved */
    std::ofstream test_log; /**< logfile to write test results */
    bool verbose = false; /**< print logs to stderr */
    bool config_mode = false; /**< output test configs instead of running them*/
    woodpecker_controller(int argc, const char* argv[])
    {
        int arg_idx = 1;
        while (arg_idx < argc) {
            if (!strcmp(argv[arg_idx], "-v")) {
                verbose = true;
            }
            else if (!strcmp(argv[arg_idx], "-t")) {
                test_no = std::atoi(argv[++arg_idx]);
            }
            else if (!strcmp(argv[arg_idx], "-f")) {
                test_log.open(argv[++arg_idx]);
            }
            else if (!strcmp(argv[arg_idx], "-c")) {
                config_mode = true;
            }
            else if (!strcmp(argv[arg_idx], "--")) {
                break;
            }
            else {
                usage(argv[0]);
                exit(2);
            }
            arg_idx++;
        }
        if (!test_log.is_open() && !verbose) {
            verbose = true;
        }
    }

    ~woodpecker_controller()
    {
        if (!config_mode) {
            *this << ">>> TOTAL POINTS: " << total << "\n";
        }
        if (test_log.is_open()) {
            test_log.close();
        }
    }

    template <typename T>
    woodpecker_controller& operator<<(const T& printable)
    {
        if (test_log.is_open())
            test_log << printable;
        if (verbose)
            std::cerr << printable;
        return *this;
    }
};

struct test_controller {
    bool enabled;
    std::string label;
    int points;
    bool ok;
    int& out;
    woodpecker_controller& woodpecker_cntr;
    test_controller(bool enabled, std::string&& label, int points, int& out,
        woodpecker_controller& woodpecker_cntr)
        : enabled(enabled)
        , label(std::move(label))
        , points(points)
        , ok(true)
        , out(out)
        , woodpecker_cntr(woodpecker_cntr)
    {
        if (woodpecker_cntr.config_mode) {
            this->enabled = false;
            // Output timeout, memory limit, etc.
            // Just the name and points for now.
            woodpecker_cntr << this->points << ";" << this->label << "\n";
        }
        if (this->enabled) {
            woodpecker_cntr << ">>> Test: " << this->label << " (" << points << ")\n";
        }
    }
    ~test_controller()
    {
        if (enabled) {
            out += ok ? points : 0;
            #ifdef _WIN32
                woodpecker_cntr << ">>> Result: " << (ok ? "PASS" : "FAIL") << "\n";
            #else
                woodpecker_cntr << ">>> Result: " << (ok ? "\033[1;32mPASS\033[0m" : "\033[1;31mFAIL\033[0m") << "\n";
            #endif
        }
    }
};

struct measurement_controller {
    std::string name;
    std::chrono::nanoseconds& out;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    bool run;
    woodpecker_controller& woodpecker_cntr;
    measurement_controller(std::string&& name, std::chrono::nanoseconds& out,
        woodpecker_controller& woodpecker_cntr)
        : name(std::move(name))
        , out(out)
        , start(std::chrono::high_resolution_clock::now())
        , run(true)
        , woodpecker_cntr(woodpecker_cntr)
    {
    }
    ~measurement_controller()
    {
        auto stop = std::chrono::high_resolution_clock::now();
        out = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        woodpecker_cntr << name << ": " << out.count() << "ns\n";
    }
};

} // end namespace woodpecker

#define TEST(label, points)                                                                       \
    if (woodpecker::test_controller cntr(                                                         \
            ++woodpecker_cntr.counter == woodpecker_cntr.test_no || woodpecker_cntr.test_no == 0, \
            label, points, woodpecker_cntr.total, woodpecker_cntr);                               \
        cntr.enabled)

#define CHECK_EQ(val, expr)                                                       \
    if (cntr.ok) {                                                                \
        try {                                                                     \
            cntr.ok = val == (expr);                                              \
        }                                                                         \
        catch (...) {                                                             \
            cntr.ok = false;                                                      \
            woodpecker_cntr << ">>> unexpected exception\n";                      \
        }                                                                         \
        if (!cntr.ok) {                                                           \
            woodpecker_cntr << ">>> " << #val << " == " << #expr << " failed at " \
                            << __LINE__ << " Expected result is " << (expr) << " instead of " << val << "\n";    \
        }                                                                         \
    }

#define CHECK_NOEQ(val, expr)                                                       \
    if (cntr.ok) {                                                                \
        try {                                                                     \
            cntr.ok = val != (expr);                                              \
        }                                                                         \
        catch (...) {                                                             \
            cntr.ok = false;                                                      \
            woodpecker_cntr << ">>> unexpected exception\n";                      \
        }                                                                         \
        if (!cntr.ok) {                                                           \
            woodpecker_cntr << ">>> " << #val << " == " << #expr << " failed at " \
                            << __LINE__ << " Expr result: " << (expr) << "\n";    \
        }                                                                         \
    }

#define CHECK_EXC(exc, code)                                              \
    if (cntr.ok) {                                                        \
        try {                                                             \
            code;                                                         \
            cntr.ok = false;                                              \
            woodpecker_cntr << ">>> expected exception\n";                \
        }                                                                 \
        catch (const exc&) {                                                     \
        }                                                                 \
        catch (...) {                                                     \
            woodpecker_cntr << ">>> unexpected exception\n";              \
            cntr.ok = false;                                              \
        }                                                                 \
        if (!cntr.ok) {                                                   \
            woodpecker_cntr << #code << " failed at" << __LINE__ << "\n"; \
        }                                                                 \
    }

#define CHECK_NOEXC(code)                                                 \
    if (cntr.ok) {                                                        \
        try {                                                             \
            code;                                                         \
            cntr.ok = true;                                               \
        }                                                                 \
        catch (...) {                                                     \
            woodpecker_cntr << ">>> unexpected exception\n";              \
            cntr.ok = false;                                              \
        }                                                                 \
        if (!cntr.ok) {                                                   \
            woodpecker_cntr << #code << " failed at" << __LINE__ << "\n"; \
        }                                                                 \
    }

#define MEASURE(label, var)                                                    \
    std::chrono::nanoseconds var;                                              \
    if (woodpecker::measurement_controller mcntr(label, var, woodpecker_cntr); \
        mcntr.run)

#define WOODPECKER_INIT \
    woodpecker::woodpecker_controller woodpecker_cntr(argc, argv)
//____________________WOODPECKER_END____________________

class abstractTester {
public:
    static constexpr unsigned N = 10;
    std::vector<long*> allocated;

    long sum() const {
        long ret = 0;
        for(auto tmp : allocated) {
            ret += *tmp;
        }
        return ret;
    }
};

class fixedTester : public abstractTester {
public :
    FixedSizePool<long, N> a;

    fixedTester() : abstractTester() {
        allocated.reserve(N);
    }

    void fillFull() {
        for (unsigned i = 0; i < N; ++i) {
            allocated.push_back(a.allocate());
            *allocated.back() = static_cast<long>(i);
        }
    }
};

class DynamicTester : public abstractTester {
public:
    DynamicPool<long, N> a;

    void fillOne() {
        for(unsigned i = 0; i < N; ++i) {
            allocated.push_back(a.allocate());
            *allocated.back() = static_cast<long>(i);
        }
    }
};

int main(int argc, const char* argv[]) {
    WOODPECKER_INIT;

    /*TEST("Syntax check fixed", 0) {
        FixedSizePool<std::pair<short, short>, 1> q;
        auto k = q.construct(std::make_pair(1,2));
        CHECK_EQ(k->first, 1);
        CHECK_EQ(k->second, 2);
        q.deallocate(k);
    }*/

    long* p = nullptr;
    TEST("Valid allocation", 5) {
        fixedTester f;
        CHECK_NOEXC(f.fillFull());
        auto it = f.allocated.begin();
        for (unsigned i = 0; i < fixedTester::N; ++i) {
            CHECK_EQ(static_cast<long>(i), **it++);
        }
        std::sort(f.allocated.begin(), f.allocated.end());
        it = std::unique(f.allocated.begin(), f.allocated.end());
        CHECK_EQ((it == f.allocated.end()), true);
    }

    TEST("Good throw allocation", 5) {
        fixedTester f;
        CHECK_NOEXC(f.fillFull());
        CHECK_EXC(std::bad_alloc, p = f.a.allocate());
    }

    TEST("Allocation N", 5) {
        fixedTester f;
        long* tmp[5]{nullptr};
        CHECK_NOEXC(tmp[0] = f.a.allocate(5));
        CHECK_NOEXC(tmp[1] = f.a.allocate());
        CHECK_NOEXC(tmp[2] = f.a.allocate(1));
        CHECK_NOEXC(tmp[3] = f.a.allocate());
        CHECK_NOEXC(tmp[4] = f.a.allocate(2));
        p = *std::min_element(&tmp[0], &tmp[4]);
        for(unsigned i = 0; i < 10; ++i) {
            p[i] = i;
        }
    }

    TEST("Allocation N throw", 5) {
        fixedTester f;
        CHECK_EXC(std::bad_alloc, p = f.a.allocate(fixedTester::N + 1));
        fixedTester f2;
        p = f2.a.allocate();
        CHECK_EXC(std::bad_alloc, p = f2.a.allocate(fixedTester::N));
    }

    TEST("Deallocation simple", 5) {
        fixedTester f;
        CHECK_NOEXC(f.fillFull());
        auto tmp = f.allocated.front();
        f.a.deallocate(f.allocated.front());
        p = f.a.allocate();
        CHECK_EQ(tmp, p);
        CHECK_EXC(std::bad_alloc, p = f.a.allocate());
    }

    TEST("Deallocation - Re-allocation Complicated", 15) { 
        fixedTester f;
        CHECK_NOEXC(f.fillFull());
        auto it = f.allocated.begin();
        f.a.deallocate(*it);
        it = f.allocated.erase(it);
        f.a.deallocate(*(++it));
        it = f.allocated.erase(it);
        f.a.deallocate(*(++it));
        f.allocated.erase(it);
        CHECK_EXC(std::bad_alloc, p = f.a.allocate(3));
        CHECK_NOEXC(p = f.a.allocate());
        f.allocated.push_back(p);
        *p = 0;
        CHECK_NOEXC(p = f.a.allocate());
        f.allocated.push_back(p);
        *p = 2;
        CHECK_NOEXC(p = f.a.allocate());
        f.allocated.push_back(p);
        *p = 4;
        CHECK_EQ(f.sum(), 45);
    }

    TEST("Deallocate N", 10) {
        fixedTester f;
        CHECK_NOEXC(p = f.a.allocate(5));
        f.a.deallocate(p, 5);
        CHECK_NOEXC(f.fillFull());
        std::vector<long> tmp{*f.allocated[1], *(f.allocated[1] + 1), *(f.allocated[1] + 2)};
        f.a.deallocate(f.allocated[1], 3);
        for(unsigned i = 0; i < 3; ++i) {
            CHECK_NOEXC(p = f.a.allocate());
            *p = tmp[i];
        }
        CHECK_EQ(f.sum(), 45);
        fixedTester g;
        g.fillFull();
        g.a.deallocate(g.allocated[2], 3);
        g.a.deallocate(g.allocated[8]);
        try {
            p = g.a.allocate(3);
            *p++ = 10;
            *p++ = 20;
            *p++ = 30;
            unsigned sum = 0;
            for(unsigned i = 0; i < 10; ++i) {
                if(i != 8) {
                    sum += *g.allocated[i];
                }
            }
            CHECK_EQ(sum, 88);
        } catch(const std::bad_alloc& e) {}
        CHECK_EXC(std::bad_alloc ,p = g.a.allocate(3));
    }

    TEST("Construct", 10) {
        fixedTester f;
        for(unsigned i = 0; i < fixedTester::N; ++i) {
            f.allocated.push_back(f.a.construct(i));
        }
        auto it = f.allocated.begin();
        for (unsigned i = 0; i < fixedTester::N; ++i) {
            CHECK_EQ(static_cast<long>(i), **it++);
        }
        std::sort(f.allocated.begin(), f.allocated.end());
        it = std::unique(f.allocated.begin(), f.allocated.end());
        CHECK_EQ((it == f.allocated.end()), true);
        CHECK_EXC(std::bad_alloc, p = f.a.construct(5));
    }

    //Dynamic allocators:

    /*TEST("Syntax check dynamic", 0) {
        DynamicPool<std::pair<short, short>, 1> w;
        auto k = w.construct(std::make_pair(1,2));
        CHECK_EQ(k->first, 1);
        CHECK_EQ(k->second, 2);
        w.deallocate(k);
    }*/

    TEST("Dynamic Allocate", 2) {
        DynamicTester d;
        CHECK_NOEXC(d.fillOne());
        CHECK_NOEXC(p = d.a.allocate());
        std::sort(d.allocated.begin(), d.allocated.end());
        CHECK_EQ((std::abs(d.allocated.front() - p) > 10), true);
    }

    /*TEST("Dynamic Deallocation - simple Re-allocation", 4) {
        DynamicTester d;
        CHECK_NOEXC(d.fillOne());
        d.a.deallocate(d.allocated[3]);
        CHECK_EQ(d.a.allocate(), d.allocated[3]);
    }

    TEST("Allocate N", 4) {
        DynamicTester d;
        CHECK_NOEXC(p = d.a.allocate());
        auto tmp = d.a.allocate(10);
        CHECK_EQ((std::abs(tmp - p) > 10), true);
        tmp = d.a.allocate(9);
        CHECK_EQ((std::abs(tmp - p) < 10), true);
        CHECK_EXC(std::bad_alloc, p = d.a.allocate(11));
    }

    TEST("Dynamic Deallocation - Re-allocation", 7) {
        DynamicTester d;
        CHECK_NOEXC(d.fillOne());
        CHECK_NOEXC(d.fillOne());
        CHECK_NOEXC(d.fillOne());
        long* tmp[4]{d.allocated[5], d.allocated[12], d.allocated[14], d.allocated[29]};
        long values[4]{5, 2, 4, 9};
        for(auto p : tmp) {
            d.a.deallocate(p);
        }
        for(unsigned i = 0; i < 4; ++i) {
            p = d.a.allocate();
            CHECK_NOEQ(std::find(&tmp[0], &tmp[4], p), &tmp[4]);
            *p = values[i];
        }
        CHECK_EQ(d.sum(), 135);
        d.a.deallocate(*std::min(d.allocated.begin(), d.allocated.end()));
    }

    TEST("Dynamic deallocate N", 4) {
        DynamicTester d;
        CHECK_NOEXC(d.fillOne());
        CHECK_NOEXC(d.fillOne());
        d.a.deallocate(d.allocated[5], 4);
        d.a.deallocate(d.allocated[12], 3);
        for(unsigned i = 0; i < 7; ++i) {
            p = d.a.allocate();
            d.allocated.push_back(p);
        }
        std::sort(d.allocated.begin(), d.allocated.end());
        CHECK_EQ((std::unique(d.allocated.begin(), d.allocated.end()) - d.allocated.begin()), 20);
    }

    TEST("Dynamic construct", 4) {
        DynamicTester d;
        for(unsigned i = 0; i < fixedTester::N; ++i) {
            CHECK_NOEXC(d.allocated.push_back(d.a.construct(i)));
        }
        CHECK_NOEXC(p = d.a.construct(11));
        std::sort(d.allocated.begin(), d.allocated.end());
        CHECK_EQ((std::abs(d.allocated.front() - p) > 10), true);
        d.allocated.push_back(p);
        CHECK_EQ(d.sum(), 56);

        p = d.a.allocate(9);
        auto tmp = *d.allocated[3];
        d.a.deallocate(d.allocated[3]);
        p = d.a.construct(tmp + 2);
        CHECK_EQ(d.sum(), 58);
    }*/

    //std::vector<std::pair<short, short>, DynamicPool<std::pair<short, short>, 10>>vstring{{1, 2}};
    //std::cout << vstring[0].first << vstring[0].second<< std::endl;
    //std::list<int, DynamicPool<int, 10>> l;
    //l.push_back(5);
    //std::cout << l.front() << " - Valoszinuleg szereztel +20 pontot, ha ezt latod" << std::endl;

    return EXIT_SUCCESS;
}
