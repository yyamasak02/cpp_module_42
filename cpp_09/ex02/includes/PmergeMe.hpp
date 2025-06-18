#pragma once

#include <algorithm>
#include <string>
#include <vector>
// #include <iostream>

class PmergeMe
{
  private:
    PmergeMe(const PmergeMe &copy);
    PmergeMe &operator=(const PmergeMe &copy);
    void merge_insertion_sort(std::vector<int> &arr, int l, int r, int chunk_size);
    void merge_insert(std::vector<int> &arr, int l, int mid, int r, int chunk_size);
    void insert_sort(std::vector<int> &arr, int array_size);
    void merge_sort(std::vector<int> &arr, int l, int r);
    void merge(std::vector<int> &arr, int l, int mid, int r);
    void binary_insert_sort(std::vector<int> &arr, int array_size);
    void show(const std::vector<int> &arr) const;
    std::vector<int>::iterator bisect(std::vector<int>::iterator begin, std::vector<int>::iterator end, int value);

  public:
    PmergeMe();
    ~PmergeMe();
    static int convert_positive_int(const std::string &str);
    static int *create_numbers(char **str_ptr, int size);
    void execute_sort(const int *array, const int size);
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