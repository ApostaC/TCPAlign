#ifndef _SOLVER_HH_
#define _SOLVER_HH_
#include <cmath>
#include "Timeseries.hh"

namespace test
{
class SolverTest; // export the classname here
class BruteForceTest;
} // namespace test

namespace src
{

class SolverBase;
class BruteForce;

/**
 * class SolverBase
 * the base of all solvers
 */
class SolverBase
{
    friend class test::SolverTest;
    friend class test::BruteForceTest;
    protected:
        using T = Timeseries::Time_t;
        using V = Timeseries::Value_t;
        using Tlist = Timeseries::Time_Set_t;
        using Vlist = Timeseries::Value_Set_t;
    public:
        const static T NO_SOLUTION; // default is 0
    protected:
        T epsilon_; /* the expected error range */

        /**
         * method check
         * check if the delta_t is valid using epsilon
		 * small + delta_t = large
         */
        bool check(const Timeseries &small, const Timeseries &large, 
                const T delta_t, const T eps) const;
    public:
        SolverBase(double e) : epsilon_(e) {}
        /**
         * method solve
         * solve the problem and return the delta_t
         */
        virtual double solve(const Timeseries &small, const Timeseries &large) = 0;

        virtual ~SolverBase() {}
};

/**
 * class BruteForce
 * solver using brute force method:
 * iterate among all possible delta t and find the possible solution
 * by dividing the epsilon
 */
class BruteForce : public SolverBase
{
    public:
        BruteForce(T eps) : SolverBase(eps) {}
        virtual double solve(const Timeseries &small, const Timeseries &large) override;
};

} // namespace src

#endif
