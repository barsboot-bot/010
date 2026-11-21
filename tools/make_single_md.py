#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
make_single_md.py - собирает 18_FULL_SOURCE.md из реальных файлов
Ревизия 20261175
"""
import pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "18_FULL_SOURCE.md"

FILES = [
    "@MusicRadioCore/config.cpp",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Constants.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Net.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Json.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Track.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Locator.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_MediaResolver.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Library.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Config.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_AudioTypes.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_DeviceBase.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_ClientAudio.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_CarrierState.c",
    "@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_SoundMix.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Carriers.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Decks.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_DeckResume.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_MediaItems.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Hosts.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Registry.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_WorldHooks.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_PlayerRPC.c",
    "@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Animations.c",
    "@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_ClientRPC.c",
    "@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_ClientState.c",
    "@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_LibraryIndex.c",
    "@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_RadioMenu.c",
    "@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_GameHooks.c",
    "@MusicRadioCore/gui/layouts/radio_ui.layout",
    "@MusicRadioCore/gui/layouts/radio_row.layout",
    "@MusicRadioCore/inputs.xml",
    "@MusicRadioCore/stringtable.csv",
    "@MusicRadioServer/config.cpp",
    "@MusicRadioServer/Scripts/4_World/MusicRadio/MR_ServerEngine.c",
    "@MusicRadioServer/Scripts/4_World/MusicRadio/MR_ServerHooks.c",
    "@MusicRadioServer/Scripts/5_Mission/MusicRadio/MR_ServerMission.c",
    "@MusicRadioDB/config.cpp",
    "@MusicRadioDB/Music/index.json",
    "@MusicRadioDB/Music/carriers.json",
    "@MusicRadioDB/Music/DatabaseConfig.json",
    "@MusicRadioDB/Music/Rock/demo_playlist.json",
    "@MusicRadioDB/CREDITS.txt",
    "@MusicRadioDB/Scripts/4_World/MR_DB_Placeholder.c",
    "server_files/profiles/MusicRadio/AudioConfig.json",
    "server_files/mpmissions/types_musicradio.xml",
    "server_files/mpmissions/cfgspawnabletypes.xml",
    "Models/config_proxy_snippet.cpp",
]

def lang_for(path):
    ext = pathlib.Path(path).suffix.lower()
    return {".c": "cpp", ".cpp": "cpp", ".json": "json", ".xml": "xml", ".layout": "xml", ".txt": "text", ".csv": "text"}.get(ext, "text")

def main():
    if "--check" in sys.argv:
        if not OUTPUT.exists():
            print("18_FULL_SOURCE.md not found")
            return 1
        print(f"18_FULL_SOURCE.md exists: {OUTPUT.stat().st_size} bytes")
        return 0

    out = []
    out.append("# MusicRadio - ПОЛНЫЙ ИСХОДНЫЙ КОД В ОДНОМ ФАЙЛЕ\n")
    out.append("Ревизия **20261175**. Файл собран автоматически: `python tools/make_single_md.py`.\n")
    out.append("---\n")
    for rel in FILES:
        p = ROOT / rel
        if not p.exists():
            print(f"WARNING: missing {rel}")
            continue
        text = p.read_text(encoding="utf-8", errors="ignore")
        lines = text.count("\n") + 1
        size = p.stat().st_size
        out.append(f"### {rel}\n")
        out.append(f"`{rel}` - {lines} строк, {size} байт.\n")
        out.append(f"```{lang_for(rel)}\n{text.rstrip()}\n```\n")
    OUTPUT.write_text("\n".join(out), encoding="utf-8")
    print(f"Wrote {OUTPUT} ({OUTPUT.stat().st_size} bytes)")
    return 0

if __name__ == "__main__":
    sys.exit(main())
