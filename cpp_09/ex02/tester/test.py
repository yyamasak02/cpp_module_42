import os
import re
import random
import subprocess
from enum import Enum
from typing import Optional
import logging

PROGRAME_FILE: str = "PmergeMe"
WORK_SPACE: str = "/home/yuhei0221/work/cursus/cpp_module_42/cpp_09/ex02"


class Prefix(str, Enum):
    BEFORE = r"^\s*Before\s*:\s*(?P<numbers>[\d\s]+)\s*$"
    AFTER = r"^\s*After\s*:\s*(?P<numbers>[\d\s]+)\s*$"
    TIME_RESULT = (
        r"^\s*Time\s+to\s+process\s+a\s+range\s+of\s+"
        r"(?P<element_num>\d+)\s+elements\s+with\s+"
        r"(?P<type>[\w:]+)\s*:\s*(?P<time>\d+)\s*us\s*$"
    )


def parse_line(line: str) -> tuple[Optional[str], Optional[dict[str, str]]]:
    for prefix in Prefix:
        if match := re.match(prefix.value, line):
            return prefix.name, match.groupdict()
    return None, None


def run_program(args: list[str]) -> list[str]:
    result = subprocess.run(
        [f"./{PROGRAME_FILE}", *args],
        capture_output=True,
        text=True,
    )
    if result.stderr:
        print(f"[STDERR] {result.stderr.strip()}")
    # print(f"[EXIT CODE] {result.returncode}\n")
    return [line.strip() for line in result.stdout.splitlines() if line.strip()]


def group_output(stdout_lines: list[str]) -> list[dict[str, dict[str, str]]]:
    groups = []
    for i in range(1, len(stdout_lines), 2):
        d = {}
        for j in range(2):
            key, data = parse_line(stdout_lines[i + j])
            if key and data:
                d[key] = data
        groups.append(d)
    return groups


def verify_result(
    groups: list[dict[str, dict[str, str]]], 
    expected: str
) -> None:
    for g in groups:
        after = g.get("AFTER", {}).get("numbers")
        status = g.get("TIME_RESULT", {})
        if after == expected:
            print("O OK")
            print(f"[{status.get('type')}] {status.get('time')} us")
        else:
            print("X NG (invalid sort result)")
    print()


def test_case(arg_str: str) -> None:
    DEBUG_PRINT: bool = True
    if DEBUG_PRINT is True: print(f"[TEST] {arg_str}")
    args = arg_str.split()
    expected = " ".join(map(str, sorted(map(int, args))))
    if DEBUG_PRINT is True: print(f"[EXPT] {expected}")
    stdout_lines = run_program(args)
    groups = group_output(stdout_lines)
    verify_result(groups=groups, expected=expected)


def build_program() -> bool:
    try:
        os.chdir(WORK_SPACE)
    except Exception as e:
        print(f"[ERROR] failed to chdir: {WORK_SPACE} ({e})")
        return False
    if os.system("make -s re") != 0:
        print("[ERROR] failed to execute make")
        return False
    if not os.path.exists(PROGRAME_FILE):
        print(f"[ERROR] file not found: {PROGRAME_FILE}")
        return False
    return True


def main() -> None:
    NUMBER_OF_ATTEMPTS: int = 5
    MIN_NUMBER: int = 1
    MAX_NUMBER: int = 22
    if not build_program():
        return

    # test_case("3 5 9 7 4")
    for _ in range(NUMBER_OF_ATTEMPTS):
        n = random.randint(MIN_NUMBER, MAX_NUMBER)
        nums = random.sample(range(1, MAX_NUMBER), n)
        test_case(" ".join(map(str, nums)))


if __name__ == "__main__":
    main()
