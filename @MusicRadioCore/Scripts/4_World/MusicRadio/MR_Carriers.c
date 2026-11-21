// ============================================================================
//  MusicRadio - Core
//  MR_Carriers.c : what you put INTO a deck. One file, one place per class.
//  Layer: 4_World
//
//  These classes are declared in config.cpp (CfgVehicles), so the script side
//  uses a plain "class" with the same name. "modded class" only injects into
//  classes the engine declares (ItemBase, CarScript, PersonalRadio); using it
//  on our own class gives "Unknown type 'MR_CassetteCase_Base'" at compile time.
//
//  Contents:
//    MR_CassetteCase_Base  - album identity + the position it stopped at
//    MR_MiniDisc_Base      - same contract, different class
//    MR_MemoryCard_Base    - a key, not a container: never holds audio
//
//  Persistence format lives in MR_CarrierState (3_Game). This file only knows
//  when to read and write it: OnStoreSave always writes the slot (even when
//  empty), because OnStoreLoad matches fields by position.
// ============================================================================

// ============================================================================
//  MR_CarrierBase : common parent for cassettes / mini discs / memory cards.
// ----------------------------------------------------------------------------
//  WHY THIS CLASS EXISTS (log 19:18, stage World):
//      mr_carriers.c,24: Function 'EEInit' is marked as override, but there is
//                        no function with this name in the base class
//
//  Our classes come from config.cpp, and config.cpp has nothing to do with the
//  SCRIPT inheritance tree: a config-level parent (Inventory_Base) does NOT give
//  the script class a base, so "override" had nothing to override and "super."
//  had nothing to call. The class must extend a real script class.
//
//  ItemBase is the right one: in vanilla scripts
//      typedef ItemBase Inventory_Base;        (itembase.c)
//      class ItemBase extends InventoryItem
//  so the script base matches the config base, and EEInit / EEAtive /
//  OnStoreSave / OnStoreLoad come from it for real.
//
//  Everything common lives here once; the three subclasses below only add
//  what makes them different (album label, hint, memory card is not a tape).
// ============================================================================
class MR_CarrierBase extends ItemBase
{
	protected string m_MR_TapeLine;                          // encoded memory
	protected ref MR_CarrierMemory m_MRTape;                 // decoded, cached
	protected ref TStringArray m_MRCfgSoundSets;             // CfgCassette cache

	override void EEInit()
	{
		super.EEInit();
		MR.Log("Carrier in world: " + GetType());
	}

	// The position is written by the SERVER (the deck reports it), read by
	// whoever inserts the tape next. Protected, not replicated: a net sync of a
	// changing int every second is exactly the traffic we are avoiding - the
	// position travels with the object, not with the network.
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		// Always write the slot, even when empty: OnStoreLoad matches fields by
		// position, so skipping the line would shift every field that follows
		// (that is how a mod silently corrupts another mod's saved data).
		ctx.Write(m_MR_TapeLine);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		string line = string.Empty;
		if (ctx.Read(line))
		{
			m_MR_TapeLine = line;
			m_MRTape = MR_CarrierState.Decode(line);
			if (m_MRTape)
				MR.Log("Tape resumes at: " + m_MRTape.m_TrackId + " +" + m_MRTape.m_Position.ToString() + "s");
		}
		return true;
	}

	// ------------------------------------------------------------------ memory
	// What this carrier remembers. Never null: callers get an empty object.
	MR_CarrierMemory MR_TapeMemory()
	{
		if (!m_MRTape)
			m_MRTape = MR_CarrierState.Decode(m_MR_TapeLine);
		if (!m_MRTape)
			m_MRTape = new MR_CarrierMemory();
		return m_MRTape;
	}

	bool MR_TapeHasMemory()
	{
		return (!MR_TapeMemory().IsEmpty());
	}

	string MR_TapeTrackId()
	{
		return MR_TapeMemory().m_TrackId;
	}

	int MR_TapePosition()
	{
		return MR_TapeMemory().m_Position;
	}

	int MR_TapeTrackNo()
	{
		return MR_TapeMemory().m_TrackNo;
	}

	// Only the server writes: the deck sends the position, the client never
	// invents one (two writers = the tape and the HUD disagree after a restart).
	void MR_TapeSetPosition(string trackId, int position, int trackNo)
	{
		if (!GetGame().IsServer())
			return;
		if (trackId == string.Empty)
			return;

		MR_CarrierMemory mem = new MR_CarrierMemory();
		mem.m_TrackId = trackId;
		mem.m_Position = MR_CarrierState.ClampPosition(position, -1);
		mem.m_TrackNo = trackNo;
		m_MRTape = mem;
		m_MR_TapeLine = MR_CarrierState.Encode(mem);
	}

	void MR_TapeForget()
	{
		if (!GetGame().IsServer())
			return;
		m_MRTape = new MR_CarrierMemory();
		m_MR_TapeLine = string.Empty;
	}

	// ---------------------------------------------- CfgCassette (VOS_Radio)
	// A carrier may declare its own playlist, so a third-party mod can ship a
	// tape without knowing our media pack index. Cached: ConfigIsExisting per
	// tick is a string comparison per frame, for no reason.
	void MR_CarrierSoundSets(out TStringArray sets)
	{
		if (!m_MRCfgSoundSets)
			MR_CarrierConfig.SoundSetsOf(GetType(), m_MRCfgSoundSets);
		sets = m_MRCfgSoundSets;
	}

	bool MR_CarrierIsPlaylist()
	{
		return MR_CarrierConfig.IsPlaylist(GetType());
	}

	// Album id this tape carries (lower-case, matches index.json).
	string MR_AlbumId()
	{
		return MR_MediaItems.AlbumOf(GetType());
	}

	// Cheap per-frame answer for the HUD: resolved once, reused.
	string MR_AlbumIdCached()
	{
		return MR_MediaItems.AlbumOf(GetType());
	}

	// A carrier is useless without a player; the HUD greys it out with this.
	string MR_CarrierHint()
	{
		return "Carrier '" + MR_AlbumId() + "'";
	}
};

class MR_CassetteCase_Base extends MR_CarrierBase
{
	// A cassette is useless without a player; the HUD greys it out with this.
	override string MR_CarrierHint()
	{
		return "Cassette '" + MR_AlbumId() + "' - put it in a cassette player";
	}
}

class MR_MiniDisc_Base extends MR_CarrierBase
{
	override string MR_CarrierHint()
	{
		return "MiniDisc '" + MR_AlbumId() + "' - put it in an MD player";
	}
}

class MR_MemoryCard_Base extends MR_CarrierBase
{
	// The card is a key, not a container: it never holds audio. Keeping the
	// method makes the intent explicit for anyone reading the code later.
	override string MR_CarrierHint()
	{
		return "Memory card - a key for the deck, it holds no music";
	}

	bool MR_HoldsMusic()
	{
		return false;
	}
}
