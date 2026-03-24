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
 * Returns the n-th Jacobsthal number, scaled for group counts.
 * Used to determine the batch sizes during the insertion phase.
 */
/*
 * Returns the n-th Jacobsthal number doubled: J(n)*2 where J(n) = (2^n - (-1)^n) / 3.
 * This determines how many elements to insert per batch during the insertion phase.
 */
int PmergeMe::jacobsthal_number(int n)
{
    int sign = (n % 2 == 0) ? -1 : 1;
    int pow2 = 1;
    for (int i = 0; i < n; i++)
        pow2 *= 2;
    return (pow2 + sign) / 3 * 2;
}

/*
 * Recursively applies Ford-Johnson sort to a sequence of groups (vector version).
 * Each group stores [small..., large...] — the small half precedes the large half.
 * At each level pairs are formed, sorted by the last element, then recursion happens.
 * After recursion, the small halves are inserted via binary search in Jacobsthal order.
 */
PmergeMe::ChainVec PmergeMe::fj_sort(ChainVec &groups)
{
    if (groups.size() == 1)
        return groups;

    // Step 1: pair adjacent groups; sort each pair so large comes last
    ChainVec pairs;
    for (size_t i = 0; i + 1 < groups.size(); i += 2)
    {
        Chain merged;
        this->compare_count_++;
        if (groups[i].back() < groups[i + 1].back())
        {
            merged.insert(merged.end(), groups[i].begin(), groups[i].end());
            merged.insert(merged.end(), groups[i + 1].begin(), groups[i + 1].end());
        }
        else
        {
            merged.insert(merged.end(), groups[i + 1].begin(), groups[i + 1].end());
            merged.insert(merged.end(), groups[i].begin(), groups[i].end());
        }
        pairs.push_back(merged);
    }

    Chain straggler = groups.back();

    // Step 2: recursively sort the paired sequence
    ChainVec rec = fj_sort(pairs);

    // Step 3: rebuild sorted sequence and insert small halves
    ChainVec result;
    int batch_start = 0;
    int batch_idx = 1;
    size_t batch_end = 1;
    int done = 0;

    while (!done)
    {
        if (batch_end >= rec.size())
        {
            batch_end = rec.size();
            done = 1;
        }
        // push the large (right) half of each group into result
        for (size_t p = static_cast<size_t>(batch_start); p < batch_end; p++)
        {
            Chain large(rec[p].begin() + rec[p].size() / 2, rec[p].end());
            result.push_back(large);
        }
        int extra = 0;
        if (done && groups.size() % 2)
        {
            result.insert(find_insert_pos(result.begin(), result.end(), straggler.back()), straggler);
            extra = 1;
        }
        // insert small halves in reverse Jacobsthal order to keep comparisons minimal
        Chain inserted_keys;
        for (int p = static_cast<int>(batch_end); p > batch_start; p--)
        {
            size_t search_end = static_cast<size_t>(batch_start + p + extra - 1);
            for (Chain::iterator it = inserted_keys.begin(); it != inserted_keys.end(); ++it)
            {
                if (rec[p - 1].back() >= *it)
                    search_end++;
            }
            int key = rec[p - 1][rec[p - 1].size() / 2 - 1];
            Chain small(rec[p - 1].begin(), rec[p - 1].begin() + rec[p - 1].size() / 2);
            inserted_keys.push_back(key);
            result.insert(find_insert_pos(result.begin(), result.begin() + search_end, key), small);
        }
        batch_start = static_cast<int>(batch_end);
        batch_end = static_cast<size_t>(batch_start) + jacobsthal_number(batch_idx);
        batch_idx++;
    }
    return result;
}

void PmergeMe::sort_vec(ChainVec &vec)
{
    std::clock_t start = std::clock();
    ChainVec sorted_container = fj_sort(vec);
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

/*
 * Same algorithm, deque variant.
 */
PmergeMe::ChainDeqVec PmergeMe::fj_sort(ChainDeqVec &groups)
{
    if (groups.size() == 1)
        return groups;

    ChainDeqVec pairs;
    for (size_t i = 0; i + 1 < groups.size(); i += 2)
    {
        ChainDeq merged;
        this->compare_count_++;
        if (groups[i].back() < groups[i + 1].back())
        {
            merged.insert(merged.end(), groups[i].begin(), groups[i].end());
            merged.insert(merged.end(), groups[i + 1].begin(), groups[i + 1].end());
        }
        else
        {
            merged.insert(merged.end(), groups[i + 1].begin(), groups[i + 1].end());
            merged.insert(merged.end(), groups[i].begin(), groups[i].end());
        }
        pairs.push_back(merged);
    }

    ChainDeq straggler = groups.back();

    ChainDeqVec rec = fj_sort(pairs);

    ChainDeqVec result;
    int batch_start = 0;
    int batch_idx = 1;
    size_t batch_end = 1;
    int done = 0;

    while (!done)
    {
        if (batch_end >= rec.size())
        {
            batch_end = rec.size();
            done = 1;
        }
        for (size_t p = static_cast<size_t>(batch_start); p < batch_end; p++)
        {
            ChainDeq large(rec[p].begin() + rec[p].size() / 2, rec[p].end());
            result.push_back(large);
        }
        int extra = 0;
        if (done && groups.size() % 2)
        {
            result.insert(find_insert_pos(result.begin(), result.end(), straggler.back()), straggler);
            extra = 1;
        }
        ChainDeq inserted_keys;
        for (int p = static_cast<int>(batch_end); p > batch_start; p--)
        {
            size_t search_end = static_cast<size_t>(batch_start + p + extra - 1);
            for (ChainDeq::iterator it = inserted_keys.begin(); it != inserted_keys.end(); ++it)
            {
                if (rec[p - 1].back() >= *it)
                    search_end++;
            }
            int key = rec[p - 1][rec[p - 1].size() / 2 - 1];
            ChainDeq small(rec[p - 1].begin(), rec[p - 1].begin() + rec[p - 1].size() / 2);
            inserted_keys.push_back(key);
            result.insert(find_insert_pos(result.begin(), result.begin() + search_end, key), small);
        }
        batch_start = static_cast<int>(batch_end);
        batch_end = static_cast<size_t>(batch_start) + jacobsthal_number(batch_idx);
        batch_idx++;
    }
    return result;
}

void PmergeMe::sort_deq(ChainDeqVec &deq)
{
    std::clock_t start = std::clock();
    ChainDeqVec sorted_container = fj_sort(deq);
    std::clock_t end = std::clock();

    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000.0;
    std::cout << "Time to process a range of " << std::setw(4) << std::setfill(' ') << sorted_container.size()
              << " elements with std::deque  : " << elapsed << " us" << std::endl;
    std::cout << "Number of comparisons with std::deque  : " << this->compare_count_ << std::endl;
}

void PmergeMe::execute_sort(const int *array, int size)
{
    std::cout << "Before: ";
    ChainVec vec = ChainVec();
    ChainDeqVec deq = ChainDeqVec();

    for (int i = 0; i < size; i++)
    {
        std::cout << array[i] << " ";
        vec.push_back(Chain(1, array[i]));
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
