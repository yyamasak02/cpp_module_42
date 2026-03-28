#include "./includes/ReversePolishCalculator.hpp"
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Error: bad argument" << std::endl;
        return 1;
    }
    try
    {
        double res = ReversePolishCalculator::calculate(std::string(argv[1]));
        std::cout << res << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "Error" << std::endl;
        return 1;
    }
    return 0;
}
