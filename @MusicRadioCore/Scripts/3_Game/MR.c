// Логика, UI, воспроизведение (версия 1.30+)
// Согласно 17_HOW_IT_WAS_BUILT.md п. 2 (27 скриптов) и 02_ARCHITECTURE.md (слой 3_Game)
// Скрипт MR — основной класс логики
#include "Script_Component.h"

class MR {
    // Контракт: логика не знает путей к файлам (02_ARCHITECTURE.md п. 3)
    // Сервер отправляет provider + trackId -> клиент собирает SoundSet
    
    // Пример реализации из 17_HOW_IT_WAS_BUILT.md (п. 2)
    static void Init() {
        // Инициализация компонентов: MR_Locator, MR_Json, MR_Track, MR_Library, MR_Net, MR_MediaResolver, MR_DeviceBase, MR_AudioTypes
    }
    
    // ... остальные 26 скриптов из слоя 3_Game (02_ARCHITECTURE.md п. 2) ...
};

// Слой 4_World (пример из 02_ARCHITECTURE.md п. 2)
class MR_Registry { /* ... */ };
class MR_Hosts { /* ... */ };

// Слой 5_Mission (пример из 02_ARCHITECTURE.md п. 2)
class MR_ClientRPC { /* ... */ };
class MR_RadioMenu { /* ... */ };
class MR_GameHooks { /* ... */ };
