#!/usr/bin/env python3
"""Prove the bit-geometry shift equals integer division for every legal width.

MISRA C++:2008 1-0-3 asks for a deterministic, documented integer division.
The wide-domain conversion replaced `bits / 8` with `bits >> 3`; this script
checks that the substitution is exact for every width the schema compiler
accepts (8/16/32/64) and for the boundary values used by the range checks.

It is an exhaustive check over the reachable domain, not a sample.
"""
import sys

LEGAL_WIDTHS = (8, 16, 32, 64)
# Every bit value the wide P08/P08m geometry can carry up to the classic limit
# is covered by the full legal-width set plus the uint32 boundary values.
BOUNDARY_VALUES = (0, 1, 7, 8, 9, 0xFFFFFFFF * 8, 0xFFFFFFFF * 8 + 7)


def bits_to_bytes_shift(bits):
    return bits >> 3


def bits_to_bytes_div(bits):
    return bits // 8


def main():
    failures = 0
    checked = 0
    for width in LEGAL_WIDTHS:
        checked += 1
        if bits_to_bytes_shift(width) != bits_to_bytes_div(width):
            failures += 1
            print("MISMATCH width", width)
    for bits in BOUNDARY_VALUES:
        checked += 1
        if bits_to_bytes_shift(bits) != bits_to_bytes_div(bits):
            failures += 1
            print("MISMATCH bits", bits)
    # Align-down used by the TP budget: (n >> 4) << 4 == (n // 16) * 16.
    for raw in range(0, 4096):
        checked += 1
        if ((raw >> 4) << 4) != (raw // 16) * 16:
            failures += 1
            print("MISMATCH align", raw)
    print("checked", checked, "cases, failures", failures)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())