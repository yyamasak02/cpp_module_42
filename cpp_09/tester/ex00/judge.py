"""
Verdict logic and main entry point for ex00: Bitcoin Exchange.

run_ex00() -> (passed, failed)
"""

import os
import re
import tempfile

from common.builder  import build
from common.logger   import TestLogger, TestResult
from common.valgrind import check_valgrind
from common.color    import Color
from ex00.cases      import all_cases, VALGRIND_INPUT, BtcCase
from ex00.runner     import run_btc, WORK_SPACE, PROGRAM_FILE

_TRUNC = 400   # max chars of output shown in failure messages


def _truncate(s: str, n: int = _TRUNC) -> str:
    s = s.strip()
    return s[:n] + " …(truncated)" if len(s) > n else s


# ── single-case verdict ───────────────────────────────────────────────────────

def judge(case: BtcCase) -> TestResult:
    r = TestResult(label=case.label)
    output, _code = run_btc(case.input_content)

    for pat in case.must_contain:
        if not re.search(pat, output):
            r.failures.append(
                f"Expected : output matches pattern\n"
                f"  Pattern: {pat}\n"
                f"Actual output:\n"
                f"  {_truncate(output)}"
            )

    for pat in case.must_not_contain:
        if re.search(pat, output):
            r.failures.append(
                f"Expected : pattern NOT in output\n"
                f"  Pattern: {pat}\n"
                f"Actual output:\n"
                f"  {_truncate(output)}"
            )

    r.ok = len(r.failures) == 0
    return r


# ── valgrind section ──────────────────────────────────────────────────────────

def judge_valgrind(log: TestLogger) -> tuple[int, int]:
    os.chdir(WORK_SPACE)

    tmp = tempfile.NamedTemporaryFile(
        mode="w", suffix=".txt", delete=False, dir=WORK_SPACE
    )
    try:
        tmp.write(VALGRIND_INPUT)
        tmp.flush()
        tmp.close()

        cases = [
            ("no argument",      [f"./{PROGRAM_FILE}"]),
            ("valid input file", [f"./{PROGRAM_FILE}", tmp.name]),
        ]
        for label, cmd in cases:
            ok, msg = check_valgrind(cmd)
            r = TestResult(label=f"valgrind: {label}", ok=ok)
            if not ok:
                r.failures.append(
                    f"Expected : no memory leaks or errors\n"
                    f"Actual   : {msg}"
                )
            log.record(r)
    finally:
        try:
            os.unlink(tmp.name)
        except OSError:
            pass

    return log.flush_section("VALGRIND (memory leak check)", expected_total=2)


# ── main entry ────────────────────────────────────────────────────────────────

def run_ex00() -> tuple[int, int]:
    print(f"\n{Color.BOLD}{'━' * 72}{Color.RESET}")
    print(f"{Color.BOLD}  EX00: Bitcoin Exchange{Color.RESET}")
    print(f"{Color.BOLD}{'━' * 72}{Color.RESET}")
    print(f"\n{Color.BOLD}Building {PROGRAM_FILE}...{Color.RESET}")

    ok, err = build(WORK_SPACE, PROGRAM_FILE)
    if not ok:
        print(f"{Color.RED}[ERROR] {err}{Color.RESET}\n")
        return 0, 1

    cases = all_cases()
    log   = TestLogger()
    total_p = total_f = 0

    for c in cases:
        log.record(judge(c))
    p, f = log.flush_section("FUNCTIONAL TESTS", expected_total=len(cases))
    total_p += p
    total_f += f

    p, f = judge_valgrind(log)
    total_p += p
    total_f += f

    log.print_total(total_p, total_f)
    return total_p, total_f
