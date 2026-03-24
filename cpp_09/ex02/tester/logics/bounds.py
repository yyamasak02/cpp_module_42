"""
Ford-Johnson (merge-insertion sort) theoretical comparison bounds.

fj_upper_bound(n)  -- worst-case upper bound W(n) for this algorithm
info_lower_bound(n) -- information-theoretic lower bound ceil(log2(n!))
                       (worst-case lower bound; individual inputs may use fewer)
"""

import math
from functools import lru_cache


@lru_cache(maxsize=None)
def _T(k: int) -> int:
    """k-th insertion-wave boundary: T(k) = (2^(k+1) + (-1)^k) / 3"""
    return (2 ** (k + 1) + ((-1) ** k)) // 3


@lru_cache(maxsize=None)
def _wave(j: int) -> int:
    """Smallest k such that T(k) >= j  (the insertion wave for pend index j)."""
    k = 1
    while _T(k) < j:
        k += 1
    return k


@lru_cache(maxsize=None)
def fj_upper_bound(n: int) -> int:
    """
    Upper bound W(n) on comparisons for Ford-Johnson sort.

    Recurrence:
      W(n) = floor(n/2)           pairing phase
           + W(floor(n/2))        recursive sort of winners
           + insertion cost       Jacobsthal-ordered binary insertion

    Insertion cost per pend index j  (j = 1 .. floor(n/2) + [n odd]):
      j == 1  ->  0  (b1 <= a1 is already known; trivially placed at front)
      j >= 2  ->  _wave(j)  (binary search in range bounded by 2^wave - 1)
    """
    if n <= 1:
        return 0
    m = n // 2
    cost = m + fj_upper_bound(m)
    for j in range(1, m + (n % 2) + 1):
        cost += 0 if j == 1 else _wave(j)
    return cost


def info_lower_bound(n: int) -> int:
    """Information-theoretic lower bound: ceil(log2(n!))"""
    if n <= 1:
        return 0
    return math.ceil(sum(math.log2(i) for i in range(2, n + 1)))
