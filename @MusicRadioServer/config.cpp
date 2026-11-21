// Серверный мод (версия 1.30+)
// Согласно 17_HOW_IT_WAS_BUILT.md п. 0 и 02_ARCHITECTURE.md
class CfgPatches {
    class MusicRadioServer {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = { "DZ_Data", "DZ_Scripts", "MusicRadioCore" };
    };
};
class CfgMods {
    class MusicRadioServer {
        dir = "MusicRadioServer";
        name = "MusicRadio - Server";
        type = "servermod";
        dependencies[] = { "Game", "World", "Mission" };
        class defs {
            class gameScriptModule  { value=""; files[] = { "MusicRadioServer/Scripts/4_World" }; };
            class missionScriptModule { value=""; files[] = { "MusicRadioServer/Scripts/5_Mission" }; };
        };
    };
};
