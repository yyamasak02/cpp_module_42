#include "./includes/PmergeMe.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error" << std::endl;
        return 1;
    }
    int *array = NULL;
    try
    {
        PmergeMe pmergeme;
        array = PmergeMe::create_numbers(&(argv[1]), argc - 1);
        pmergeme.execute_sort(array, argc - 1);
        delete[] array;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        delete[] array;
        return 1;
    }
    return 0;
}
