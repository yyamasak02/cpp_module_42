#include "./includes/PmergeMe.hpp"
#include <ctime>
#include <iostream>
#include <sstream>

void show_static_array(const int *array, const int size)
{
    for (int i = 0; i < size; ++i)
    {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    std::clock_t start, end;
    double duration_us;
    PmergeMe sorter_obj;
    int *array = NULL;

    if (argc < 2)
    {
        std::cerr << "Error: Lack of arguments" << std::endl;
        return ERR_NO;
    }

    try
    {
        array = PmergeMe::create_numbers(&(argv[1]), argc - 1);
        if (!array)
        {
            std::cerr << "Error: Invalid input" << std::endl;
            return ERR_NO;
        }
        // Display unsorted sequence
        std::cout << "Before: ";
        show_static_array(array, argc - 1);
        // Time vector sorting
        {
            start = std::clock();
            sorter_obj.execute_sort(array, argc - 1);
            end = std::clock();
            duration_us = (double)(end - start) * 1000000.0 / CLOCKS_PER_SEC;
            std::cout << "Time to process a range of " << (argc - 1) << " elements with std::vector : " << duration_us
                      << " us" << std::endl;
        }
        // Time deque sorting
        {
            start = std::clock();
            sorter_obj.execute_sort_deque(array, argc - 1);
            end = std::clock();
            duration_us = (double)(end - start) * 1000000.0 / CLOCKS_PER_SEC;
            std::cout << "Time to process a range of " << (argc - 1) << " elements with std::deque  : " << duration_us
                      << " us" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error" << std::endl;
        if (array)
            delete[] array;
        return 1;
    }

    if (array)
        delete[] array;
    return 0;
}