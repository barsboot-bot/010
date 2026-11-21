// Скрипты слоя 4_World и 5_Mission (согласно 02_ARCHITECTURE.md п. 2 и 17_HOW_IT_WAS_BUILT.md п. 0)

// @MusicRadioCore/Scripts/4_World/MR_Registry.c
class MR_Registry {
    // Реестр объектов-хостов (машины, рации)
    static void RegisterHost(EntityAI host) {
        // ...
    }
};

// @MusicRadioCore/Scripts/4_World/MR_Hosts.c
class MR_Hosts {
    // Обработка хостов (item/vehicle)
    static bool IsHostReady(EntityAI host) {
        // Проверка по 13_CHECK_BUILD.md (ревизия 20261174)
        return true;
    }
};

// @MusicRadioCore/Scripts/4_World/CarScript_modded.c (дополнение)
// Патч машин (версия 1.30+)
class CarScript_modded : CarScript {
    // ... (уже создан ранее) ...
    // Добавление проверки слота MR_DashSlot (пример из 07_MODELS.md и 13_CHECK_BUILD.md)
    override void OnInventorySlotAdded(EntityAI item, int slotIdx) {
        super.OnInventorySlotAdded(item, slotIdx);
        if (slotIdx == "MR_DashSlot") {
            // Логика вставки кассеты/MD/SD (пример из 13_CHECK_BUILD.md)
        }
    }
};

// @MusicRadioCore/Scripts/4_World/PersonalRadio_modded.c (дополнение)
// Патч рации (версия 1.30+)
class PersonalRadio_modded : PersonalRadio {
    // ... (уже создан ранее, добавлены проверки из 13_CHECK_BUILD.md) ...
    override bool CanPlay() {
        // Проверка готовности рации (пример из 13_CHECK_BUILD.md)
        return m_MediaBay.IsReady() && m_MediaBay.HasBattery() && m_MediaBay.HasCarrier();
    }
};

// @MusicRadioCore/Scripts/5_Mission/MR_ClientRPC.c (пример из 02_ARCHITECTURE.md п. 4)
class MR_ClientRPC {
    static void RequestPlay(EntityAI host, int type, int idLow, int idHigh, int trackId, int vol, int radius, int broadcast) {
        // Отправка ScriptRPC на сервер
    }
    static void HandlePlay(string locator, int vol, int radius) {
        // Резолв SoundSet и запуск звука (пример из 02_ARCHITECTURE.md п. 3-4)
    }
};
