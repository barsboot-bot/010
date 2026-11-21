#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""autobuild.py - весь конвейер одной командой"""
import subprocess, sys, pathlib
ROOT = pathlib.Path(__file__).resolve().parent.parent

def run(cmd):
    print(f"$ {' '.join(cmd)}")
    r = subprocess.run(cmd, cwd=ROOT)
    return r.returncode

def main():
    args = sys.argv[1:]
    deploy = "--deploy" in args or "1" in args
    no_build = "--no-build" in args

    steps = [
        ["python", "tools/generate_db_config.py"],
        ["python", "tools/check_project.py"],
        ["python", "tools/stage_pbo.py", "--clean"],
    ]
    for s in steps:
        if run(s) != 0:
            print(f"FAILED: {s}")
            return 1
    if not no_build:
        print("NOTE: BUILD_PBO.bat requires DayZ Tools on Windows, run it manually")
    print("autobuild.py: OK")
    return 0

if __name__ == "__main__":
    sys.exit(main())
