#include <numeric>
#include <iostream>
#include <algorithm>
#include "Timeseries.hh"
#include "common.hh"

namespace src
{

std::string Timestamp::to_string() const
{
    auto outusec = usec;
    auto outsec = sec;
    if(sec < 0) outusec = USEC_SCALE - usec;
    if(sec < 0 && outusec > 0) outsec = sec + 1;

    int64_t temp = outusec;
    std::string zeros{};
    while(temp && temp * 10 < USEC_SCALE)
    {
        temp *= 10;
        zeros += "0";
    }
    return std::to_string(outsec) + "." + zeros + std::to_string(outusec); 
}

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

namespace std
{

std::string to_string(const src::Timestamp &ts)
{
    return ts.to_string();
}

std::ofstream &operator<<(std::ofstream &o, const src::Timestamp &ts)
{
    o<<ts.to_string();
    return o;
}

} // namespace std
