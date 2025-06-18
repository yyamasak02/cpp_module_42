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

void PmergeMe::show(const std::vector<int> &arr) const
{
    std::cout << "[";
    for (std::vector<int>::const_iterator it = arr.begin(); it != arr.end(); ++it)
    {
        if (it == arr.begin())
            std::cout << *it;
        else
            std::cout << ", " << *it;
    }
    std::cout << "]" << std::endl;
}

/*
 * Merge insertion sort algorithm
 *
 * @param arr: array to be sorted
 * @param l: left index
 * @param mid: middle index
 * @param r: right index
 */
void PmergeMe::merge_insert(std::vector<int> &arr, int l, int mid, int r, int chunk_size)
{
    std::cout << "index: [l, mid, r] = [" << l << ", " << (l + r) / 2 << ", " << r << "]"
              << " "
              << "merge-insert: [" << arr[l] << ", " << arr[mid] << ", " << arr[r] << "]" << std::endl;
    if (arr[mid - 1] > arr[r - 1])
    {
        std::swap_ranges(arr.begin() + l, arr.begin() + mid, arr.begin() + mid);
        std::cout << "after: ";
        this->PmergeMe::show(arr);
    }
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
        int min_v = std::min(arr[i], arr[i + 1]);
        int max_v = std::max(arr[i], arr[i + 1]);
        arr_b.push_back(min_v);
        arr_a.push_back(max_v);
    }
    if ((r - l) % 2 == 1)
    {
        arr_a.push_back(arr[r - 1]);
    }
    this->merge_insertion_sort(arr_a, 0, arr_a.size(), chunk_size * 2);
    for (int i = 0; i < (int)arr_b.size(); ++i)
    {
        std::vector<int>::iterator it = std::lower_bound(arr_a.begin(), arr_a.end(), arr_b[i]);
        arr_a.insert(it, arr_b[i]);
    }
    arr = arr_a;
}

// void PmergeMe::ford_johnson_sort(std::vector<int> &arr, int l, int r)
// {
//     int size = r - l;
//     if (size <= 1)
//         return;

//     // 1. ペア分け
//     std::vector<int> main_chain;
//     std::vector<int> pending;

//     for (int i = l; i + 1 < r; i += 2)
//     {
//         int a = arr[i];
//         int b = arr[i + 1];
//         if (a < b)
//         {
//             main_chain.push_back(b);
//             pending.push_back(a);
//         }
//         else
//         {
//             main_chain.push_back(a);
//             pending.push_back(b);
//         }
//     }

//     // 奇数個のときは最後の要素が余る
//     if ((size % 2) == 1)
//         pending.push_back(arr[r - 1]);

//     // 2. main_chain を再帰的にソート
//     ford_johnson_sort(main_chain, 0, main_chain.size());

//     // 3. pending をスプレッド順で挿入（ここでは単純に1つずつでOK）
//     for (std::vector<int>::iterator it = pending.begin(); it != pending.end(); ++it)
//     {
//         int val = *it;
//         std::vector<int>::iterator insert_pos = std::lower_bound(main_chain.begin(), main_chain.end(), val);
//         main_chain.insert(insert_pos, val);
//     }

//     // 4. arr に結果を反映
//     for (int i = 0; i < (int)main_chain.size(); ++i)
//         arr[l + i] = main_chain[i];
// }

/*
 * Insertion sort algorithm
 *
 * @param arr: array to be sorted
 * @param array_size: size of the array
 */
void PmergeMe::insert_sort(std::vector<int> &arr, int array_size)
{
    int i, j;
    int tmp;

    for (i = 1; i < array_size; i++)
    {
        // 整列されていない部分の先頭を指す
        j = i;
        // 交換要素のためのインデックス

        // 整列済みの場合は処理しない
        while ((j > 0) && (arr[j - 1] > arr[j]))
        {
            // 整列されていない隣り合う要素を交換する
            tmp = arr[j - 1];
            arr[j - 1] = arr[j];
            arr[j] = tmp;
            // 隣り合う要素のインデックスを更新

            // this->PmergeMe::show(arr);
            j--;
        }
    }
}

/*
 * Binary search algorithm
 *
 * @param begin: start iterator
 * @param end: end iterator
 * @param value: value to be searched
 */
std::vector<int>::iterator PmergeMe::bisect(std::vector<int>::iterator begin, std::vector<int>::iterator end, int value)
{
    while (begin < end)
    {
        std::vector<int>::iterator mid = begin + (end - begin) / 2;
        if (*mid < value)
        {
            begin = mid + 1;
        }
        else
        {
            end = mid;
        }
    }
    return begin;
}

/*
 * binary Insertion sort algorithm
 *
 * @param arr: array to be sorted
 * @param array_size: size of the array
 */
void PmergeMe::binary_insert_sort(std::vector<int> &arr, int array_size)
{
    for (int i = 1; i < array_size; ++i)
    {
        int key = arr[i];
        std::vector<int>::iterator insert_pos = this->bisect(arr.begin(), arr.begin() + i, key);
        for (std::vector<int>::iterator it = arr.begin() + i; it != insert_pos; --it)
        {
            *it = *(it - 1);
        }
        *insert_pos = key;
    }
}

/*
 * Merge two sorted arrays
 *
 * @param arr: array to be sorted
 * @param l: left index
 * @param mid: middle index
 * @param r: right index
 */
void PmergeMe::merge(std::vector<int> &arr, int l, int mid, int r)
{
    std::vector<int> left(arr.begin() + l, arr.begin() + mid);
    std::vector<int> right(arr.begin() + mid, arr.begin() + r);

    int i = 0, j = 0, k = l;
    while (i < (int)left.size() && j < (int)right.size())
    {
        if (left[i] <= right[j])
        {
            arr[k++] = left[i++];
        }
        else
        {
            arr[k++] = right[j++];
        }
    }

    while (i < (int)left.size())
    {
        arr[k++] = left[i++];
    }

    while (j < (int)right.size())
    {
        arr[k++] = right[j++];
    }
}

/*
 * Merge sort algorithm
 *
 * @param arr: array to be sorted
 * @param l: left index
 * @param r: right index
 */
void PmergeMe::merge_sort(std::vector<int> &arr, int l, int r)
{

    if (r - l <= 1)
    {
        return;
    }
    int mid = (l + r) / 2;
    std::cout << "index: [l, mid, r] = [" << l << ", " << (l + r) / 2 << ", " << r << "]" << std::endl;
    this->PmergeMe::merge_sort(arr, l, mid);
    this->PmergeMe::merge_sort(arr, mid, r);
    this->PmergeMe::merge(arr, l, mid, r);
}

/*
 * Execute the merge insertion sort algorithm
 *
 * @param array: array to be sorted
 * @param size: size of the array
 */
void PmergeMe::execute_sort(const int *array, const int size)
{
    int end = (size - (size % 2));
    std::vector<int> array_vec(array, array + size);
    this->show(array_vec);
    // this->merge_sort(array_vec, 0, size);
    // this->insert_sort(array_vec, size);
    // this->binary_insert_sort(array_vec, size);
    this->merge_insertion_sort(array_vec, 0, size - 1, 2);
    // this->ford_johnson_sort(array_vec, 0, size - 1);
    this->show(array_vec);
}

/*
 * Exception class for invalid input
 */
const char *PmergeMe::InvalidInput::what() const throw()
{
    return ("Invalid input");
}