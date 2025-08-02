#include "../includes/PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <vector>

PmergeMe::PmergeMe()
{
    std::cout << "PmergeMe: "
              << "default constructor called" << std::endl;
}

PmergeMe::~PmergeMe()
{
    std::cout << "PmergeMe: "
              << "default destructor called" << std::endl;
}

PmergeMe::PmergeMe(const PmergeMe &copy)
{
    (void)copy;
    std::cout << "PmergeMe: "
              << "copy constructor called" << std::endl;
}

PmergeMe &PmergeMe::operator=(const PmergeMe &copy)
{
    (void)copy;
    std::cout << "PmergeMe: "
              << "copy assignment called" << std::endl;
    return *this;
}

/*
 * Convert a string to a positive integer
 *
 * @param str: string to be converted
 */
int PmergeMe::convert_positive_int(const std::string &str)
{
    std::stringstream ss(str);
    int value;
    ss >> value;
    if (ss.fail() || !ss.eof())
    {
        return (-1);
    }
    return (value);
}

/*
 * Create an array of integers from the input strings
 *
 * @param str_ptr: array of strings
 * @param size: size of the array
 */
int *PmergeMe::create_numbers(char **str_ptr, int size)
{
    int *array = new int[size];
    for (int i = 0; i < size; i++)
    {
        int res = PmergeMe::convert_positive_int(std::string(str_ptr[i]));
        if (res == -1)
        {
            delete[] array;
            throw PmergeMe::InvalidInput();
        }
        array[i] = res;
    }
    return array;
}

template <typename Container> void PmergeMe::show_container(const Container &arr) const
{
    for (typename Container::const_iterator it = arr.begin(); it != arr.end(); ++it)
    {
        if (it != arr.begin())
            std::cout << " ";
        std::cout << *it;
    }
    std::cout << std::endl;
}

/*
 * Merge insertion sort algorithm
 *
 * @param arr: array to be sorted
 * @param l: left index
 * @param r: right index
 */
void PmergeMe::merge_insertion_sort(std::vector<int> &arr, int l, int r, int chunk_size)
{
    if (r - l <= 1)
    {
        return;
    }
    std::vector<int> arr_b;
    std::vector<int> arr_a;
    for (int i = l; i + 1 < r; i += 2)
    {
        arr_b.push_back(std::min(arr[i], arr[i + 1]));
        arr_a.push_back(std::max(arr[i], arr[i + 1]));
    }
    if ((r - l) % 2 == 1)
    {
        arr_a.push_back(arr[r - 1]);
    }
    this->merge_insertion_sort(arr_a, 0, arr_a.size(), chunk_size * 2);
    for (int i = 0; i < (int)arr_b.size(); ++i)
    {
        arr_a.insert(std::lower_bound(arr_a.begin(), arr_a.end(), arr_b[i]), arr_b[i]);
    }
    arr = arr_a;
}

/*
 * Execute the merge insertion sort algorithm
 *
 * @param array: array to be sorted
 * @param size: size of the array
 */
void PmergeMe::execute_sort(const int *array, const int size)
{
    std::vector<int> array_vec(array, array + size);

    // Apply merge-insertion sort
    this->merge_insertion_sort(array_vec, 0, size, 2);

    // Display sorted sequence
    std::cout << "After:  ";
    this->show_container(array_vec);
}

void PmergeMe::execute_sort_deque(const int *array, const int size)
{
    std::deque<int> array_deque(array, array + size);

    // Apply merge-insertion sort for deque
    this->merge_insertion_sort_deque(array_deque, 0, size, 2);

    // Display sorted sequence
    std::cout << "After:  ";
    this->show_container(array_deque);
}

void PmergeMe::merge_insertion_sort_deque(std::deque<int> &arr, int l, int r, int chunk_size)
{
    if (r - l <= 1)
    {
        return;
    }

    std::deque<int> arr_b;
    std::deque<int> arr_a;

    // Create pairs and separate into winners and losers
    for (int i = l; i + 1 < r; i += 2)
    {
        arr_b.push_back(std::min(arr[i], arr[i + 1]));
        arr_a.push_back(std::max(arr[i], arr[i + 1]));
    }

    // Handle odd element
    if ((r - l) % 2 == 1)
    {
        arr_a.push_back(arr[r - 1]);
    }

    // Recursively sort winners
    if (arr_a.size() > 1)
    {
        merge_insertion_sort_deque(arr_a, 0, arr_a.size(), chunk_size * 2);
    }

    // Insert losers into sorted winners
    for (size_t i = 0; i < arr_b.size(); ++i)
    {
        arr_a.insert(std::lower_bound(arr_a.begin(), arr_a.end(), arr_b[i]), arr_b[i]);
    }
    // Update original deque
    arr = arr_a;
}

/*
 * Exception class for invalid input
 */
const char *PmergeMe::InvalidInput::what() const throw()
{
    return ("Invalid input");
}