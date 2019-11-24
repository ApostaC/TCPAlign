#include <numeric>
#include <iostream>
#include <algorithm>
#include "Timeseries.hh"
#include "common.hh"

namespace src
{

Timeseries::Time_Set_t Timeseries::getTimeSet() const
{
    Time_Set_t ret;
    std::transform(data_.begin(), data_.end(), std::back_inserter(ret),
            [](auto &ent){return ent.first;});
    return ret;
}

Timeseries::Value_Set_t Timeseries::getValueSet() const
{
    Value_Set_t ret;
    std::transform(data_.begin(), data_.end(), std::back_inserter(ret),
            [](auto &ent){return ent.second;});
    return ret;
}

Timeseries TimeseriesReader::ReadTwoCols(const std::string &fname, const char delim)
{

    Timeseries ret;
    src::input_helper helper(fname, delim);
    while(helper.hasNext())
    {
        auto vec = helper.next();
        if(vec.size() == 0) continue;
        if(vec.size() < 2) 
        {
            std::cerr<<"Error occured in TimeseriesReader::ReadTwoCols"<<std::endl;
            for(auto v:vec) std::cerr<<v<<" "; std::cerr<<std::endl;
            throw std::runtime_error("TimeseriesReader::ReadTwoCols: need at least 2 column but only got 1 or 0");
        }
        T time = std::stod(vec[0]);
        V value= std::stoll(vec[1]);
        ret.insert(time, value);
    }
    return ret;
}

Timeseries TimeseriesReader::ReadByColId(const std::string &fname,
        const int tcol, const int vcol, const char delim)
{
    Timeseries ret;
    src::input_helper helper(fname, delim);
    unsigned maxv = std::max(tcol, vcol);
    while(helper.hasNext())
    {
        auto vec = helper.next();
        if(vec.size() < maxv) continue;
        T time = std::stod(vec[tcol]);
        V value= std::stoll(vec[vcol]);
        ret.insert(time, value);
    }
    return ret;
}

} // namespace src
