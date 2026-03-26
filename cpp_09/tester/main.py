#!/usr/bin/env python3
"""
C++ Module 09 — Test Runner

Usage:
  python3 main.py          # run all exercises
  python3 main.py ex00     # run only ex00
  python3 main.py ex01     # run only ex01
  python3 main.py ex02     # run only ex02
"""

import os
import sys

# Add tester root to sys.path so sub-modules can import from common/
_TESTER_ROOT = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, _TESTER_ROOT)

from common.color  import Color
from ex00.judge    import run_ex00
from ex01.judge    import run_ex01
from ex02.main     import run_ex02


def main() -> None:
    target = sys.argv[1] if len(sys.argv) > 1 else "all"

    if target not in ("all", "ex00", "ex01", "ex02"):
        print(f"Usage: python3 main.py [all|ex00|ex01|ex02]")
        sys.exit(1)

    grand_p = grand_f = 0

    if target in ("all", "ex00"):
        p, f = run_ex00()
        grand_p += p
        grand_f += f

    if target in ("all", "ex01"):
        p, f = run_ex01()
        grand_p += p
        grand_f += f

    if target in ("all", "ex02"):
        p, f = run_ex02()
        grand_p += p
        grand_f += f

    bar   = "█" * 80
    color = Color.GREEN if grand_f == 0 else Color.RED
    print(f"\n{Color.BOLD}{bar}{Color.RESET}")
    print(
        f"{color}{Color.BOLD}"
        f"  GRAND TOTAL  {grand_p} passed  /  {grand_f} failed"
        f"{Color.RESET}"
    )
    print(f"{Color.BOLD}{bar}{Color.RESET}\n")


if __name__ == "__main__":
    main()
