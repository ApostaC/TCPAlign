#include <iostream>
#include "src/Timeseries.hh"
#include "src/Solver.hh"

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Usage: %s <small_tcp_file> <large_lte_file>\n", argv[0]);
		return -1;
	}
    src::TimeseriesReader reader;
	std::cout<<"Reading two files..."<<std::endl;
    auto small{reader.ReadTwoCols(argv[1])};
    auto large{reader.ReadTwoCols(argv[2])};
	std::cout<<"Done!"<<std::endl;

	std::cout<<"Solve the problem..."<<std::endl;
	src::BruteForce solver(0.5);
	auto delta = solver.solve(small, large);
	std::cout<<"Done! The result is: "<<delta<<std::endl;
}
