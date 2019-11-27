#ifndef _TIMESERIES_HH_
#define _TIMESERIES_HH_

#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>


namespace src
{

class Timestamp;
class Timeseries;
class TimeseriesReader;

} //namespace src

namespace std
{
std::string to_string(const src::Timestamp &ts);
std::ofstream &operator<<(std::ofstream &o, const src::Timestamp &ts);
}

namespace src
{
class Timestamp
{
    public:
        constexpr static int64_t USEC_SCALE = 100000000;
    public:
        //constexpr static Timestamp NAN{0};
    public:
        int64_t sec;
        int64_t usec;    // real time usec = this->usec / USEC_SCALE

    public:
        /* constructors */
        constexpr Timestamp() : sec(0), usec(0) {}
        constexpr Timestamp(int64_t s, int64_t us) : sec(s), usec(us) {}
        Timestamp(double time) { sec = std::floor(time); usec = (int)((time - sec) * USEC_SCALE); }

        /* bool and assign operator */
        bool operator<(const Timestamp &r) const { if(sec == r.sec) return usec < r.usec;  return sec < r.sec; }
        bool operator==(const Timestamp &r) const{ return sec == r.sec && usec == r.usec; }
        Timestamp &operator=(const Timestamp &r) { sec = r.sec; usec = r.usec; return *this; }

        /* operators with Timestamp */
        Timestamp &operator+=(const Timestamp &r) {sec += r.sec; usec += r.usec; if(usec > USEC_SCALE) {usec -= USEC_SCALE; sec += 1; } return *this;}
        Timestamp &operator-=(const Timestamp &r) {sec -= r.sec; usec -= r.usec; if(usec < 0) {usec += USEC_SCALE; sec -= 1; } return *this;}
        Timestamp operator+(const Timestamp &r) const {Timestamp ret{*this}; ret += r; return ret; }
        Timestamp operator-(const Timestamp &r) const {Timestamp ret{*this}; ret -= r; return ret; }

        /* operators with double */
        Timestamp &operator+=(double t) {Timestamp r(t); sec += r.sec; usec += r.usec; return *this;}
        Timestamp &operator-=(double t) {Timestamp r(t); sec -= r.sec; usec -= r.usec; return *this;}
        Timestamp &operator/=(double t) 
        {
            if (sec % 2 != 0) {sec -= 1; usec += USEC_SCALE; }
            sec /= t; usec /= t; return *this;
        }
        Timestamp operator+(double t) const {Timestamp ret{*this}; ret += Timestamp(t); return ret; }
        Timestamp operator-(double t) const {Timestamp ret{*this}; ret -= Timestamp(t); return ret; }
        Timestamp operator/(double t) const {Timestamp ret{*this}; ret /= t; return ret;}

        /* convert to double */
        operator double() const {return sec + usec * 1. / USEC_SCALE;}

        std::string to_string() const; 
};


class Timeseries
{
    public:
        using Time_t    = Timestamp;
        using Value_t   = size_t; 
        using Time_Set_t    = std::vector<Time_t>;
        using Value_Set_t   = std::vector<Value_t>;

    private:
        using Data_t         = std::map<Time_t, Value_t>;

    public:
        using iterator       = Data_t::iterator;
        using const_iterator = Data_t::const_iterator;

    private:
        Data_t data_;
        
    public:
        /* constructors */
        Timeseries() = default;
        Timeseries(Timeseries &&other) noexcept
            : data_(other.data_) {}

        /**
         * getTimeSet and getValueSet
         * get T and S from this timeseries
         */
        Time_Set_t  getTimeSet() const;
        Value_Set_t getValueSet() const;

        const Data_t & getData() const {return this->data_;}
        
        /* access the timeseries */
        iterator    begin() { return data_.begin(); }
        iterator    end() { return data_.end(); }
        const_iterator  begin() const { return data_.begin(); }
        const_iterator  end() const { return data_.end(); }
        size_t      size() { return data_.size(); }

        /* clear the timeseries */
        Timeseries &    clear() {data_.clear(); return *this;}

        /**
         * insert and insertBatch
         * modify the timeseries -> add new values into it
         */
        Timeseries &    insert(Time_t time, Value_t value) 
        { 
            static Time_t eps{1e-8};
            while(data_.count(time))
            {
                auto newtime = time + eps;
                std::cerr<<"Warning: Timeseries::insert: duplicate key! time: " + std::to_string(time) + ", modify the time to " + std::to_string(newtime)<<std::endl;
                time = newtime;
            }
            data_.insert({time, value}); return *this;
        }
        Timeseries &    insertBatch(const Time_Set_t &vt, const Value_Set_t &vs)
        {
            if(vt.size() != vs.size()) throw std::runtime_error("Timeseries::insertBatch: input length not equal!");
            for(unsigned i=0;i<vt.size();i++)
                this->insert(vt[i], vs[i]);
            return *this;
        }

};

/**
 * class TimeseriesReader
 * Read the timeseries data from file and get a timeseries
 */
class TimeseriesReader
{
    public:
        using ColPair_t     = std::pair<int, int>;
        using ColPairList_t = std::vector<ColPair_t>;
    private:
        using T = Timeseries::Time_t;
        using V = Timeseries::Value_t;
    public:
        /**
         * static method: ReadTwoCols
         * read the timeseries from file with 2 columns
         *  col1: timestamp
         *  col2: value
         */
        static Timeseries ReadTwoCols(const std::string &, const char delim = ' ');

        /**
         * static method: ReadByColID
         * read the timeseries from file with columnID specified by user
         */
        static Timeseries ReadByColId(const std::string &, const int, const int, const char delim = ' '); 

        /**
         * static method: ReadByColIds
         * read the timeseries from file with mutiple columnID pairs
         * while only open the file once
         */
        static std::vector<Timeseries> ReadByColIds(const std::string &, const ColPairList_t &, const char delim = ' ');
};

} // namespace src

#endif
