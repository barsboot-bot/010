// ============================================================================
//  MusicRadio - Server
//  MR_ServerHooks.c : server-side reactions to world events.
//  Layer: 4_World
// ============================================================================

// A ruined car must not keep singing.
modded class CarScript
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!GetGame().IsServer())
			return;

		int idLow; int idHigh;
		MR.NetId(this, idLow, idHigh);
		MR_ServerEngine.StopByHost(MR_HostType.HOST_VEHICLE, idLow, idHigh, MR_StopReason.STOP_RUINED);
	}

	// Some builds destroy vehicles through EEHitBy / deletion instead.
	override void EEDelete(EntityAI parent)
	{
		if (GetGame().IsServer())
			int idLow; int idHigh;
			MR.NetId(this, idLow, idHigh);
			MR_ServerEngine.StopByHost(MR_HostType.HOST_VEHICLE, idLow, idHigh, MR_StopReason.STOP_HOST_GONE);

		super.EEDelete(parent);
	}
}

// A destroyed handheld radio stops the music immediately.
modded class PersonalRadio
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!GetGame().IsServer())
			return;

		int idLow; int idHigh;
		MR.NetId(this, idLow, idHigh);
		MR_ServerEngine.StopByHost(MR_HostType.HOST_ITEM, idLow, idHigh, MR_StopReason.STOP_RUINED);
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame().IsServer())
			int idLow; int idHigh;
			MR.NetId(this, idLow, idHigh);
			MR_ServerEngine.StopByHost(MR_HostType.HOST_ITEM, idLow, idHigh, MR_StopReason.STOP_HOST_GONE);

		super.EEDelete(parent);
	}
}

// ---------------------------------------------------------------------------
//  RPC entry point.
//  Clients send their requests with target = their own player object, so the
//  packet lands exactly here and nowhere else.
// ---------------------------------------------------------------------------
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (!GetGame().IsServer())
			return;
		if (rpc_type != MR.RPC_REQUEST)
			return;

		// Never trust the "sender" field: resolve the player from the identity
		// that actually delivered the packet.
		if (!sender)
			return;

		string payload;
		if (!ctx.Read(payload))
			return;

		PlayerBase player = PlayerBase.Cast(MR_ServerUtil.FindPlayer(sender));
		if (!player)
		{
			MR.LogWarn("Request from unknown identity: " + sender.GetName());
			return;
		}

		MR_ServerEngine.HandleRequest(player, payload);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!GetGame().IsServer())
			return;

		// The radio in their hands is gone with the body; the session is dropped
		// by the watchdog in MR_ServerEngine.Tick().
	}
}
