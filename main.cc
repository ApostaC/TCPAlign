#include <iostream>
#include "src/Timeseries.hh"

int main(int argc, char *argv[])
{
    std::cout<<"Hello world!"<<std::endl;
    src::TimeseriesReader reader;
    auto ts{reader.ReadTwoCols("data/testlarge.ts")};
}
