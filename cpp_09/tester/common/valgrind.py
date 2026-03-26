"""
Valgrind memory-leak checker.

check_valgrind(cmd, timeout) -> (ok, message)

  Uses --error-exitcode=42 so valgrind errors are distinguishable
  from the program's own non-zero exits.
  --track-origins=yes shows the origin of uninitialised values.
"""
import subprocess


def check_valgrind(cmd: list[str], timeout: int = 30) -> tuple[bool, str]:
    """
    Run `cmd` under valgrind.
    Returns (True, "") on clean run, (False, detail) on leak / error.
    """
    valgrind_cmd = [
        "valgrind",
        "--leak-check=full",
        "--track-origins=yes",
        "--error-exitcode=42",
        "--quiet",
        *cmd,
    ]
    try:
        result = subprocess.run(
            valgrind_cmd,
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        if result.returncode == 42:
            detail = result.stderr.strip()
            return False, f"valgrind detected error(s):\n{detail[:800] if detail else '(no detail)'}"
        return True, ""
    except FileNotFoundError:
        return False, "valgrind not found — install with: sudo apt install valgrind"
    except subprocess.TimeoutExpired:
        return False, f"valgrind timed out after {timeout}s"
