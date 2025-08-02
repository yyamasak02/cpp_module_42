#pragma once

#include <algorithm>
#include <deque>
#include <string>
#include <vector>
// #include <iostream>

#define ERR_NO 1
#define NORMAL 0

class PmergeMe
{
  private:
    PmergeMe(const PmergeMe &copy);
    PmergeMe &operator=(const PmergeMe &copy);
    void merge_insertion_sort(std::vector<int> &arr, int l, int r, int chunk_size);
    void merge_insertion_sort_deque(std::deque<int> &arr, int l, int r, int chunk_size);
    template <typename Container> void show_container(const Container &arr) const;

  public:
    PmergeMe();
    ~PmergeMe();
    static int convert_positive_int(const std::string &str);
    static int *create_numbers(char **str_ptr, int size);
    void execute_sort(const int *array, const int size);
    void execute_sort_deque(const int *array, const int size);
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
};