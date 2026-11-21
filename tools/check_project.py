#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
check_project.py - проверки проекта MusicRadio
Ревизия 20261175
"""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent

def check_brackets(path):
    text = path.read_text(encoding="utf-8", errors="ignore")
    # string-aware balance check: ignore brackets inside "..." and // comments
    stack = []
    pairs = {")": "(", "}": "{", "]": "["}
    opens = set(pairs.values())
    i = 0
    in_str = False
    in_line_comment = False
    while i < len(text):
        ch = text[i]
        nxt = text[i+1] if i+1 < len(text) else ""
        if in_line_comment:
            if ch == "\n":
                in_line_comment = False
            i += 1
            continue
        if in_str:
            if ch == "\\":
                i += 2
                continue
            if ch == '"':
                in_str = False
            i += 1
            continue
        # not in string/comment
        if ch == "/" and nxt == "/":
            in_line_comment = True
            i += 2
            continue
        if ch == '"':
            in_str = True
            i += 1
            continue
        if ch in opens:
            stack.append(ch)
        elif ch in pairs:
            if not stack or stack.pop() != pairs[ch]:
                return False, f"unbalanced at {i}"
        i += 1
    return len(stack) == 0, "ok" if not stack else f"unclosed {stack}"

def main():
    errors = []
    warnings = []

    # Check config.cpp files exist
    for mod in ["MusicRadioCore", "MusicRadioDB", "MusicRadioServer"]:
        cfg = ROOT / f"@{mod}" / "config.cpp"
        if not cfg.exists():
            errors.append(f"missing {cfg}")
        else:
            text = cfg.read_text(encoding="utf-8", errors="ignore")
            if "class CfgPatches" not in text:
                errors.append(f"{mod} config missing CfgPatches")
            if "class CfgMods" not in text:
                errors.append(f"{mod} config missing CfgMods")
            ok, msg = check_brackets(cfg)
            if not ok:
                errors.append(f"{mod} config brackets: {msg}")
            print(f"{mod} config: {len(text.splitlines())} lines")

    # Check scripts layers
    core_scripts = ROOT / "@MusicRadioCore" / "Scripts"
    if core_scripts.exists():
        for layer in ["3_Game", "4_World", "5_Mission"]:
            p = core_scripts / layer
            if p.exists():
                files = list(p.rglob("*.c"))
                print(f"Core {layer}: {len(files)} files")
                for f in files:
                    ok, msg = check_brackets(f)
                    if not ok:
                        errors.append(f"{f}: {msg}")
            else:
                warnings.append(f"missing layer {layer}")

    # Check DB index vs config
    index = ROOT / "@MusicRadioDB" / "Music" / "index.json"
    if index.exists():
        print(f"index.json exists: {index.stat().st_size} bytes")
    else:
        errors.append("missing @MusicRadioDB/Music/index.json")

    # Check for forbidden patterns (from 13_CHECK_BUILD.md)
    # - modded class on own classes
    # - class X : CarScript for vanilla
    for f in (ROOT / "@MusicRadioCore").rglob("*.c"):
        raw = f.read_text(encoding="utf-8", errors="ignore")
        # strip // comments for this check (modded in comments is ok)
        lines = []
        for line in raw.splitlines():
            # naive: cut at // not inside string (good enough for this check)
            if "//" in line:
                # keep code before //
                # but avoid cutting inside string - simple heuristic: if // after quote, keep
                idx = line.find("//")
                # if there's an odd number of quotes before, it's inside string, skip
                if line[:idx].count('"') % 2 == 0:
                    line = line[:idx]
            lines.append(line)
        text = "\n".join(lines)
        # modded class is correct for vanilla classes (EffectSound, PlayerBase, CarScript, PersonalRadio, etc)
        # It is wrong only for own config classes (MR_CassetteCase_Base, MR_MiniDisc_Base, MR_CassettePlayer, etc)
        for m in re.finditer(r"modded\s+class\s+(MR_Cassette\w*|MR_MiniDisc\w*|MR_MemoryCard\w*)", text):
            errors.append(f"{f}: modded class on own class {m.group(1)}")
        if re.search(r"class\s+\w+\s*:\s*CarScript", text):
            errors.append(f"{f}: class : CarScript (should be modded)")

    if errors:
        print("ERRORS:")
        for e in errors:
            print(f"  - {e}")
        return 1
    if warnings:
        print("WARNINGS:")
        for w in warnings:
            print(f"  - {w}")
    print("check_project.py: OK")
    return 0

if __name__ == "__main__":
    sys.exit(main())
