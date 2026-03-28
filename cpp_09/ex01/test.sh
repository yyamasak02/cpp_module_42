make re

valgrind --leak-check=full -q ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
valgrind --leak-check=full -q ./RPN "7 7 * 7 -"
valgrind --leak-check=full -q ./RPN "1 2 * 2 / 2 * 2 4 - +"
valgrind --leak-check=full -q ./RPN "("