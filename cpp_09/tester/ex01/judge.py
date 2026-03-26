"""
Verdict logic and main entry point for ex01: Reverse Polish Notation.

run_ex01() -> (passed, failed)
"""

import os
import re

from common.builder  import build
from common.logger   import TestLogger, TestResult
from common.valgrind import check_valgrind
from common.color    import Color
from ex01.cases      import all_cases, VALGRIND_ARGS, RpnCase
from ex01.runner     import run_rpn, WORK_SPACE, PROGRAM_FILE

_TRUNC = 300


def _truncate(s: str, n: int = _TRUNC) -> str:
    s = s.strip()
    return s[:n] + " …(truncated)" if len(s) > n else s


# ── single-case verdict ───────────────────────────────────────────────────────

def judge(case: RpnCase) -> TestResult:
    r = TestResult(label=case.label)
    stdout, stderr, retcode = run_rpn(case.args)
    combined = stdout + "\n" + stderr

    # ── error expected ────────────────────────────────────────────────────────
    if case.expect_error:
        has_error = (
            retcode != 0
            or bool(stderr)
            or bool(re.search(r"(?i)error", combined))
        )
        if not has_error:
            r.failures.append(
                f"Expected : non-zero exit  OR  non-empty stderr  OR  'Error' in output\n"
                f"Actual   :\n"
                f"  exit code : {retcode}\n"
                f"  stdout    : {_truncate(stdout) or '(empty)'}\n"
                f"  stderr    : {_truncate(stderr) or '(empty)'}"
            )
        r.ok = len(r.failures) == 0
        return r

    # ── stdout pattern check (MULTILINE: '^42$' matches any line) ─────────────
    if case.expected_stdout is not None:
        if not re.search(case.expected_stdout, stdout, re.MULTILINE):
            r.failures.append(
                f"Expected stdout : (pattern) {case.expected_stdout}\n"
                f"Actual stdout   : {_truncate(stdout) or '(empty)'}"
            )

    # ── must_contain / must_not_contain ───────────────────────────────────────
    for pat in case.must_contain:
        if not re.search(pat, combined):
            r.failures.append(
                f"Expected : output matches pattern\n"
                f"  Pattern: {pat}\n"
                f"Actual output:\n"
                f"  {_truncate(combined)}"
            )

    for pat in case.must_not_contain:
        if re.search(pat, combined):
            r.failures.append(
                f"Expected : pattern NOT in output\n"
                f"  Pattern: {pat}\n"
                f"Actual output:\n"
                f"  {_truncate(combined)}"
            )

    if retcode != 0:
        r.failures.append(
            f"Expected exit code : 0\n"
            f"Actual exit code   : {retcode}"
        )

    r.ok = len(r.failures) == 0
    return r


# ── valgrind section ──────────────────────────────────────────────────────────

def judge_valgrind(log: TestLogger) -> tuple[int, int]:
    os.chdir(WORK_SPACE)

    cases = [
        ("valid expression", VALGRIND_ARGS),
        ("error expression", ["(1 + 1)"]),
        ("no argument",      []),
    ]
    for label, args in cases:
        cmd = [f"./{PROGRAM_FILE}", *args]
        ok, msg = check_valgrind(cmd)
        r = TestResult(label=f"valgrind: {label}", ok=ok)
        if not ok:
            r.failures.append(
                f"Expected : no memory leaks or errors\n"
                f"Actual   : {msg}"
            )
        log.record(r)

    return log.flush_section("VALGRIND (memory leak check)", expected_total=len(cases))


# ── main entry ────────────────────────────────────────────────────────────────

def run_ex01() -> tuple[int, int]:
    print(f"\n{Color.BOLD}{'━' * 72}{Color.RESET}")
    print(f"{Color.BOLD}  EX01: Reverse Polish Notation{Color.RESET}")
    print(f"{Color.BOLD}{'━' * 72}{Color.RESET}")
    print(f"\n{Color.BOLD}Building {PROGRAM_FILE}...{Color.RESET}")

    ok, err = build(WORK_SPACE, PROGRAM_FILE)
    if not ok:
        print(f"{Color.RED}[ERROR] {err}{Color.RESET}\n")
        return 0, 1

    cases = all_cases()
    fixed_cases = [c for c in cases if c.expected_stdout is not None
                   or c.must_contain or c.must_not_contain or c.expect_error]
    free_cases  = [c for c in cases if c not in fixed_cases]

    log = TestLogger()
    total_p = total_f = 0

    for c in fixed_cases:
        log.record(judge(c))
    p, f = log.flush_section("FUNCTIONAL TESTS", expected_total=len(fixed_cases))
    total_p += p
    total_f += f

    if free_cases:
        for c in free_cases:
            stdout, stderr, retcode = run_rpn(c.args)
            r = TestResult(
                label=c.label,
                ok=True,
                extra=f"exit={retcode}  stdout={stdout!r}",
            )
            log.record(r)
        p, f = log.flush_section("EDGE CASES (no assertion)", expected_total=len(free_cases))
        total_p += p
        total_f += f

    p, f = judge_valgrind(log)
    total_p += p
    total_f += f

    log.print_total(total_p, total_f)
    return total_p, total_f
