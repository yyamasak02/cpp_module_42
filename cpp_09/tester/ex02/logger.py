"""
Buffered logger for ex02: PmergeMe.
Displays comparison counts colour-coded against Ford-Johnson bounds.
"""

import io
import logging
from logging.handlers import MemoryHandler
from typing import Optional

from ex02.bean   import TestResult
from ex02.bounds import fj_upper_bound, info_lower_bound
from common.color import Color

_RESULT_LEVEL = 25
logging.addLevelName(_RESULT_LEVEL, "RESULT")


def _fmt_cmp(val: Optional[int], n: int) -> str:
    if val is None:
        return f"{Color.RED}???{Color.RESET}"
    lb = info_lower_bound(n)
    ub = fj_upper_bound(n)
    if val <= ub:
        color = Color.GREEN
    elif val <= ub * 1.05 + 1:
        color = Color.YELLOW
    else:
        color = Color.RED
    return f"{color}{val}{Color.RESET} {Color.DIM}≤{ub}(lb:{lb}){Color.RESET}"


class _ResultFormatter(logging.Formatter):
    def format(self, record: logging.LogRecord) -> str:
        r: TestResult = getattr(record, "result")
        mark = f"{Color.GREEN}✓{Color.RESET}" if r.ok else f"{Color.RED}✗{Color.RESET}"

        if r.kind == "error":
            extra = "" if r.ok else f"  {Color.RED}{r.failures[0]}{Color.RESET}"
            return f"  {mark} {r.label:<36}{extra}"

        line = f"  {mark} {r.label:<36} n={r.n:<5}"
        if r.ok and r.parsed:
            p = r.parsed
            vec = (
                f"vec {p.time_vec:>6} us  cmp={_fmt_cmp(p.cmp_vec, r.n)}"
                if p.time_vec else ""
            )
            deq = (
                f"deq {p.time_deq:>6} us  cmp={_fmt_cmp(p.cmp_deq, r.n)}"
                if p.time_deq else ""
            )
            line += f"  {vec}   {deq}"
        else:
            for msg in r.failures:
                for ln in msg.splitlines():
                    line += f"\n      {Color.RED}{ln}{Color.RESET}"
        return line


class Ex02Logger:
    def __init__(self) -> None:
        self._buf: list[TestResult] = []
        self._stream = io.StringIO()

        target = logging.StreamHandler(self._stream)
        target.setFormatter(_ResultFormatter())

        self._mem = MemoryHandler(
            capacity=100_000,
            flushLevel=logging.CRITICAL,
            target=target,
        )
        name = f"pmerge_tester.{id(self)}"
        self._log = logging.getLogger(name)
        self._log.setLevel(_RESULT_LEVEL)
        self._log.handlers.clear()
        self._log.addHandler(self._mem)
        self._log.propagate = False

    def record(self, result: TestResult) -> None:
        lr = self._log.makeRecord(
            name=self._log.name, level=_RESULT_LEVEL,
            fn="", lno=0, msg="", args=(), exc_info=None,
        )
        lr.result = result  # type: ignore[attr-defined]
        self._log.handle(lr)
        self._buf.append(result)

    def flush_section(
        self,
        title: str,
        expected_total: Optional[int] = None,
    ) -> tuple[int, int]:
        passed = sum(1 for r in self._buf if r.ok)
        failed = len(self._buf) - passed
        total  = expected_total if expected_total is not None else len(self._buf)

        self._mem.flush()
        body = self._stream.getvalue()
        self._stream.truncate(0)
        self._stream.seek(0)
        self._buf.clear()

        bar = "─" * 80
        c_sum = Color.GREEN if passed == total else Color.RED
        print(f"\n{Color.BOLD}{bar}{Color.RESET}")
        print(f"{Color.BOLD}  {title}{Color.RESET}")
        print(f"{Color.BOLD}{bar}{Color.RESET}")
        print(body, end="")
        print(f"\n  {c_sum}{Color.BOLD}{passed}/{total} passed{Color.RESET}\n")

        return passed, failed

    def print_total(self, passed: int, failed: int) -> None:
        bar   = "═" * 80
        color = Color.GREEN if failed == 0 else Color.RED
        print(f"{Color.BOLD}{bar}{Color.RESET}")
        print(f"{color}{Color.BOLD}  TOTAL  {passed} passed  /  {failed} failed{Color.RESET}")
        print(f"{Color.BOLD}{bar}{Color.RESET}\n")
