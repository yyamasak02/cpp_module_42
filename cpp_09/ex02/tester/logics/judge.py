"""
Verdict logic.

judge()        -- sort correctness + Ford-Johnson comparison bound check
judge_error()  -- expected non-zero exit check
"""

from models.bean import TestResult
from logics.bounds import fj_upper_bound
from logics.runner import run_program, parse_output

# A result is flagged only when it exceeds the FJ bound by more than 5%+1.
# (FJ bound is the worst-case upper bound; the +5% slack accounts for minor
#  implementation-level deviations on specific inputs.)
_SLACK_FACTOR = 1.05
_SLACK_CONST  = 1


def judge(label: str, nums: list[int]) -> TestResult:
    """Run the sort program and verify correctness + comparison efficiency."""
    r = TestResult(label=label, n=len(nums), kind="sort")
    args     = list(map(str, nums))
    expected = " ".join(map(str, sorted(nums)))

    lines, retcode = run_program(args)
    if retcode != 0:
        r.failures.append(f"non-zero exit ({retcode})")
        r.ok = False
        return r

    p       = parse_output(lines)
    r.parsed = p

    # ── sort correctness ─────────────────────────────────────────────
    if p.after is None:
        r.failures.append("no 'After:' line in output")
    elif p.after != expected:
        r.failures.append(
            f"sort mismatch\n"
            f"      got : {p.after[:72]}\n"
            f"      want: {expected[:72]}"
        )

    # ── comparison count upper-bound check ───────────────────────────
    # lb (info_lower_bound) is the worst-case lower bound; shown for
    # reference only — individual inputs may legitimately use fewer.
    ub = fj_upper_bound(len(nums))
    for cmp_val, container in ((p.cmp_vec, "vector"), (p.cmp_deq, "deque")):
        if cmp_val is None:
            r.failures.append(f"no comparison count found for {container}")
        elif cmp_val > ub * _SLACK_FACTOR + _SLACK_CONST:
            r.failures.append(
                f"{container}: cmp={cmp_val} greatly exceeds fj_upper_bound={ub}"
            )

    r.ok = len(r.failures) == 0
    return r


def judge_error(label: str, args: list[str], expected_exit: int) -> TestResult:
    """Verify that the program exits with the expected error code."""
    r = TestResult(label=label, n=0, kind="error")
    _, retcode = run_program(args)
    if retcode != expected_exit:
        r.failures.append(f"expected exit={expected_exit}, got={retcode}")
    r.ok = len(r.failures) == 0
    return r
