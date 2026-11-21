// Контент-мод (Версия 1.30+)
// Согласно 17_HOW_IT_WAS_BUILT.md п. 0 и 07_MODELS.md
// Содержит .ogg, CfgSoundShaders, CfgSoundSets и классы носителей

class CfgPatches {
    class MusicRadioDB {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = { "DZ_Data", "DZ_Scripts", "MusicRadioCore" };
    };
};

class CfgMods {
    class MusicRadioDB {
        dir = "MusicRadioDB";
        name = "MusicRadio - DB";
        type = "mod";
        dependencies[] = { "Game", "World", "Mission" };
        class defs {
            class gameScriptModule  { value=""; files[] = { "MusicRadioDB/Scripts/4_World" }; };
        };
    };
};

// Классы носителей (пример из 07_MODELS.md и 13_CHECK_BUILD.md)
class MR_Cassette_Rock01 : MR_CassetteCase_Base {
    scope = 2;
    displayName = "Cassette - Demo Rock A";
    model = "MusicRadioCore/data/mr_cassette.p3d";
    // Ссылка на SoundSet (пример из 02_ARCHITECTURE.md п. 3)
    soundShaders[] = { "MR_MUSICRADIODB_WASTELAND_01_SoundShader" };
    soundSets[] = { "MR_MUSICRADIODB_WASTELAND_01_SoundSet" };
};

class MR_MiniDisc : ItemBase {
    scope = 2;
    displayName = "MiniDisc";
    model = "MusicRadioCore/data/mr_minidisc.p3d";
    soundShaders[] = { "MR_MUSICRADIODB_WASTELAND_01_SoundShader" };
    soundSets[] = { "MR_MUSICRADIODB_WASTELAND_01_SoundSet" };
};

class MR_MemoryCard : ItemBase {
    scope = 2;
    displayName = "Memory Card";
    model = "MusicRadioCore/data/mr_memorycard.p3d";
    soundShaders[] = { "MR_MUSICRADIODB_WASTELAND_01_SoundShader" };
    soundSets[] = { "MR_MUSICRADIODB_WASTELAND_01_SoundSet" };
};
