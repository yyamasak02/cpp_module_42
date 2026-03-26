"""
Program runner for ex01: Reverse Polish Notation.

run_rpn(args) -> (stdout, stderr, exit_code)
"""

import os
import subprocess

WORK_SPACE:   str = "/home/yuhei0221/work/cursus/cpp_module_42/cpp_09/ex01"
PROGRAM_FILE: str = "RPN"


def run_rpn(args: list[str]) -> tuple[str, str, int]:
    """
    Run ./RPN with the given argument list.
    Returns (stdout, stderr, exit_code).
    """
    os.chdir(WORK_SPACE)
    cmd = [f"./{PROGRAM_FILE}", *args]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.stdout.strip(), result.stderr.strip(), result.returncode
