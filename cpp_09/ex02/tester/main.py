"""
PmergeMe tester
  - 21 fixed cases (edge cases, Jacobsthal-boundary sizes, etc.)
  - Volume tests (1000+ elements)
  - Error / invalid-input tests
  - Comparison count verified against Ford-Johnson theoretical upper bound
"""

import random

from builder import build
from cases import fixed_cases, VOLUME_SIZES, ERROR_CASES
from consts.const import PROGRAM_FILE, WORK_SPACE, Color
from logics.judge import judge, judge_error
from logger import TestLogger


def main() -> None:
    print(f"\n{Color.BOLD}Building {PROGRAM_FILE}...{Color.RESET}")
    ok, err = build(WORK_SPACE, PROGRAM_FILE)
    if not ok:
        print(f"{Color.RED}[ERROR] {err}{Color.RESET}")
        return

    log = TestLogger()
    total_pass = total_fail = 0

    # ── fixed test cases ─────────────────────────────────────────────
    for case in fixed_cases():
        log.record(judge(case.label, case.nums))
    p, f = log.flush_section("FIXED TEST CASES (28)", expected_total=28)
    total_pass += p
    total_fail += f

    # ── volume tests ─────────────────────────────────────────────────
    for size in VOLUME_SIZES:
        nums = random.sample(range(1, size * 10), size)
        log.record(judge(f"{size} random", nums))
    p, f = log.flush_section(
        f"VOLUME TESTS  ({VOLUME_SIZES[0]}+ elements)",
        expected_total=len(VOLUME_SIZES),
    )
    total_pass += p
    total_fail += f

    # ── error / invalid-input tests ───────────────────────────────────
    for case in ERROR_CASES:
        log.record(judge_error(case.label, case.args, case.expected_exit))
    p, f = log.flush_section(
        "ERROR / INVALID INPUT TESTS",
        expected_total=len(ERROR_CASES),
    )
    total_pass += p
    total_fail += f

    log.print_total(total_pass, total_fail)


if __name__ == "__main__":
    main()
