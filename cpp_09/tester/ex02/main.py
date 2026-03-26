"""
Main entry point for ex02: PmergeMe tester.

run_ex02() -> (passed, failed)  ← called from tester/main.py
"""

import os
import random

from common.builder  import build
from common.color    import Color
from common.valgrind import check_valgrind
from common.logger   import TestResult as CommonTestResult
from ex02.cases      import fixed_cases, VOLUME_SIZES, ERROR_CASES
from ex02.judge      import judge, judge_error
from ex02.logger     import Ex02Logger
from ex02.runner     import WORK_SPACE, PROGRAM_FILE


def _valgrind_section(log: Ex02Logger) -> tuple[int, int]:
    """Run valgrind on a small representative input."""
    os.chdir(WORK_SPACE)
    cases = [
        ("5 elements",  ["3", "5", "9", "7", "4"]),
        ("no argument", []),
        ("invalid arg", ["-1", "2"]),
    ]
    from ex02.bean import TestResult as Ex02TestResult
    results = []
    for label, args in cases:
        cmd = [f"./{PROGRAM_FILE}", *args]
        ok, msg = check_valgrind(cmd)
        r = Ex02TestResult(label=f"valgrind: {label}", n=0, kind="error")
        r.ok = ok
        if not ok:
            r.failures.append(
                f"Expected : no memory leaks or errors\n"
                f"Actual   : {msg}"
            )
        log.record(r)
    return log.flush_section("VALGRIND (memory leak check)", expected_total=len(cases))


def run_ex02() -> tuple[int, int]:
    print(f"\n{Color.BOLD}{'━' * 80}{Color.RESET}")
    print(f"{Color.BOLD}  EX02: PmergeMe{Color.RESET}")
    print(f"{Color.BOLD}{'━' * 80}{Color.RESET}")
    print(f"\n{Color.BOLD}Building {PROGRAM_FILE}...{Color.RESET}")

    ok, err = build(WORK_SPACE, PROGRAM_FILE)
    if not ok:
        print(f"{Color.RED}[ERROR] {err}{Color.RESET}\n")
        return 0, 1

    log = Ex02Logger()
    total_p = total_f = 0

    # Fixed test cases
    for case in fixed_cases():
        log.record(judge(case.label, case.nums))
    p, f = log.flush_section("FIXED TEST CASES (28)", expected_total=28)
    total_p += p
    total_f += f

    # Volume tests
    for size in VOLUME_SIZES:
        nums = random.sample(range(1, size * 10), size)
        log.record(judge(f"{size} random", nums))
    p, f = log.flush_section(
        f"VOLUME TESTS  ({VOLUME_SIZES[0]}+ elements)",
        expected_total=len(VOLUME_SIZES),
    )
    total_p += p
    total_f += f

    # Error / invalid input
    for case in ERROR_CASES:
        log.record(judge_error(case.label, case.args, case.expected_exit))
    p, f = log.flush_section(
        "ERROR / INVALID INPUT TESTS",
        expected_total=len(ERROR_CASES),
    )
    total_p += p
    total_f += f

    # Valgrind
    p, f = _valgrind_section(log)
    total_p += p
    total_f += f

    log.print_total(total_p, total_f)
    return total_p, total_f
