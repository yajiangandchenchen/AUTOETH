#!/usr/bin/env python3
"""Land the round-2 evidence and prune both rounds to summary + SARIF.

Round 1 and round 2 artifacts are produced by the scanner under
.static-analysis/. This script:

  1. copies the current (round-2 static) summary and SARIF into
     docs/08_实施/static-analysis/round2-static/,
  2. prunes every round directory down to ___summary.md and analysis_result.sarif,
  3. prints what remains so the result is unambiguous.

The ruleset lives at .static-analysis/ruleset_c_cpp.json and is never touched.
"""
import shutil
from pathlib import Path

ROOT = Path("/home/user1/AUTOETH/AUTOETH")
SRC = ROOT / ".static-analysis"
SA = ROOT / "docs" / "08_实施" / "static-analysis"
KEEP = {"___summary.md", "analysis_result.sarif"}

ARTIFACTS = ("___summary.md", "analysis_result.sarif")


def copy_round(name):
    dst = SA / name
    dst.mkdir(parents=True, exist_ok=True)
    for artifact in ARTIFACTS:
        source = SRC / artifact
        if source.is_file():
            shutil.copy2(source, dst / artifact)
            print("copied", artifact, "->", name)
        else:
            print("MISSING", artifact)


def prune_round(round_dir):
    removed = 0
    for entry in sorted(round_dir.iterdir(), key=lambda p: len(p.parts), reverse=True):
        if entry.is_file():
            if entry.name in KEEP:
                continue
            entry.unlink()
            removed += 1
        elif entry.is_dir():
            for child in sorted(entry.rglob("*"), key=lambda p: len(p.parts), reverse=True):
                if child.is_file():
                    child.unlink()
                    removed += 1
                elif child.is_dir():
                    child.rmdir()
            entry.rmdir()
            removed += 1
    return removed


def main():
    copy_round("round2-static")
    for round_dir in sorted(SA.iterdir()):
        if round_dir.is_dir() and round_dir.name.startswith("round"):
            removed = prune_round(round_dir)
            print(round_dir.name, "pruned", removed, "entries ->",
                  sorted(p.name for p in round_dir.iterdir()))


if __name__ == "__main__":
    main()