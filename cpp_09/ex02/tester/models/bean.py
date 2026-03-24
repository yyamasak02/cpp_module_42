from dataclasses import dataclass, field
from typing import Optional


@dataclass
class ParsedOutput:
    after:    Optional[str] = None
    time_vec: Optional[str] = None
    time_deq: Optional[str] = None
    cmp_vec:  Optional[int] = None
    cmp_deq:  Optional[int] = None


@dataclass
class TestResult:
    label:    str
    n:        int
    ok:       bool               = False
    failures: list[str]          = field(default_factory=list)
    parsed:   Optional[ParsedOutput] = None
    kind:     str                = "sort"   # "sort" | "error"
