// Серверный мод (Версия 1.30+)
// Согласно 17_HOW_IT_WAS_BUILT.md п. 0 и 02_ARCHITECTURE.md (слой MR_ServerEngine)
// Права, лимиты, аудит (без подписи, лежит в ServerMOD/Addons)

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

// Скрипты сервера (пример из 17_HOW_IT_WAS_BUILT.md п. 0)
class MR_ServerEngine {
    // Обработка запросов (пример из 02_ARCHITECTURE.md п. 4)
    static void HandleRequest(string action, int type, int idLow, int idHigh, int trackId, int vol, int radius, int broadcast) {
        // Проверка флагов, кулдауна, существования трека, объекта-хоста, правил хоста
        // Диспечтинг RPC_PLAY, RPC_REPLY, RPC_NOWPLAYING
    }
};

class MR_RPC {
    // Строки RPC (пример из 02_ARCHITECTURE.md п. 2)
};

class MR_Limits {
    // Лимиты (пример из 10_LIMITS.md и 13_CHECK_BUILD.md)
};
