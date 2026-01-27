#include "../includes/PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <vector>

PmergeMe::PmergeMe()
{
    // std::cout << "PmergeMe: "
    //           << "default constructor called" << std::endl;
}

PmergeMe::~PmergeMe()
{
    // std::cout << "PmergeMe: "
    //           << "default destructor called" << std::endl;
}

PmergeMe::PmergeMe(const PmergeMe &copy)
{
    (void)copy;
    // std::cout << "PmergeMe: "
    //           << "copy constructor called" << std::endl;
}

PmergeMe &PmergeMe::operator=(const PmergeMe &copy)
{
    (void)copy;
    // std::cout << "PmergeMe: "
    //           << "copy assignment called" << std::endl;
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
 * Generate insertion order based on Jacobsthal sequence
 * Jacobsthal: J(0)=0, J(1)=1, J(n)=J(n-1)+2*J(n-2)
 * Sequence: 0, 1, 1, 3, 5, 11, 21, 43, 85, ...
 *
 * Insertion order: b[0], b[1], b[3], b[2], b[5], b[4], b[11], b[10], ..., b[6], ...
 */
std::vector<size_t> PmergeMe::generate_jacobsthal_order(size_t n)
{
    std::vector<size_t> order;
    if (n == 0)
        return order;

    // Generate Jacobsthal numbers up to n
    std::vector<size_t> jacobsthal;
    jacobsthal.push_back(0);
    jacobsthal.push_back(1);
    while (jacobsthal.back() < n)
    {
        size_t next = jacobsthal[jacobsthal.size() - 1] + 2 * jacobsthal[jacobsthal.size() - 2];
        jacobsthal.push_back(next);
    }

    // Generate insertion order
    std::vector<bool> inserted(n, false);

    // First element b[0] is always inserted first
    order.push_back(0);
    inserted[0] = true;

    // Process each Jacobsthal number
    for (size_t k = 2; k < jacobsthal.size(); ++k)
    {
        size_t curr = jacobsthal[k];
        size_t prev = jacobsthal[k - 1];

        // Insert from curr down to prev+1 (in descending order)
        for (size_t i = std::min(curr, n); i > prev; --i)
        {
            if (i - 1 < n && !inserted[i - 1])
            {
                order.push_back(i - 1);
                inserted[i - 1] = true;
            }
        }
    }

    return order;
}

PmergeMe::binary_search_result PmergeMe::binary_search(const std::vector<int> &arr, int value)
{

    size_t comparisons = 0;

    // Binary search with comparison counting
    std::vector<int>::const_iterator lo = arr.begin();
    std::vector<int>::const_iterator hi = arr.end();
    while (lo < hi)
    {
        ++comparisons;
        std::vector<int>::const_iterator mid = lo + (hi - lo) / 2;
        if (*mid < value)
            lo = mid + 1;
        else
            hi = mid;
    }
    return binary_search_result(static_cast<size_t>(lo - arr.begin()), comparisons);
}

PmergeMe::binary_search_result PmergeMe::binary_search(const std::deque<int> &arr, int value)
{

    size_t comparisons = 0;

    // Binary search with comparison counting
    std::deque<int>::const_iterator lo = arr.begin();
    std::deque<int>::const_iterator hi = arr.end();
    while (lo < hi)
    {
        ++comparisons;
        std::deque<int>::const_iterator mid = lo + (hi - lo) / 2;
        if (*mid < value)
            lo = mid + 1;
        else
            hi = mid;
    }
    return binary_search_result(static_cast<size_t>(lo - arr.begin()), comparisons);
}

/*
 * Merge insertion sort algorithm (Ford-Johnson)
 *
 * @param arr: array to be sorted
 * @param l: left index
 * @param r: right index
 * @return: number of comparisons made
 */
size_t PmergeMe::merge_insertion_sort(std::vector<int> &arr, int l, int r, int chunk_size)
{
    if (r - l <= 1)
    {
        return 0;
    }
    size_t comparisons = 0;
    std::vector<int> arr_b;
    std::vector<int> arr_a;
    for (int i = l; i + 1 < r; i += 2)
    {
        ++comparisons;
        if (arr[i] < arr[i + 1])
        {
            arr_b.push_back(arr[i]);
            arr_a.push_back(arr[i + 1]);
        }
        else
        {
            arr_b.push_back(arr[i + 1]);
            arr_a.push_back(arr[i]);
        }
    }
    if ((r - l) % 2 == 1)
    {
        arr_b.push_back(arr[r - 1]);
    }
    comparisons += this->merge_insertion_sort(arr_a, 0, arr_a.size(), chunk_size * 2);

    // Insert arr_b elements following Jacobsthal sequence order
    std::vector<size_t> order = this->generate_jacobsthal_order(arr_b.size());
    for (size_t i = 0; i < order.size(); ++i)
    {
        int value = arr_b[order[i]];
        PmergeMe::binary_search_result result = this->binary_search(arr_a, value);
        arr_a.insert(arr_a.begin() + result.index, value);
        comparisons += result.comparisons;
    }
    arr = arr_a;
    return comparisons;
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
    size_t comparisons = this->merge_insertion_sort(array_vec, 0, size, 2);

    // Display sorted sequence
    std::cout << "After: ";
    this->show_container(array_vec);

    // Display comparison count
    std::cout << "Comparisons (vector): " << comparisons << std::endl;
}

void PmergeMe::execute_sort_deque(const int *array, const int size)
{
    std::deque<int> array_deque(array, array + size);

    // Apply merge-insertion sort for deque
    size_t comparisons = this->merge_insertion_sort_deque(array_deque, 0, size, 2);

    // Display sorted sequence
    std::cout << "After: ";
    this->show_container(array_deque);

    // Display comparison count
    std::cout << "Comparisons (deque): " << comparisons << std::endl;
}

size_t PmergeMe::merge_insertion_sort_deque(std::deque<int> &arr, int l, int r, int chunk_size)
{
    if (r - l <= 1)
    {
        return 0;
    }

    size_t comparisons = 0;
    std::deque<int> arr_b;
    std::deque<int> arr_a;

    // Create pairs and separate into winners and losers
    for (int i = l; i + 1 < r; i += 2)
    {
        ++comparisons;
        if (arr[i] < arr[i + 1])
        {
            arr_b.push_back(arr[i]);
            arr_a.push_back(arr[i + 1]);
        }
        else
        {
            arr_b.push_back(arr[i + 1]);
            arr_a.push_back(arr[i]);
        }
    }

    // Handle odd element
    if ((r - l) % 2 == 1)
    {
        arr_b.push_back(arr[r - 1]);
    }

    // Recursively sort winners
    if (arr_a.size() > 1)
    {
        comparisons += merge_insertion_sort_deque(arr_a, 0, arr_a.size(), chunk_size * 2);
    }

    // Insert losers into sorted winners following Jacobsthal sequence order
    std::vector<size_t> order = this->generate_jacobsthal_order(arr_b.size());
    for (size_t i = 0; i < order.size(); ++i)
    {
        int value = arr_b[order[i]];
        // Binary search with comparison counting
        PmergeMe::binary_search_result result = this->binary_search(arr_a, value);
        arr_a.insert(arr_a.begin() + result.index, value);
        comparisons += result.comparisons;
    }
    // Update original deque
    arr = arr_a;
    return comparisons;
}

/*
 * Exception class for invalid input
 */
const char *PmergeMe::InvalidInput::what() const throw()
{
    return ("Invalid input");
}