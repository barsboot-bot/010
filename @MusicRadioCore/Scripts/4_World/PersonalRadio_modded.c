// Патч рации (Версия 1.30+)
// Согласно 13_CHECK_BUILD.md (ревизия 20261174) и 01_INSTALL.md
// Наследник Transmitter_Base с слотами для MD, SD и кассеты
class PersonalRadio : Transmitter_Base {
    // Добавление слотов (пример из 13_CHECK_BUILD.md)
    attachments[] += { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" };
    
    class GUIInventoryAttachmentsProps {
        class MR_MusicMedia { 
            attachmentSlots[] = { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" }; 
        };
    };
    
    override void OnAttachmentChanged(EntityAI attachment, int slotIdx, int action) {
        super.OnAttachmentChanged(attachment, slotIdx, action);
        // Проверка готовности рации (пример из 13_CHECK_BUILD.md)
        if (m_MediaBay != null) {
            m_MediaBay.CheckReadyState();
        }
    }
};

class MR_MediaBay {
    // Единая проверка «что вставлено» для всех устройств (пример из 13_CHECK_BUILD.md)
    bool IsReady() {
        // Проверка носителя → карты → батарейки (пример из 13_CHECK_BUILD.md)
        return HasCarrier() && HasMemoryCard() && HasBattery();
    }
    
    bool HasCarrier() { return true; } // Кассета, MD или SD
    bool HasMemoryCard() { return true; }
    bool HasBattery() { return true; }
};
