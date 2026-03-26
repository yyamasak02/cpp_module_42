"""
Build phase: cd to work_space, run `make re`, verify binary exists.
"""
import os
import subprocess


def build(work_space: str, program_file: str) -> tuple[bool, str]:
    """Returns (success, error_message)."""
    try:
        os.chdir(work_space)
    except OSError as e:
        return False, f"chdir failed: {e}"

    ret = subprocess.run(["make", "-s", "re"], capture_output=True)
    if ret.returncode != 0:
        return False, "make failed:\n" + ret.stderr.decode(errors="replace")

    if not os.path.exists(program_file):
        return False, f"binary not found after build: {program_file}"

    return True, ""
