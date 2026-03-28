#!/usr/bin/env bash

set -u

echo "== Build =="
make re || exit 1

echo
echo "== Valid Cases =="
./PmergeMe 3 5 9 7 4
./PmergeMe 20 17 18 14 12 11 7 4 5 3 2 1 6 9 8 10 15 16 19 13 21
./PmergeMe 7
./PmergeMe 1 2
./PmergeMe 2 1
./PmergeMe 5 2 5 1 3 3 4 2 1 4

echo
echo "== Invalid Cases (expected to fail) =="
./PmergeMe || true
./PmergeMe -1 2 3 || true
./PmergeMe 1.5 2 || true
./PmergeMe abc 2 || true
./PmergeMe "" || true

echo
echo "== Done =="