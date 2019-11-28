#include <iostream>
#include <algorithm>
#include "Solver.hh"

namespace src
{

const SolverBase::T SolverBase::NO_SOLUTION{NAN};
bool SolverBase::check(const Timeseries &t1, const Timeseries &t2, 
        const T delta_t, const T eps) const
{
    /* t1 --> small; t2 --> large */
    const auto &data1 = t1.getData();
    const auto &data2 = t2.getData(); 
    for(auto & ent : data1)
    {
        bool flag = false;      // found value ?
        auto t = ent.first;     // get time
        auto v = ent.second;    // get value
        auto rl = t + delta_t - eps;    // time range low bound
        auto rh = t + delta_t + eps;    // time range up bound
        auto lb = data2.lower_bound(rl);    // iterator low bound
        auto ub = data2.upper_bound(rh);    // iterator up bound
        if(std::distance(lb, ub) <= 0)
		{
			std::cerr<<"MESSAGE: SolverBase::check failed because out of range: ";
            std::cerr<<"small time: "<<t.to_string()<<", delta_t: "<<delta_t
				<<", range: "<<lb->first.to_string()<<","
                <<ub->first.to_string()<<std::endl;
			return false;    // not in range
		} 
        for(auto it = lb; it!=ub; it++)
        {
            if(it->second == v)
            {
                flag = true;
                break;
            }
        }
        if(!flag) 
        {
            std::cerr<<"MESSAGE: SolverBase::check: failed because no solution: ";
            std::cerr<<"small time: "<<t.to_string()<<", delta_t: "<<delta_t
				<<", range: "<<lb->first.to_string()<<","
                <<ub->first.to_string()<<std::endl;
            return false;
        }
    }
    std::cerr<<"MESSAGE: SolverBase::check: sucessed! ";
    return true;
}

double BruteForce::solve(const Timeseries &small, const Timeseries &large)
{
    T l_eps = 0.0;
    T r_eps = this->epsilon_ * 2;

    /* iterate all possible delta_t */
    std::vector<T> possible_dt;
    const auto T2 = large.getTimeSet();
    const auto T1 = small.getTimeSet();
    std::cout<<"BruteForce::solve: sizes are: "<<T1.size()<<" "<<T2.size()<<std::endl;
    for(auto t2 : T2)
    {
        auto t1 = T1[0];
        if(std::fabs(t2 - t1) < 200)
			possible_dt.push_back(t2 - t1);
    }
    std::cout<<"BruteForce::solve: got "<<possible_dt.size()<<" possible delta_t"<<std::endl;

    /* modify the possible_dt, return the state:
     *  retval > 0: more than 1 solution, need smaller eps
     *  retval < 0: no solution, need larger eps
     *  retval == 0: good! change the solution!
     */
    auto one_try = [this, &small, &large](std::vector<T> &all_dt, T eps, T &solu)
    {
        bool finished = false;
        bool has_solu = false;
        std::vector<T> new_dt;
        for(auto delta_t : all_dt)
        {
            auto res = this->check(small, large, delta_t, eps);
            if(res)         // this dt can be solution
            {
				std::cerr<<"PASSED!! "<<all_dt.size()<<std::endl;
                new_dt.push_back(delta_t);
                if(!has_solu) solu = delta_t;
                else // already got a solution
                    finished = true;    // now finished!
                has_solu = true;
            }
        }
        all_dt = new_dt;    // update all_dt
        /**
         * has_solu = true, finished = true: more than one solution
         * has_solu = false, finished = true: invalid state
         * has_solu = true, finished = false: just one solution
         * has_solu = false, finished = false: no solution
         */
        if(has_solu && finished)
            return 1;
        if(!has_solu && finished)
            throw std::runtime_error("BruteForce::solve: invalid state in one_try(), has_solu = false, finished = true");
        if(has_solu && !finished) 
            return 0;
        if(!has_solu && !finished)
            return -1;

        // control will never reach here!
        throw std::runtime_error("BruteForce::solve: invalid state in one_try()");
    };

    /* check_possible: check if all dt in set is in 2 * eps */
    auto check_possible = [](const std::vector<T> &dt, T eps)
    {
        auto max = *std::max_element(dt.begin(), dt.end());
        auto min = *std::min_element(dt.begin(), dt.end());
        std::cerr<<", "<<dt.size()<<" solutions are very near... try to find the result "<<std::endl;
        //for(auto v:dt) std::cerr<<v<<" "; std::cerr<<std::endl;
        std::cerr<<"Max is "<<max.to_string()<<", Min is "<<min.to_string()<<", Range is "<<(max-min).to_string()<<std::endl;
        if((double)(max - min) < 2 * eps) return true;
        else return false;
    };

    int try_ret = 0;
    do
    {
        T solu = NO_SOLUTION;
        auto mid = (l_eps + r_eps)/2.;
        std::cout<<"BruteForce::solve: trying epsilon: "<<mid.to_string()
                <<", remaining: "<<possible_dt.size();
        if((double)mid < 1e-3 && check_possible(possible_dt, mid))
        {
            // return average
            T sum = 0;
            for(auto v : possible_dt) sum += v;
            return sum / (double)possible_dt.size();
        }
        try_ret = one_try(possible_dt, mid, solu);
        if(try_ret == 0)
        {
            std::cout<<" found solution: "<<solu.to_string()<<std::endl;
            return solu;
        }
        if(try_ret > 0)
        {
            std::cout<<", get more than 1 solutions"<<std::endl;
            r_eps = mid;
        }
        if(try_ret < 0)
        {
            std::cout<<", get no solution"<<std::endl;
            l_eps = mid;
        }
    }while(possible_dt.size());
    return NO_SOLUTION;
}

} // namespace src
