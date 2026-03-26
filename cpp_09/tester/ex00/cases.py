"""
Test case definitions for ex00: Bitcoin Exchange.

BtcCase fields
  label              : human-readable name
  input_content      : file content written to a temp file (None = no file arg)
  must_contain       : regex patterns that MUST appear in combined stdout+stderr
  must_not_contain   : regex patterns that must NOT appear
  nonzero_exit       : True if we expect a non-zero exit code

Known exchange rates from data.csv (used for expected calculation results):
  2011-01-01: 0.3  (closest-lower for 2011-01-03, 2011-01-05, 2011-01-06)
  2011-01-07: 0.32 (closest-lower for 2011-01-09)
  2012-01-11: 7.1  (exact match)
"""

from dataclasses import dataclass, field
from typing import Optional


@dataclass
class BtcCase:
    label:            str
    input_content:    Optional[str]  # None → run ./btc with no file argument
    must_contain:     list[str]      = field(default_factory=list)
    must_not_contain: list[str]      = field(default_factory=list)
    nonzero_exit:     bool           = False


# ── Subject example (input from the subject PDF) ──────────────────────────────
SUBJECT_INPUT = """\
date | value
2011-01-03 | 3
2011-01-03 | 2
2011-01-03 | 1
2011-01-03 | 1.2
2011-01-09 | 1
2012-01-11 | -1
2001-42-42
2012-01-11 | 1
2012-01-11 | 2147483648
"""

# ── All test cases ─────────────────────────────────────────────────────────────

def all_cases() -> list[BtcCase]:
    return [
        # ── no argument ────────────────────────────────────────────────────────
        BtcCase(
            label="no argument → error message",
            input_content=None,
            must_contain=[r"(?i)error"],
        ),

        # ── subject example: valid calculations ───────────────────────────────
        BtcCase(
            label="subject example: 2011-01-03 | 3  → 0.9",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-03\s*=>\s*3\s*=\s*0\.9"],
        ),
        BtcCase(
            label="subject example: 2011-01-03 | 2  → 0.6",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-03\s*=>\s*2\s*=\s*0\.6"],
        ),
        BtcCase(
            label="subject example: 2011-01-03 | 1  → 0.3",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-03\s*=>\s*1\s*=\s*0\.3"],
        ),
        BtcCase(
            label="subject example: 2011-01-03 | 1.2 → 0.36",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-03\s*=>\s*1\.2\s*=\s*0\.36"],
        ),
        BtcCase(
            label="subject example: 2011-01-09 | 1  → 0.32",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-09\s*=>\s*1\s*=\s*0\.32"],
        ),
        BtcCase(
            label="subject example: 2012-01-11 | 1  → 7.1",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2012-01-11\s*=>\s*1\s*=\s*7\.1"],
        ),

        # ── subject example: error lines ──────────────────────────────────────
        BtcCase(
            label="negative value → error (not positive / too small)",
            input_content=SUBJECT_INPUT,
            must_contain=[r"(?i)error.*(not.*positive|too small|negative)"],
        ),
        BtcCase(
            label="bad date 2001-42-42 → error (bad input / invalid / not valid)",
            input_content=SUBJECT_INPUT,
            must_contain=[r"(?i)error.*(bad|invalid|not valid)"],
        ),
        BtcCase(
            label="value 2147483648 → error (too large)",
            input_content=SUBJECT_INPUT,
            must_contain=[r"(?i)error.*too large"],
        ),

        # ── edge: value 0 ─────────────────────────────────────────────────────
        BtcCase(
            label="value 0 → valid, result 0",
            input_content="date | value\n2012-01-11 | 0\n",
            must_contain=[r"2012-01-11\s*=>\s*0\s*=\s*0"],
            must_not_contain=[r"(?i)error"],
        ),

        # ── edge: value 1000 (max valid) ──────────────────────────────────────
        BtcCase(
            label="value 1000 → valid, result 7100",
            input_content="date | value\n2012-01-11 | 1000\n",
            must_contain=[r"2012-01-11\s*=>\s*1000\s*=\s*7100"],
            must_not_contain=[r"(?i)error"],
        ),

        # ── edge: value 1001 (just over limit) ────────────────────────────────
        BtcCase(
            label="value 1001 → error too large",
            input_content="date | value\n2012-01-11 | 1001\n",
            must_contain=[r"(?i)error.*too large"],
        ),

        # ── edge: value -0.1 ──────────────────────────────────────────────────
        BtcCase(
            label="value -0.1 → error not positive",
            input_content="date | value\n2012-01-11 | -0.1\n",
            must_contain=[r"(?i)error.*(not.*positive|too small|negative)"],
        ),

        # ── edge: float value ─────────────────────────────────────────────────
        BtcCase(
            label="float value 3.14 → valid",
            input_content="date | value\n2012-01-11 | 3.14\n",
            must_contain=[r"2012-01-11\s*=>\s*3\.14\s*="],
            must_not_contain=[r"(?i)error"],
        ),

        # ── date not in DB: use closest lower ─────────────────────────────────
        BtcCase(
            label="2011-01-03 not in DB → uses 2011-01-01 (rate 0.3)",
            input_content="date | value\n2011-01-03 | 10\n",
            must_contain=[r"2011-01-03\s*=>\s*10\s*=\s*3"],
        ),

        # ── malformed lines ───────────────────────────────────────────────────
        BtcCase(
            label="no pipe separator → error",
            input_content="date | value\n20120111\n",
            must_contain=[r"(?i)error"],
        ),
        BtcCase(
            label="missing value after pipe → error",
            input_content="date | value\n2012-01-11 | \n",
            must_contain=[r"(?i)error"],
        ),
        BtcCase(
            label="non-numeric value → error",
            input_content="date | value\n2012-01-11 | abc\n",
            must_contain=[r"(?i)error"],
        ),
        BtcCase(
            label="invalid month 13 → error",
            input_content="date | value\n2012-13-01 | 1\n",
            must_contain=[r"(?i)error"],
        ),
        BtcCase(
            label="non-leap year Feb 29 → error",
            input_content="date | value\n2025-02-29 | 1\n",
            must_contain=[r"(?i)error"],
        ),
        BtcCase(
            label="leap year Feb 29 → valid",
            input_content="date | value\n2024-02-29 | 1\n",
            must_not_contain=[r"(?i)error"],
        ),

        # ── nonexistent file ──────────────────────────────────────────────────
        BtcCase(
            label="nonexistent file → error (error/fail/cannot/no such)",
            input_content="__NONEXISTENT_FILE__",  # special sentinel
            must_contain=[r"(?i)(error|fail|cannot|could.not|no such)"],
        ),
    ]


# ── Valgrind input (simple, representative) ───────────────────────────────────
VALGRIND_INPUT = """\
date | value
2011-01-03 | 3
2012-01-11 | 1
2012-01-11 | -1
2001-42-42
2012-01-11 | 2147483648
"""
