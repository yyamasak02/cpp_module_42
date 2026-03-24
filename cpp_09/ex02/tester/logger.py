"""
Buffered test logger.

TestLogger uses Python's logging.handlers.MemoryHandler to accumulate
LogRecords in memory.  Results are NOT written to stdout until
flush_section() is called, so each section's output appears as a
clean, contiguous block — even if the underlying tests run interleaved.

Architecture:
  judge() → TestLogger.record()        stores LogRecord in MemoryHandler
  TestLogger.flush_section()           drains MemoryHandler → StreamHandler
                                       → StringIO, then prints to stdout
"""

import io
import logging
from logging.handlers import MemoryHandler
from typing import Optional

from models.bean import TestResult
from logics.bounds import fj_upper_bound, info_lower_bound
from consts.const import Color

# Custom log level so test records don't mix with Python's own INFO/WARNING
_RESULT_LEVEL = 25
logging.addLevelName(_RESULT_LEVEL, "RESULT")


# ── Result formatter ──────────────────────────────────────────────────────────

def _fmt_cmp(val: Optional[int], n: int) -> str:
    """
    Colour-code a comparison count relative to the FJ bounds:
      green  = within W(n)           (optimal)
      yellow = within W(n)*1.05 + 1  (minor overshoot)
      red    = clearly exceeds W(n)
    lb (information lower bound) is shown for reference only.
    """
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
    """Converts a LogRecord carrying a TestResult into a coloured string."""

    def format(self, record: logging.LogRecord) -> str:
        r: TestResult = getattr(record, "result")
        mark = f"{Color.GREEN}✓{Color.RESET}" if r.ok else f"{Color.RED}✗{Color.RESET}"

        if r.kind == "error":
            extra = "" if r.ok else f"  {Color.RED}{r.failures[0]}{Color.RESET}"
            return f"  {mark} {r.label:<36}{extra}"

        # kind == "sort"
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


# ── TestLogger ────────────────────────────────────────────────────────────────

class TestLogger:
    """
    Buffers TestResult objects as LogRecords via MemoryHandler.

    Usage:
        log = TestLogger()
        for case in cases:
            log.record(judge(case))          # buffered, nothing printed yet
        passed, failed = log.flush_section("Section title", expected_total=21)
    """

    def __init__(self) -> None:
        self._buf: list[TestResult] = []
        self._stream = io.StringIO()

        target = logging.StreamHandler(self._stream)
        target.setFormatter(_ResultFormatter())

        # flushLevel=CRITICAL ensures the buffer is never auto-flushed
        # mid-section; only our explicit flush_section() call drains it.
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

    # ── public API ────────────────────────────────────────────────────

    def record(self, result: TestResult) -> None:
        """Buffer one TestResult as a LogRecord (no output yet)."""
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
        """
        Drain the MemoryHandler and print the section to stdout.
        Returns (passed, failed).
        """
        passed = sum(1 for r in self._buf if r.ok)
        failed = len(self._buf) - passed
        total  = expected_total if expected_total is not None else len(self._buf)

        # MemoryHandler → StreamHandler → StringIO
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
        """Print the grand total line."""
        bar   = "═" * 80
        color = Color.GREEN if failed == 0 else Color.RED
        print(f"{Color.BOLD}{bar}{Color.RESET}")
        print(f"{color}{Color.BOLD}  TOTAL  {passed} passed  /  {failed} failed{Color.RESET}")
        print(f"{Color.BOLD}{bar}{Color.RESET}\n")
