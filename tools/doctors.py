#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""doctors.py - сводная диагностика"""
import subprocess, sys
def main():
    for script in ["tools/check_project.py", "tools/check_bat.py", "tools/check_fresh.py", "tools/check_db_config.py"]:
        print(f"=== {script} ===")
        subprocess.run(["python", script])
    return 0
if __name__ == "__main__":
    sys.exit(main())
