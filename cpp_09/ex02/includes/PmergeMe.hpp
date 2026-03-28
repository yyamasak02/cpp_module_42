#pragma once

#include <deque>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

# define DEBUG false
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

    typedef std::vector<int> ChainVec;
    typedef std::vector<ChainVec> ChainVecList;
    typedef std::deque<int> ChainDeq;
    typedef std::deque<ChainDeq> ChainDeqList;

    int compare_count_;

    static size_t jacobsthal_number(size_t n);

    // vector/deque 用オーバーロード実装
    ChainVecList::iterator find_insert_pos(ChainVecList::iterator first, ChainVecList::iterator last, int val);
    ChainDeqList::iterator find_insert_pos(ChainDeqList::iterator first, ChainDeqList::iterator last, int val);

    ChainVecList generate_next_level_groups(const ChainVecList &groups);
    ChainDeqList generate_next_level_groups(const ChainDeqList &groups);

    ChainVecList binary_insertion_with_jacobsthal(const ChainVecList &records, const ChainVec &extra_elem,
                                                  const bool has_extra);
    ChainDeqList binary_insertion_with_jacobsthal(const ChainDeqList &records, const ChainDeq &extra_elem,
                                                  const bool has_extra);

    ChainVecList fj_sort(const ChainVecList &groups);
    ChainDeqList fj_sort(const ChainDeqList &groups);

    void sort_vec(ChainVecList &vec);
    void sort_deq(ChainDeqList &deq);
    PmergeMe *resetCompareCount();
};
