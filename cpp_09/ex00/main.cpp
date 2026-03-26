#include "./includes/BitcoinExchange.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cerr << "Error: could not open file." << std::endl;
        return 1;
    }
    if (argc != 2)
    {
        std::cerr << "Error: too much arguments" << std::endl;
        return (0);
    }
    std::string rate_file_path = "./const/data.csv";
    std::string input_file_path(argv[1]);
    try
    {
        BitcoinExchange bit(rate_file_path);
        // bit.show();
        bit.execute_exchange(input_file_path);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}