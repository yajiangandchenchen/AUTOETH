#!/usr/bin/env python3
"""Prune the analysis evidence to summaries plus SARIF, then keep the rounds.

Only these files are retained per round:
  ___summary.md
  analysis_result.sarif

Everything else under the round directories is removed because it is
regenerable. The ruleset lives outside these directories and is untouched.
"""
from pathlib import Path

SA = Path("/home/user1/AUTOETH/AUTOETH/docs/08_实施/static-analysis")
KEEP = {"___summary.md", "analysis_result.sarif"}


def main():
    removed = 0
    for round_dir in sorted(SA.iterdir()):
        if not round_dir.is_dir() or not round_dir.name.startswith("round"):
            continue
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
                entry.rmdir()
                removed += 1
    print("removed entries:", removed)
    for round_dir in sorted(SA.iterdir()):
        if round_dir.is_dir():
            print(round_dir.name, "->", sorted(p.name for p in round_dir.iterdir()))


if __name__ == "__main__":
    main()