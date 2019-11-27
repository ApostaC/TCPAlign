#ifndef _COMMON_TEST_HH_
#define _COMMON_TEST_HH_

#include "Test.hh"
namespace test
{

class TimeseriesGen;
class TimestampTest;
class TimeseriesTest;
class SolverTest;
class BruteForceTest;

class TimeseriesGen :public Test
{
    public:
        static const int DELTA_T = 10;
    private:
        std::string prefix;
        double tl, th;
        size_t vl, vh;
        size_t len;
    public:
        TimeseriesGen(const std::string &pre, double tlow, double thigh,
                size_t vlow, size_t vhigh, size_t length = 32768)
            : prefix(pre), tl(tlow), th(thigh), vl(vlow), vh(vhigh), len(length) {}

        virtual std::string getName() const override
        {
            return "Timeseries generator";
        }

        virtual bool run() override;
};

class TimestampTest : public Test
{
    public:
        virtual std::string getName() const override
        {
            return "Test Timestamp struct";
        }

        virtual bool run() override;
};

class TimeseriesTest : public Test
{
    private:
        std::string fname;
    public:
        TimeseriesTest(const std::string &f) : fname(f) {}
        virtual std::string getName() const override
        {
            return "Testing timeseries";
        }

        virtual bool run() override;
};

/* test the function: solver::check */
class SolverTest : public Test
{
    private:
        std::string fname;
    public:
        SolverTest(const std::string &n) : fname(n) {}
        virtual std::string getName() const override
        {
            return "Testing Solver::check";
        }

        /**
         * Note: this method is implemented in src/Solver.cc
         * because it will use the private function of the 
         * Solver
         */
        virtual bool run() override;
};

class BruteForceTest : public Test
{
    private:
        std::string file_small;
        std::string file_large;
        double result;
    public:
        BruteForceTest(const std::string &sf, const std::string &lf, double res)
            : file_small(sf), file_large(lf), result(res){}
        virtual std::string getName() const override
        {
            return "Brute Force test";
        }

        virtual bool run() override;
};

} // namespace test
#endif
