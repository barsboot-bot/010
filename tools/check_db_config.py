#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""check_db_config.py - проверка @MusicRadioDB/config.cpp"""
import pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parent.parent
def main():
    cfg = ROOT / "@MusicRadioDB" / "config.cpp"
    if not cfg.exists():
        print("ERROR: missing @MusicRadioDB/config.cpp")
        return 1
    text = cfg.read_text(encoding="utf-8", errors="ignore")
    if text.count("class CfgSoundSets") != 1:
        print(f"ERROR: CfgSoundSets count = {text.count('class CfgSoundSets')}, expected 1")
        return 1
    if text.count("class CfgSoundShaders") != 1:
        print(f"ERROR: CfgSoundShaders count = {text.count('class CfgSoundShaders')}")
        return 1
    print("check_db_config.py: OK")
    return 0
if __name__ == "__main__":
    sys.exit(main())
