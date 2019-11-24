#include <random>
#include <fstream>
#include <set>
#include <algorithm>
#include "../src/Timeseries.hh"
#include "../src/Solver.hh"
#include "common_test.hh"

static std::random_device _rd;
static std::ranlux24 global_random_engine(_rd());


namespace test
{
bool TimeseriesGen::run()
{
    using T = src::Timeseries::Time_t;
    using V = src::Timeseries::Value_t;

    /* initialize error range */
    static T epsilon = 0.01; // 10 ms
    static const double RATIO = 0.7; // sizeof(small) / sizeof(large)
    std::uniform_real_distribution<T> dis_err(-epsilon / RATIO, epsilon / RATIO);

    /* initialize value pool */
    // value set should be smaller than time set
    std::cerr<<"Generating input data pool ... time:";
    int valsize = std::floor(std::sqrt(std::sqrt(this->len)));
    valsize = std::max(valsize, 5);
    std::set<T> timeset;
    std::vector<V> valueset(valsize);
    std::uniform_real_distribution<T> dis_r(tl, th);
    std::uniform_int_distribution<V> dis_i(vl, vh);
    for(unsigned i=0;i<this->len;i++)
    {
        T v = 0;
        do{
            v = dis_r(global_random_engine);
        }while(timeset.count(v) > 0);
        timeset.insert(v);
    }
    std::cerr<<" Done, value: ";
    for(auto &v : valueset) v = dis_i(global_random_engine);
    std::cerr<<"Done!"<<std::endl;

    /* draw values from valpool */
    std::cerr<<"generatin data from pool..."<<std::endl;
    //std::map<T, V> largedata, smalldata;
    src::Timeseries largedata, smalldata;
    std::uniform_int_distribution<int> dis_idx(0, valsize);
    for(auto time : timeset)    // for large timeseries
    {
        auto idx = dis_idx(global_random_engine);
        largedata.insert(time, valueset[idx]);
    }
    
    for(auto ent : largedata)
    {
        auto err = dis_err(global_random_engine);
        if(std::fabs(err) < epsilon)
            smalldata.insert(ent.first + err + DELTA_T, ent.second);
    }

    /**
     * print to file, file names:
     *  large file: ${prefix}-large.ts
     *  samll file: ${prefix}-small.ts
     */
    std::cerr<<"Print the results into file..."<<std::endl;
    std::string largef = prefix + "large.ts",
                smallf = prefix + "small.ts";
    std::ofstream fout_l(largef), fout_s(smallf);
    fout_l.setf(std::ios::fixed);
    fout_l.precision(8);
    fout_s.setf(std::ios::fixed);
    fout_s.precision(8);
    for(auto ent : largedata) fout_l<<ent.first<<" "<<ent.second<<std::endl;
    for(auto ent : smalldata) fout_s<<ent.first<<" "<<ent.second<<std::endl;
    return true;
}

bool TimeseriesTest::run()
{
    using T = src::Timeseries::Time_t;
    using V = src::Timeseries::Value_t;

    src::Timeseries ts1{src::TimeseriesReader::ReadTwoCols(this->fname)};
    auto data{ts1.getData()};
    ASSERT_EQUAL(data.size(), ts1.size());

    /* getting histogram */
    std::map<V, int> histo;
    for(const auto &val : ts1.getValueSet())
        histo[val]++;
    std::cerr<<"Histogram result for file: "<<this->fname<<std::endl;
    for(auto ent : histo)
        std::cerr<<"  "<<ent.first<<" "<<ent.second<<std::endl;
    return true;
}

bool SolverTest::run()
{
    src::Timeseries t1(src::TimeseriesReader::ReadTwoCols(this->fname));
    src::Timeseries t2(src::TimeseriesReader::ReadTwoCols(this->fname));
    std::cerr<<"got size: "<<t1.size()<<std::endl;
    src::SolverBase *sv = new src::BruteForce(0.01);
    // timeseries 1 and 2 should be equal
    ASSERT(sv->check(t1, t1, 0, 0.01));
    ASSERT(sv->check(t2, t2, 0, 0.01));
    ASSERT(sv->check(t1, t2, 0, 0.01));
    ASSERT(sv->check(t2, t1, 0, 0.01));
    
    ASSERT(!sv->check(t1, t1, 1, 0.01));
    delete sv;
    return true;
}

bool BruteForceTest::run()
{
    src::Timeseries t1(src::TimeseriesReader::ReadTwoCols(this->file_small));
    src::Timeseries t2(src::TimeseriesReader::ReadTwoCols(this->file_large));
    src::SolverBase *sv = new src::BruteForce(1);
    ASSERT(sv->check(t1, t2, -10, 0.1));
    double res = sv->solve(t1, t2);
    std::cerr<<this->getName()<<": got res: "<<res<<std::endl;
    ASSERT((result - res < 1e-3));
    return true;
}

} // namespace test
