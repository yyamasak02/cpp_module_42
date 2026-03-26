"""
Test case definitions for ex01: Reverse Polish Notation.

RpnCase fields
  label          : human-readable name
  args           : list of strings passed as argv (usually one expression string)
  expected_stdout: regex pattern the stdout must match (None = don't check)
  must_contain   : patterns that MUST appear in stdout
  must_not_contain: patterns that must NOT appear in stdout
  expect_error   : True if we expect a non-zero exit or "Error" on stderr
"""

from dataclasses import dataclass, field
from typing import Optional


@dataclass
class RpnCase:
    label:            str
    args:             list[str]
    expected_stdout:  Optional[str]    = None   # exact stdout line (regex)
    must_contain:     list[str]        = field(default_factory=list)
    must_not_contain: list[str]        = field(default_factory=list)
    expect_error:     bool             = False   # True → stderr/exit non-zero expected


def all_cases() -> list[RpnCase]:
    return [
        # ── subject examples (must produce the exact result) ───────────────────
        RpnCase(
            label='subject: "8 9 * 9 - 9 - 9 - 4 - 1 +" → 42',
            args=["8 9 * 9 - 9 - 9 - 4 - 1 +"],
            expected_stdout=r"^42$",
        ),
        RpnCase(
            label='subject: "7 7 * 7 -" → 42',
            args=["7 7 * 7 -"],
            expected_stdout=r"^42$",
        ),
        RpnCase(
            label='subject: "1 2 * 2 / 2 * 2 4 - +" → 0',
            args=["1 2 * 2 / 2 * 2 4 - +"],
            expected_stdout=r"^0$",
        ),
        RpnCase(
            label='subject: "(1 + 1)" → Error',
            args=["(1 + 1)"],
            expect_error=True,
        ),

        # ── basic arithmetic ───────────────────────────────────────────────────
        RpnCase(
            label="addition: 3 4 + → 7",
            args=["3 4 +"],
            expected_stdout=r"^7$",
        ),
        RpnCase(
            label="subtraction: 9 3 - → 6",
            args=["9 3 -"],
            expected_stdout=r"^6$",
        ),
        RpnCase(
            label="multiplication: 6 7 * → 42",
            args=["6 7 *"],
            expected_stdout=r"^42$",
        ),
        RpnCase(
            label="division: 8 4 / → 2",
            args=["8 4 /"],
            expected_stdout=r"^2$",
        ),

        # ── chained operations ─────────────────────────────────────────────────
        RpnCase(
            label="chained: 2 3 + 4 * → 20",
            args=["2 3 + 4 *"],
            expected_stdout=r"^20$",
        ),
        RpnCase(
            label="chained: 5 1 2 + 4 * + 3 - → 14",
            args=["5 1 2 + 4 * + 3 -"],
            expected_stdout=r"^14$",
        ),

        # ── result can exceed 10 (only inputs < 10) ────────────────────────────
        RpnCase(
            label="result > 10: 9 9 * → 81",
            args=["9 9 *"],
            expected_stdout=r"^81$",
        ),
        RpnCase(
            label="result > 10: 9 9 + 9 9 + * → 324",
            args=["9 9 + 9 9 + *"],
            expected_stdout=r"^324$",
        ),

        # ── single number (edge case) ─────────────────────────────────────────
        RpnCase(
            label="single number: 5 → some implementations accept, some reject",
            args=["5"],
            # We don't assert pass/fail; just that it doesn't crash unexpectedly.
            # Both exit 0 (output 5) and error are acceptable per subject.
        ),

        # ── error: too few operands (operator with one arg) ───────────────────
        RpnCase(
            label="too few operands: 3 + → Error",
            args=["3 +"],
            expect_error=True,
        ),

        # ── error: too many operands (leftover on stack) ──────────────────────
        RpnCase(
            label="too many operands: 3 4 5 + → Error",
            args=["3 4 5 +"],
            expect_error=True,
        ),

        # ── error: invalid token ───────────────────────────────────────────────
        RpnCase(
            label="invalid token 'a': 1 2 a + → Error",
            args=["1 2 a +"],
            expect_error=True,
        ),

        # ── error: division by zero ────────────────────────────────────────────
        RpnCase(
            label="division by zero: 5 0 / → Error",
            args=["5 0 /"],
            expect_error=True,
        ),

        # ── error: brackets are not supported ─────────────────────────────────
        RpnCase(
            label="brackets '(3 4 +)' → Error",
            args=["(3 4 +)"],
            expect_error=True,
        ),

        # ── error: empty argument ─────────────────────────────────────────────
        RpnCase(
            label="empty argument '' → Error",
            args=[""],
            expect_error=True,
        ),

        # ── error: no argument ────────────────────────────────────────────────
        RpnCase(
            label="no argument → Error",
            args=[],
            expect_error=True,
        ),

        # ── number >= 10 as token (subject says input nums < 10) ──────────────
        RpnCase(
            label="token 10 (>=10 input) → Error",
            args=["10 2 +"],
            expect_error=True,
        ),
    ]


# Input used for valgrind check (known-good expression)
VALGRIND_ARGS = ["8 9 * 9 - 9 - 9 - 4 - 1 +"]
