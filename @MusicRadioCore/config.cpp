// Конфиг мода @MusicRadioCore (Версия 1.30+)
// Согласно 17_HOW_IT_WAS_BUILT.md п. 2.1-2.2 и 13_CHECK_BUILD.md (ревизия 20261174)

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

// Патч слотов (пример из 13_CHECK_BUILD.md ревизия 20261174)
class CfgSlots {
    class Slot_MR_DashSlot {
        name = "MR_DashSlot";
        selection = "MR_DashSlot";
    };
};

// Классы предметов (пример из 07_MODELS.md и 13_CHECK_BUILD.md)
class MR_Cassette_Rock01 : MR_CassetteCase_Base {
    scope = 2;
    displayName = "Cassette - Demo Rock A";
    model = "MusicRadioCore/data/mr_cassette.p3d";
};

class MR_MiniDisc : ItemBase {
    scope = 2;
    displayName = "MiniDisc";
    model = "MusicRadioCore/data/mr_minidisc.p3d";
};

class MR_MemoryCard : ItemBase {
    scope = 2;
    displayName = "Memory Card";
    model = "MusicRadioCore/data/mr_memorycard.p3d";
};
