// Конфиг мода (версия 1.30+)
// Согласно 17_HOW_IT_WAS_BUILT.md п. 2
class CfgPatches {
    class MusicRadioCore {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = { "DZ_Data", "DZ_Scripts" };
    };
};
class CfgMods {
    class MusicRadioCore {
        dir = "MusicRadioCore";
        name = "MusicRadio - Core";
        type = "mod";
        dependencies[] = { "Game", "World", "Mission" };
        class defs {
            class gameScriptModule  { value=""; files[] = { "MusicRadioCore/Scripts/3_Game" }; };
            class worldScriptModule { value=""; files[] = { "MusicRadioCore/Scripts/4_World" }; };
            class missionScriptModule { value=""; files[] = { "MusicRadioCore/Scripts/5_Mission" }; };
        };
    };
};
