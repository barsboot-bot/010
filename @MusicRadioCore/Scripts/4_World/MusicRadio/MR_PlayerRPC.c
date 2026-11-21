// ============================================================================
//  MusicRadio - Core
//  MR_PlayerRPC.c : входящие RPC на клиенте.
//  Layer: 4_World
//
//  ПОЧЕМУ ЗДЕСЬ, А НЕ В MISSION
//  -------------------------------------------------------------------------
//  Серверу нужно доставить клиенту конфиг, каталог, PLAY/STOP/VOLUME и
//  состояние. Раньше это делал "modded class DayZGame { override OnRPC }" -
//  и не собралось ни разу:
//
//      Unknown type 'DayZGame'                          (лог 01:12)
//      Function 'OnRPC' is marked as override, but
//      there is no function with this name              (лог 01:23)
//
//  У Mission OnRPC нет вообще, а класс объекта игры в наших слоях недоступен.
//  Зато есть рабочий образец, который уже проверен на этом же сервере: наши
//  ЗАПРОСЫ идут с target = объект игрока и приходят в PlayerBase.OnRPC
//  (MR_ServerHooks.c). Ровно тот же адрес используем и в обратную сторону:
//  сервер шлёт пакет объекту игрока, клиент принимает его здесь.
//
//  Проверка идёт первой строкой: PlayerBase патчат многие моды, и наш
//  обработчик не должен даже читать чужой пакет.
// ============================================================================

// Мост 4_World -> 5_Mission: делегат ставится один раз из MR_ClientRPC.
class MR_MissionBridge
{
	static ref ScriptCaller ClientRpc;

	static void HandleClientRpc(int rpcType, ParamsReadContext ctx)
	{
		if (ClientRpc)
			ClientRpc.Invoke(rpcType, ctx);
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		// Диапазон наших id проверяем ДО IsClient: чужой пакет не трогаем.
		if (rpc_type < MR.RPC_CONFIG || rpc_type > MR.RPC_STATE)
			return;
		if (!GetGame().IsClient())
			return;

		// Client-side only: серверный обработчик живёт в @MusicRadioServer и
		// читает только RPC_REQUEST, поэтому ветки не пересекаются.
		//
		// Сам разбор живёт в 5_Mission (MR_ClientRPC), а этот файл - 4_World:
		// звать верхний слой нельзя (проверка слоёв это запрещает, движок тоже
		// грузит слои по порядку). Поэтому здесь только точка входа, а тело
		// подставляется из 5_Mission (MR_ClientRPC.Install).
		MR_MissionBridge.HandleClientRpc(rpc_type, ctx);
	}
}
