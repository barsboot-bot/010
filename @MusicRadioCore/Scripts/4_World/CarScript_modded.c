// Патч машин (Версия 1.30+)
// Согласно 13_CHECK_BUILD.md (ревизия 20261174) и 07_MODELS.md (селекшен MR_DashSlot)
// Модификация ванильного CarScript для поддержки слота MR_DashSlot и музыки
#include "Script_Component.h"
#include "CarScript.h"

class CarScript_modded : CarScript {
    // Слот под кассету/диск/плеер в машине (пример из 13_CHECK_BUILD.md)
    protected:
    ref MR_MediaBay m_MediaBay; // Общая проверка «что вставлено» (карта, кассета, диск)
    
    override void OnAttachmentChanged(EntityAI attachment, int slotIdx, int action) {
        super.OnAttachmentChanged(attachment, slotIdx, action);
        // Проверка слота MR_DashSlot (пример из 13_CHECK_BUILD.md и 07_MODELS.md)
        if (slotIdx == "MR_DashSlot") {
            if (attachment != null) {
                m_MediaBay.InsertCarrier(attachment); // Кассета, MD или SD
            } else {
                m_MediaBay.RemoveCarrier();
            }
        }
    }
    
    override bool CanPlay() {
        // Проверка готовности (пример из 13_CHECK_BUILD.md)
        return m_MediaBay.IsReady() && m_MediaBay.HasBattery();
    }
};
