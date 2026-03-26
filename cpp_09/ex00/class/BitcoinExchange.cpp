#include "../includes/BitcoinExchange.hpp"
#include <ctime>
#include <iostream>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &copy) : data_(copy.data_) {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &copy)
{
    if (this != &copy)
        data_ = copy.data_;
    return *this;
}

BitcoinExchange::BitcoinExchange(const std::string rate_path)
{
    this->data_ = _read_rate_file(rate_path);
}

std::map<std::string, float> BitcoinExchange::_read_rate_file(const std::string &rate_path)
{
    std::map<std::string, float> result;
    std::fstream rate_file;
    std::string line, date_str, rate_str;
    float rate;

    rate_file.open(rate_path.c_str(), std::ios::in);
    if (!rate_file.is_open())
        throw FilePermissionException();

    std::getline(rate_file, line); // skip header
    while (std::getline(rate_file, line))
    {
        std::stringstream ss(line);
        std::getline(ss, date_str, ',');
        std::getline(ss, rate_str);
        std::stringstream(rate_str) >> rate;
        result[date_str] = rate;
    }
    return result;
}

void BitcoinExchange::show()
{
    for (std::map<std::string, float>::iterator it = this->data_.begin(); it != this->data_.end(); ++it)
        std::cout << it->first << " -> " << it->second << std::endl;
}

std::string BitcoinExchange::_trimSpace(const std::string &s)
{
    std::string::size_type start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    std::string::size_type end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool BitcoinExchange::_isValidDate(const std::string &date_str)
{
    if (date_str.length() != 10 || date_str[4] != '-' || date_str[7] != '-')
        return false;

    int y = std::atoi(date_str.substr(0, 4).c_str());
    int m = std::atoi(date_str.substr(5, 2).c_str());
    int d = std::atoi(date_str.substr(8, 2).c_str());

    if (m < 1 || m > 12 || d < 1 || d > 31)
        return false;

    struct tm t = {};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    t.tm_isdst = -1;
    mktime(&t);

    return (t.tm_year == y - 1900 && t.tm_mon == m - 1 && t.tm_mday == d);
}

void BitcoinExchange::execute_exchange(const std::string &target_path)
{
    std::fstream target_file;
    std::string line, date_str, value_str;
    float value;

    target_file.open(target_path.c_str(), std::ios::in);
    if (!target_file.is_open())
        throw FilePermissionException();

    std::getline(target_file, line); // skip header
    while (std::getline(target_file, line))
    {
        try
        {
            if (line.empty())
                throw BadFormatException();

            std::stringstream ss(line);
            if (!std::getline(ss, date_str, '|') || !std::getline(ss, value_str))
                throw BadFormatException();

            date_str = _trimSpace(date_str);
            value_str = _trimSpace(value_str);
            if (date_str.empty() || value_str.empty())
                throw BadFormatException();

            std::stringstream val_ss(value_str);
            val_ss >> value;
            if (val_ss.fail())
                throw InvalidNumberException();

            if (value > 1000)
                throw ValueTooHighException();
            if (value < 0)
                throw ValueTooLowException();
            if (!this->_isValidDate(date_str))
                throw InvalidDateException();

            std::map<std::string, float>::iterator it = this->data_.upper_bound(date_str);
            if (it == this->data_.begin())
                throw BadFormatException(); // date is before the oldest DB entry
            --it;
            std::cout << date_str << " => " << value << " = " << value * it->second << std::endl;
        }
        catch (const ValueTooHighException &)
        {
            std::cerr << "Error: too large a number." << std::endl;
        }
        catch (const ValueTooLowException &)
        {
            std::cerr << "Error: not a positive number." << std::endl;
        }
        catch (const ParseFailException &)
        {
            std::cerr << "Error: bad input => " << line << std::endl;
        }
    }
}

const char *BitcoinExchange::FilePermissionException::what() const throw()
{
    return "Failed to open file";
}
const char *BitcoinExchange::ParseFailException::what() const throw()
{
    return "Parse failed.";
}
const char *BitcoinExchange::ValueTooHighException::what() const throw()
{
    return "too large a number.";
}
const char *BitcoinExchange::ValueTooLowException::what() const throw()
{
    return "too small a number.";
}
const char *BitcoinExchange::BadFormatException::what() const throw()
{
    return "bad format.";
}
const char *BitcoinExchange::InvalidDateException::what() const throw()
{
    return "not valid date.";
}
const char *BitcoinExchange::InvalidNumberException::what() const throw()
{
    return "not a number.";
}
