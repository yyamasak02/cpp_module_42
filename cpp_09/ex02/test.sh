make re

valgrind --leak-check=full -q ./PmergeMe 3 5 9 2 4
valgrind --leak-check=full -q ./PmergeMe 3 5 9 7 4
valgrind --leak-check=full -q ./PmergeMe 20 17 18 14 12 11 7 4 5 3 2 1 6 9 8 10 15 16 19 13 21
valgrind --leak-check=full -q ./PmergeMe 1
valgrind --leak-check=full -q ./PmergeMe 1 2
valgrind --leak-check=full -q ./PmergeMe 2 1
# valgrind --leak-check=full -q ./PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`
# valgrind --leak-check=full -q ./PmergeMe `shuf -i 1-100000 -n 3000 | tr "\n" " "`