#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""check_log.py - разбор логов DayZ"""
import pathlib, sys, glob
def main():
    logs = glob.glob("DayZServer_x64_*.RPT") + glob.glob("*.log")
    if not logs:
        print("No logs found")
        return 0
    for log in logs:
        print(f"--- {log} ---")
        text = pathlib.Path(log).read_text(encoding="utf-8", errors="ignore")
        for line in text.splitlines():
            if "MusicRadio" in line:
                print(line)
    return 0
if __name__ == "__main__":
    sys.exit(main())
