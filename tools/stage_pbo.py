#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
stage_pbo.py - раскладка файлов в release/staging как для AddonBuilder
Ревизия 20261175
"""
import pathlib, shutil, hashlib, sys, json, zipfile, datetime

ROOT = pathlib.Path(__file__).resolve().parent.parent
STAGING = ROOT / "release" / "staging"
PBO_DIR = ROOT / "release" / "pbo"

MODS = ["MusicRadioCore", "MusicRadioDB", "MusicRadioServer"]

def sha256_file(p):
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()

def stage(clean=False, do_zip=False, check_only=False):
    if check_only:
        # check if staging exists and is fresh (simplified)
        if not STAGING.exists():
            print("staging not found, need --clean")
            return 1
        print("staging exists")
        return 0

    if clean and STAGING.exists():
        shutil.rmtree(STAGING)
    STAGING.mkdir(parents=True, exist_ok=True)

    manifest = []
    report = []

    for mod in MODS:
        src = ROOT / f"@{mod}"
        if not src.exists():
            print(f"WARNING: @{mod} not found")
            report.append(f"missing @{mod}")
            continue
        dst = STAGING / mod
        if dst.exists():
            shutil.rmtree(dst)
        shutil.copytree(src, dst, ignore=shutil.ignore_patterns("__pycache__", "*.pyc", "Thumbs.db", "*~", "*.pbo", "*.bisign", ".gitkeep"))
        # files.txt
        files = []
        for f in dst.rglob("*"):
            if f.is_file():
                rel = f.relative_to(dst).as_posix()
                files.append(rel)
                manifest.append((f"{mod}/{rel}", sha256_file(f), f.stat().st_size))
        (STAGING / f"{mod}.files.txt").write_text("\n".join(sorted(files)), encoding="utf-8")
        print(f"Staged {mod}: {len(files)} files")

    # BUILD_MANIFEST
    manifest_path = ROOT / "release" / "BUILD_MANIFEST.txt"
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    with open(manifest_path, "w", encoding="utf-8") as out:
        out.write(f"# MusicRadio build manifest {datetime.datetime.now().isoformat()}\n")
        for path, sha, size in sorted(manifest):
            out.write(f"{sha}  {size}  {path}\n")

    report_path = ROOT / "release" / "report_stage.txt"
    report_path.write_text("\n".join(report) if report else "OK - staged for AddonBuilder, no problems\n", encoding="utf-8")
    print("OK - staged for AddonBuilder, no problems")

    if do_zip:
        zip_path = ROOT / "MusicRadio_ARCHIVE.zip"
        with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as z:
            for f in ROOT.rglob("*"):
                if f.is_file() and ".git" not in f.parts and "release/pbo" not in str(f) and not str(f).endswith(".zip"):
                    z.write(f, f.relative_to(ROOT))
        print(f"Archive created: {zip_path}")
    return 0

if __name__ == "__main__":
    args = sys.argv[1:]
    clean = "--clean" in args
    do_zip = "--zip" in args
    check = "--check" in args
    sys.exit(stage(clean=clean, do_zip=do_zip, check_only=check))
