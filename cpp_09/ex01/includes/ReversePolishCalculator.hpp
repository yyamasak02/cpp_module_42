#pragma once

#include <list>
#include <sstream>
#include <stack>
#include <string>
// #include <iostream>

class ReversePolishCalculator
{
  private:
    ReversePolishCalculator(const ReversePolishCalculator &copy);
    ReversePolishCalculator &operator=(const ReversePolishCalculator &copy);
    ~ReversePolishCalculator();
    ReversePolishCalculator();
    static bool is_operator(const std::string &str);

  public:
    typedef std::stack<double, std::list<double>> DoubleStack;
    static double calculate(const std::string &formula);
};

// std::ostream& operator<<(std::ostream& os, const ReversePolishCalculator&
// ReversePolishCalculator); std::ostream& operator<<(std::ostream& os, const
// ReversePolishCalculator* ReversePolishCalculator);
