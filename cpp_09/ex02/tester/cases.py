"""
Test case definitions.

  fixed_cases()  -- 21 deterministic/representative cases
  VOLUME_SIZES   -- element counts for volume tests
  ERROR_CASES    -- invalid-input cases that must exit non-zero
"""

import random
from typing import NamedTuple


class SortCase(NamedTuple):
    label: str
    nums:  list[int]


class ErrorCase(NamedTuple):
    label:         str
    args:          list[str]
    expected_exit: int


def fixed_cases() -> list[SortCase]:
    cases = [
        # ── minimal ──────────────────────────────────────────────────
        SortCase("single element",          [42]),
        SortCase("two  ordered",            [1, 2]),
        SortCase("two  reversed",           [2, 1]),
        # ── small ────────────────────────────────────────────────────
        SortCase("three elements",          [3, 1, 2]),
        SortCase("four elements",           [4, 2, 3, 1]),
        SortCase("five  (subject sample)",  [3, 5, 9, 7, 4]),
        # ── already sorted ───────────────────────────────────────────
        SortCase("6  ascending",            [1, 2, 3, 4, 5, 6]),
        SortCase("7  ascending",            list(range(1, 8))),
        # ── reverse sorted ───────────────────────────────────────────
        SortCase("6  descending",           [6, 5, 4, 3, 2, 1]),
        SortCase("10 descending",           list(range(10, 0, -1))),
        # ── Jacobsthal wave-boundary sizes ───────────────────────────
        SortCase("size 3  (wave-1 edge)",   [7, 2, 5]),
        SortCase("size 5  (wave-2 edge)",   [9, 1, 8, 3, 6]),
        SortCase("size 11 (wave-3 edge)",   [11, 4, 7, 2, 9, 1, 6, 3, 10, 5, 8]),
        SortCase("size 21 (wave-4 edge)",   list(range(21, 0, -1))),
        SortCase("size 21 (wave-4 edge)",   random.sample(range(21), 21)),
        SortCase("size 21 (wave-4 edge)",   random.sample(range(21), 21)),
        SortCase("size 21 (wave-4 edge)",   random.sample(range(21), 21)),
        SortCase("size 21 (wave-4 edge)",   random.sample(range(21), 21)),
        SortCase("size 21 (wave-4 edge)",   random.sample(range(21), 21)),
        SortCase("size 21 (wave-4 edge)",   random.sample(range(21), 21)),
        SortCase("size 21 (wave-4 edge)",   random.sample(range(21), 21)),
        # ── medium random ─────────────────────────────────────────────
        SortCase("15 random unique",        random.sample(range(1, 200), 15)),
        SortCase("16 random unique",        random.sample(range(1, 200), 16)),
        SortCase("17 random unique",        random.sample(range(1, 200), 17)),
        # ── larger fixed ─────────────────────────────────────────────
        SortCase("50  descending",          list(range(50, 0, -1))),
        SortCase("100 ascending",           list(range(1, 101))),
        SortCase("100 random",              random.sample(range(1, 1000), 100)),
        # ── stress small ─────────────────────────────────────────────
        SortCase("200 random",              random.sample(range(1, 10000), 200)),
    ]
    size = len(cases)
    return cases


VOLUME_SIZES: list[int] = [1000, 2000, 3000, 5000]

ERROR_CASES: list[ErrorCase] = [
    ErrorCase("negative number",   ["-1", "2", "3"],  1),
    ErrorCase("float  input",      ["1.5", "2"],       1),
    ErrorCase("non-numeric",       ["abc", "2"],       1),
    ErrorCase("empty string arg",  [""],               1),
    ErrorCase("no arguments",      [],                 1),
]
