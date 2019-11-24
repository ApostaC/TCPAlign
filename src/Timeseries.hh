#ifndef _TIMESERIES_HH_
#define _TIMESERIES_HH_

#include <map>
#include <vector>
#include <string>

namespace src
{

class Timeseries;
class TimeseriesReader;

class Timeseries
{
    public:
        using Time_t    = double;
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
            if(data_.count(time))
                throw std::runtime_error("Timeseries::insert: duplicate key! time: " + std::to_string(time));
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
