"""
Buffered section logger.

Usage:
    log = TestLogger()
    log.record(some_TestResult)
    passed, failed = log.flush_section("Section title", expected_total=N)
    log.print_total(total_pass, total_fail)
"""

from dataclasses import dataclass, field
from typing import Optional
from common.color import Color


@dataclass
class TestResult:
    label:    str
    ok:       bool        = False
    failures: list[str]   = field(default_factory=list)
    extra:    str         = ""     # shown (dim) only when ok=True


class TestLogger:
    def __init__(self) -> None:
        self._buf: list[TestResult] = []

    def record(self, result: TestResult) -> None:
        self._buf.append(result)

    def flush_section(
        self,
        title: str,
        expected_total: Optional[int] = None,
    ) -> tuple[int, int]:
        passed = sum(1 for r in self._buf if r.ok)
        failed = len(self._buf) - passed
        total  = expected_total if expected_total is not None else len(self._buf)

        bar = "─" * 72
        print(f"\n{Color.BOLD}{bar}{Color.RESET}")
        print(f"{Color.BOLD}  {title}{Color.RESET}")
        print(f"{Color.BOLD}{bar}{Color.RESET}")

        for r in self._buf:
            mark = f"{Color.GREEN}✓{Color.RESET}" if r.ok else f"{Color.RED}✗{Color.RESET}"
            print(f"  {mark} {r.label}")
            if not r.ok:
                for msg in r.failures:
                    for ln in msg.splitlines():
                        print(f"      {Color.RED}{ln}{Color.RESET}")
            elif r.extra:
                print(f"      {Color.DIM}{r.extra}{Color.RESET}")

        c = Color.GREEN if passed == total else Color.RED
        print(f"\n  {c}{Color.BOLD}{passed}/{total} passed{Color.RESET}\n")
        self._buf.clear()
        return passed, failed

    def print_total(self, passed: int, failed: int) -> None:
        bar   = "═" * 72
        color = Color.GREEN if failed == 0 else Color.RED
        print(f"{Color.BOLD}{bar}{Color.RESET}")
        print(f"{color}{Color.BOLD}  TOTAL  {passed} passed  /  {failed} failed{Color.RESET}")
        print(f"{Color.BOLD}{bar}{Color.RESET}\n")
