#include "../includes/ReversePolishCalculator.hpp"
#include <iostream>
#include <stdexcept>

ReversePolishCalculator::ReversePolishCalculator() {}

ReversePolishCalculator::~ReversePolishCalculator() {}

ReversePolishCalculator::ReversePolishCalculator(const ReversePolishCalculator &copy)
{
    (void)copy;
}

ReversePolishCalculator &ReversePolishCalculator::operator=(const ReversePolishCalculator &copy)
{
    (void)copy;
    if (this == &copy)
        return *this;
    return *this;
}

double ReversePolishCalculator::calculate(const std::string &formula)
{
    DoubleStack data;
    std::stringstream ss(formula);
    std::string token;

    if (formula.empty())
        throw std::runtime_error("Error: empty expression");

    while (ss >> token)
    {
        std::stringstream token_ss(token);
        double number;

        if (token_ss >> number && token_ss.eof())
        {
            if (number >= 10 || number < 0)
                throw std::runtime_error("Error: token out of range: " + token);
            data.push(number);
        }
        else if (is_operator(token))
        {
            if (data.size() < 2)
                throw std::runtime_error("Error: not enough operands");
            double b = data.top();
            data.pop();
            double a = data.top();
            data.pop();
            if (token == "+")
                data.push(a + b);
            else if (token == "-")
                data.push(a - b);
            else if (token == "*")
                data.push(a * b);
            else if (token == "/")
            {
                if (b == 0)
                    throw std::runtime_error("Error: division by zero");
                data.push(a / b);
            }
        }
        else
        {
            throw std::runtime_error("Error: invalid token: " + token);
        }
    }

    if (data.size() != 1)
        throw std::runtime_error("Error: too many operands");

    return data.top();
}

bool ReversePolishCalculator::is_operator(const std::string &str)
{
    return (str == "+" || str == "-" || str == "*" || str == "/");
}
