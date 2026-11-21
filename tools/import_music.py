#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""import_music.py - импорт музыки (ffmpeg -> .ogg) - заглушка"""
import pathlib, sys, argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", required=True)
    parser.add_argument("--album", default="Unknown")
    parser.add_argument("--genre", default="Unknown")
    parser.add_argument("--stage", action="store_true")
    args = parser.parse_args()
    src = pathlib.Path(args.source)
    if not src.exists():
        print(f"ERROR: source not found: {src}")
        return 1
    print(f"Import music: {src} -> album={args.album} genre={args.genre}")
    print("NOTE: ffmpeg conversion not implemented in this stub, place .ogg manually in @MusicRadioDB/Music/<Genre>/")
    return 0

if __name__ == "__main__":
    sys.exit(main())
