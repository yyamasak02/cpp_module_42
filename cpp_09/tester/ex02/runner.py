"""
Program execution and raw output parsing for ex02: PmergeMe.
"""

import re
import subprocess

from ex02.bean import ParsedOutput

WORK_SPACE:   str = "/home/yyamasak/Desktop/cursus/cpp_modules/private/cpp_09/ex02"
PROGRAM_FILE: str = "PmergeMe"

_PATTERNS: dict[str, re.Pattern] = {
    "after":    re.compile(r"^\s*After\s*:\s*(?P<numbers>[\d\s]+)\s*$"),
    "time_vec": re.compile(
        r"^\s*Time\s+to\s+process.*?"
        r"(?P<n>\d+)\s+elements\s+with\s+std::vector\s*:\s*(?P<t>[\d.]+)\s*us"
    ),
    "time_deq": re.compile(
        r"^\s*Time\s+to\s+process.*?"
        r"(?P<n>\d+)\s+elements\s+with\s+std::deque\s*:\s*(?P<t>[\d.]+)\s*us"
    ),
    "cmp_vec":  re.compile(r"^\s*Number of comparisons with std::vector\s*:\s*(?P<c>\d+)"),
    "cmp_deq":  re.compile(r"^\s*Number of comparisons with std::deque\s*:\s*(?P<c>\d+)"),
}


def run_program(args: list[str]) -> tuple[list[str], int]:
    """Run the binary with args. Returns (stdout_lines, exit_code)."""
    result = subprocess.run(
        [f"./{PROGRAM_FILE}", *args],
        capture_output=True,
        text=True,
    )
    lines = [line.strip() for line in result.stdout.splitlines() if line.strip()]
    return lines, result.returncode


def parse_output(lines: list[str]) -> ParsedOutput:
    """Parse stdout lines into a ParsedOutput dataclass."""
    out = ParsedOutput()
    for line in lines:
        if m := _PATTERNS["after"].match(line):
            out.after = " ".join(m.group("numbers").split())
        elif m := _PATTERNS["time_vec"].search(line):
            out.time_vec = m.group("t")
        elif m := _PATTERNS["time_deq"].search(line):
            out.time_deq = m.group("t")
        elif m := _PATTERNS["cmp_vec"].match(line):
            out.cmp_vec = int(m.group("c"))
        elif m := _PATTERNS["cmp_deq"].match(line):
            out.cmp_deq = int(m.group("c"))
    return out
