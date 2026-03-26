#include "../includes/PmergeMe.hpp"
#include <climits>
#include <ctime>
#include <sstream>

PmergeMe::PmergeMe() : compare_count_(0) {}

PmergeMe::~PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe &copy) : compare_count_(copy.compare_count_) {}

PmergeMe &PmergeMe::operator=(const PmergeMe &copy)
{
    if (this != &copy)
    {
        this->compare_count_ = copy.compare_count_;
    }
    return *this;
}

/*
 * Convert a string to a positive integer.
 * Returns -1 on invalid input.
 */
int PmergeMe::convert_positive_int(const std::string &str)
{
    std::stringstream ss(str);
    long value;
    ss >> value;
    if (ss.fail() || !ss.eof() || value < 0 || value > INT_MAX)
        return -1;
    return static_cast<int>(value);
}

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

/*
 * Returns the n-th Jacobsthal number doubled: J(n)*2 where J(n) = (2^n - (-1)^n) / 3.
 * This determines how many elements to insert per batch during the insertion phase.
 */
size_t PmergeMe::jacobsthal_number(size_t n)
{
    int sign = (n % 2 == 0) ? -1 : 1;
    int pow2 = 1;
    for (int i = 0; i < n; i++)
        pow2 *= 2;
    return (pow2 + sign) / 3 * 2;
}

void PmergeMe::sort_vec(ChainVecList &vec)
{
    std::clock_t start = std::clock();
    ChainVecList sorted_container = fj_sort(vec);
    std::clock_t end = std::clock();

    std::cout << "After:  ";
    for (size_t i = 0; i < sorted_container.size(); i++)
        std::cout << sorted_container[i][0] << " ";
    std::cout << std::endl;

    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000.0;
    std::cout << "Time to process a range of " << std::setw(4) << std::setfill(' ') << sorted_container.size()
              << " elements with std::vector : " << elapsed << " us" << std::endl;
    std::cout << "Number of comparisons with std::vector : " << this->compare_count_ << std::endl;
}

void PmergeMe::sort_deq(ChainDeqList &deq)
{
    std::clock_t start = std::clock();
    ChainDeqList sorted_container = fj_sort(deq);
    std::clock_t end = std::clock();

    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000.0;
    std::cout << "Time to process a range of " << std::setw(4) << std::setfill(' ') << sorted_container.size()
              << " elements with std::deque  : " << elapsed << " us" << std::endl;
    std::cout << "Number of comparisons with std::deque  : " << this->compare_count_ << std::endl;
}

void PmergeMe::execute_sort(const int *array, int size)
{
    std::cout << "Before: ";
    ChainVecList vec = ChainVecList();
    ChainDeqList deq = ChainDeqList();

    for (int i = 0; i < size; i++)
    {
        std::cout << array[i] << " ";
        vec.push_back(ChainVec(1, array[i]));
        deq.push_back(ChainDeq(1, array[i]));
    }
    std::cout << std::endl;

    resetCompareCount();
    sort_vec(vec);
    resetCompareCount();
    sort_deq(deq);
}

const char *PmergeMe::InvalidInput::what() const throw()
{
    return "Invalid input";
}

PmergeMe *PmergeMe::resetCompareCount()
{
    this->compare_count_ = 0;
    return this;
}
