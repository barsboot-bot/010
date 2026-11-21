// Патч рации (версия 1.30+)
// Согласно 13_CHECK_BUILD.md (ревизия 20261174)
class PersonalRadio : Transmitter_Base {
    attachments[] += { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" };
    class GUIInventoryAttachmentsProps {
        class MR_MusicMedia { attachmentSlots[] = { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" }; };
    };
};
class MR_MediaBay {
    // ... проверка «что вставлено» (пример из 13_CHECK_BUILD.md) ...
};
