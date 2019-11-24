#include "test/Test.hh"
#include "test/common_test.hh"

using namespace test;
class Test1 : public Test
{
    public:
        virtual std::string getName() const override
        {
            return "test 1 (should pass)";
        }

        virtual bool run() override
        {
            return true;
        }
};

void inputgen(int size)
{
    std::cerr<<"Generating input!"<<std::endl;
    Tester t;
    t.emplace<TimeseriesGen>("data/test", 0, 180, 0, 1412, size);
    t.start();
    std::cerr<<"Done!"<<std::endl;
}

int main(int argc, char *argv[])
{
    inputgen(2048);
    Tester t;
    t.emplace<TimeseriesTest>("data/testlarge.ts");
    t.emplace<TimeseriesTest>("data/testsmall.ts");
    t.emplace<SolverTest>("data/testsmall.ts");
    t.emplace<BruteForceTest>("data/testsmall.ts", "data/testlarge.ts",
            -10.);
    t.start();
    return 0;
}
