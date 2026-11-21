#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""check_pbo.py - проверка содержимого PBO"""
import pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parent.parent
def main():
    pbo_dir = ROOT / "release" / "pbo"
    if not pbo_dir.exists():
        print("release/pbo not found")
        return 1
    for p in pbo_dir.glob("*.pbo"):
        print(f"{p.name}: {p.stat().st_size} bytes")
    print("check_pbo.py: OK")
    return 0
if __name__ == "__main__":
    sys.exit(main())
