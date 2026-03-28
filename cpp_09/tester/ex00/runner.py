"""
Program runner for ex00: Bitcoin Exchange.

run_btc(input_content) -> (combined_output, exit_code)

  - Writes input_content to a temp file (if not None)
  - cd's to WORK_SPACE so that ./const/data.csv is found
  - Runs ./btc [tmpfile] and collects stdout + stderr
  - Cleans up the temp file
"""

import os
import subprocess
import tempfile
from typing import Optional

WORK_SPACE:    str = "/home/yyamasak/Desktop/cursus/cpp_modules/private/cpp_09/ex00"
PROGRAM_FILE:  str = "btc"
_NONEXISTENT_SENTINEL = "__NONEXISTENT_FILE__"


def run_btc(input_content: Optional[str]) -> tuple[str, int]:
    """
    Returns (combined_stdout_stderr, exit_code).

    input_content:
      None                  → run ./btc with no argument
      _NONEXISTENT_SENTINEL → run ./btc with a path that does not exist
      str                   → write to tmp file, run ./btc <tmpfile>
    """
    os.chdir(WORK_SPACE)

    if input_content is None:
        # No argument at all
        cmd = [f"./{PROGRAM_FILE}"]
        result = subprocess.run(cmd, capture_output=True, text=True)
        combined = result.stdout + result.stderr
        return combined, result.returncode

    if input_content == _NONEXISTENT_SENTINEL:
        cmd = [f"./{PROGRAM_FILE}", "/tmp/__btc_no_such_file_12345__"]
        result = subprocess.run(cmd, capture_output=True, text=True)
        combined = result.stdout + result.stderr
        return combined, result.returncode

    # Write input to a temp file
    tmp = tempfile.NamedTemporaryFile(
        mode="w",
        suffix=".txt",
        delete=False,
        dir=WORK_SPACE,
    )
    try:
        tmp.write(input_content)
        tmp.flush()
        tmp.close()
        cmd = [f"./{PROGRAM_FILE}", tmp.name]
        result = subprocess.run(cmd, capture_output=True, text=True)
        combined = result.stdout + result.stderr
        return combined, result.returncode
    finally:
        try:
            os.unlink(tmp.name)
        except OSError:
            pass


def run_btc_with_path(file_path: str) -> tuple[str, int]:
    """Run ./btc with an explicit file path (for valgrind tests)."""
    os.chdir(WORK_SPACE)
    cmd = [f"./{PROGRAM_FILE}", file_path]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.stdout + result.stderr, result.returncode
