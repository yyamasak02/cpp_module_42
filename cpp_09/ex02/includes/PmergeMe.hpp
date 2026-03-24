#pragma once

#include <deque>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

class PmergeMe
{
  public:
    PmergeMe();
    ~PmergeMe();
    static int convert_positive_int(const std::string &str);
    static int *create_numbers(char **str_ptr, int size);
    void execute_sort(const int *array, int size);

    class PmergeMeException : public std::exception
    {
      public:
        virtual const char *what() const throw() = 0;
    };
    class InvalidInput : public PmergeMeException
    {
      public:
        virtual const char *what() const throw();
    };

  private:
    PmergeMe(const PmergeMe &copy);
    PmergeMe &operator=(const PmergeMe &copy);

    typedef std::vector<int> Chain;
    typedef std::vector<Chain> ChainVec;
    typedef std::deque<int> ChainDeq;
    typedef std::deque<ChainDeq> ChainDeqVec;

    int compare_count_;

    static int jacobsthal_number(int n);

    template <typename Iter> Iter find_insert_pos(Iter first, Iter last, int val)
    {
        while (first < last)
        {
            Iter mid = first + (last - first) / 2;
            compare_count_++;
            if (mid->back() < val)
                first = mid + 1;
            else
                last = mid;
        }
        return first;
    }

    ChainVec fj_sort(ChainVec &groups);
    ChainDeqVec fj_sort(ChainDeqVec &groups);
    void sort_vec(ChainVec &vec);
    void sort_deq(ChainDeqVec &deq);
    PmergeMe *resetCompareCount();
};
