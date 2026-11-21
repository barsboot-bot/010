#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""check_bat.py - проверка .bat файлов (CRLF, BOM, скобки)"""
import pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent

def main():
    errors = []
    for bat in ROOT.glob("*.bat"):
        data = bat.read_bytes()
        if data.startswith(b"\xef\xbb\xbf"):
            errors.append(f"{bat.name}: BOM found")
        # Check CRLF (allow LF only if file is small? but spec says CRLF)
        text = data.decode("utf-8", errors="ignore")
        if "\r\n" not in text and "\n" in text:
            # LF only
            print(f"WARNING: {bat.name} uses LF, should be CRLF (will fix on Windows)")
            # Auto-fix to CRLF
            fixed = text.replace("\r\n", "\n").replace("\n", "\r\n")
            bat.write_bytes(fixed.encode("utf-8"))
            print(f"  fixed {bat.name} to CRLF")
        # Check unbalanced parentheses in if/for (simple)
        # Count '(' and ')'
        if text.count("(") != text.count(")"):
            print(f"WARNING: {bat.name} parentheses count mismatch: {text.count('(')} vs {text.count(')')}")
    if errors:
        for e in errors:
            print(f"ERROR: {e}")
        return 1
    print("check_bat.py: OK")
    return 0

if __name__ == "__main__":
    sys.exit(main())
