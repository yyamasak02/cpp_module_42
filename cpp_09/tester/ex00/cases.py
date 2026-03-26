"""
Test case definitions for ex00: Bitcoin Exchange.

BtcCase fields
  label              : human-readable name
  input_content      : file content written to a temp file (None = no file arg)
  must_contain       : regex patterns that MUST appear in combined stdout+stderr
  must_not_contain   : regex patterns that must NOT appear
  nonzero_exit       : (unused, kept for future use)

Error message format required by the subject:
  Too large  : "Error: too large a number."
  Negative   : "Error: not a positive number."
  Bad format : "Error: bad input => <original line>"
  No file    : "Error: could not open file."

Known exchange rates from data.csv:
  2011-01-01: 0.3  (used for 2011-01-03, 2011-01-05, etc.)
  2011-01-07: 0.32 (used for 2011-01-09)
  2012-01-11: 7.1  (exact match)
  2016-02-29: in DB range (2016 is a leap year)
"""

from dataclasses import dataclass, field
from typing import Optional


@dataclass
class BtcCase:
    label:            str
    input_content:    Optional[str]
    must_contain:     list[str] = field(default_factory=list)
    must_not_contain: list[str] = field(default_factory=list)


# ── Subject example input ──────────────────────────────────────────────────────
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


def all_cases() -> list[BtcCase]:
    return [
        # ── no argument ────────────────────────────────────────────────────────
        BtcCase(
            label="no argument → Error: could not open file.",
            input_content=None,
            must_contain=[r"Error: could not open file\."],
        ),

        # ── subject example: valid calculations ───────────────────────────────
        BtcCase(
            label="subject example: 2011-01-03 | 3  → 0.9",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-03 => 3 = 0\.9"],
        ),
        BtcCase(
            label="subject example: 2011-01-03 | 2  → 0.6",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-03 => 2 = 0\.6"],
        ),
        BtcCase(
            label="subject example: 2011-01-03 | 1  → 0.3",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-03 => 1 = 0\.3"],
        ),
        BtcCase(
            label="subject example: 2011-01-03 | 1.2 → 0.36",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-03 => 1\.2 = 0\.36"],
        ),
        BtcCase(
            label="subject example: 2011-01-09 | 1  → 0.32",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2011-01-09 => 1 = 0\.32"],
        ),
        BtcCase(
            label="subject example: 2012-01-11 | 1  → 7.1",
            input_content=SUBJECT_INPUT,
            must_contain=[r"2012-01-11 => 1 = 7\.1"],
        ),

        # ── subject example: error lines (exact subject format) ────────────────
        BtcCase(
            label="negative value → Error: not a positive number.",
            input_content=SUBJECT_INPUT,
            must_contain=[r"Error: not a positive number\."],
        ),
        BtcCase(
            label="bad date 2001-42-42 → Error: bad input => 2001-42-42",
            input_content=SUBJECT_INPUT,
            must_contain=[r"Error: bad input => 2001-42-42"],
        ),
        BtcCase(
            label="value 2147483648 → Error: too large a number.",
            input_content=SUBJECT_INPUT,
            must_contain=[r"Error: too large a number\."],
        ),

        # ── edge: value 0 ─────────────────────────────────────────────────────
        BtcCase(
            label="value 0 → valid, result 0",
            input_content="date | value\n2012-01-11 | 0\n",
            must_contain=[r"2012-01-11 => 0 = 0"],
            must_not_contain=[r"Error:"],
        ),

        # ── edge: value 1000 (max valid) ──────────────────────────────────────
        BtcCase(
            label="value 1000 → valid, result 7100",
            input_content="date | value\n2012-01-11 | 1000\n",
            must_contain=[r"2012-01-11 => 1000 = 7100"],
            must_not_contain=[r"Error:"],
        ),

        # ── edge: value 1001 (over limit) ─────────────────────────────────────
        BtcCase(
            label="value 1001 → Error: too large a number.",
            input_content="date | value\n2012-01-11 | 1001\n",
            must_contain=[r"Error: too large a number\."],
        ),

        # ── edge: value -0.1 ──────────────────────────────────────────────────
        BtcCase(
            label="value -0.1 → Error: not a positive number.",
            input_content="date | value\n2012-01-11 | -0.1\n",
            must_contain=[r"Error: not a positive number\."],
        ),

        # ── edge: float value ─────────────────────────────────────────────────
        BtcCase(
            label="float value 3.14 → valid",
            input_content="date | value\n2012-01-11 | 3.14\n",
            must_contain=[r"2012-01-11 => 3\.14 = "],
            must_not_contain=[r"Error:"],
        ),

        # ── date not in DB: use closest lower ─────────────────────────────────
        BtcCase(
            label="2011-01-03 not in DB → uses 2011-01-01 (rate 0.3)",
            input_content="date | value\n2011-01-03 | 10\n",
            must_contain=[r"2011-01-03 => 10 = 3"],
        ),

        # ── malformed lines ───────────────────────────────────────────────────
        BtcCase(
            label="no pipe separator → Error: bad input =>",
            input_content="date | value\n20120111\n",
            must_contain=[r"Error: bad input => 20120111"],
        ),
        BtcCase(
            label="missing value after pipe → Error: bad input =>",
            input_content="date | value\n2012-01-11 | \n",
            must_contain=[r"Error: bad input =>"],
        ),
        BtcCase(
            label="non-numeric value → Error: bad input =>",
            input_content="date | value\n2012-01-11 | abc\n",
            must_contain=[r"Error: bad input =>"],
        ),
        BtcCase(
            label="invalid month 13 → Error: bad input =>",
            input_content="date | value\n2012-13-01 | 1\n",
            must_contain=[r"Error: bad input =>"],
        ),

        # ── leap year tests ───────────────────────────────────────────────────
        BtcCase(
            label="leap year 2024-02-29 (div by 4) → valid",
            input_content="date | value\n2024-02-29 | 1\n",
            must_not_contain=[r"Error:"],
        ),
        BtcCase(
            label="non-leap 2025-02-29 → Error: bad input =>",
            input_content="date | value\n2025-02-29 | 1\n",
            must_contain=[r"Error: bad input => 2025-02-29"],
        ),
        BtcCase(
            label="leap year 2000-02-29 (div by 400) → valid date but before DB",
            input_content="date | value\n2000-02-29 | 1\n",
            must_contain=[r"Error: bad input => 2000-02-29"],
        ),
        BtcCase(
            label="non-leap 2100-02-29 (div by 100 not 400) → Error: bad input =>",
            input_content="date | value\n2100-02-29 | 1\n",
            must_contain=[r"Error: bad input => 2100-02-29"],
        ),
        BtcCase(
            label="leap year 2016-02-29 (div by 4, in DB) → valid",
            input_content="date | value\n2016-02-29 | 1\n",
            must_not_contain=[r"Error:"],
        ),

        # ── nonexistent file ──────────────────────────────────────────────────
        BtcCase(
            label="nonexistent file → some error message",
            input_content="__NONEXISTENT_FILE__",
            must_contain=[r"(?i)(error|fail|cannot|could.not|no such)"],
        ),
    ]


# ── Valgrind input ─────────────────────────────────────────────────────────────
VALGRIND_INPUT = """\
date | value
2011-01-03 | 3
2012-01-11 | 1
2012-01-11 | -1
2001-42-42
2012-01-11 | 2147483648
"""
