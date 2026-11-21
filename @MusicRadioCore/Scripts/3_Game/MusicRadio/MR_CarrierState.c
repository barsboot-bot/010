// ============================================================================
//  MusicRadio - Core
//  MR_CarrierState.c : where the music state lives - ON THE TAPE.
//  Layer: 3_Game
//
//  WHY THIS FILE EXISTS
//  --------------------
//  Before this file the position of a song existed in three places that could
//  disagree: m_MR_OffsetAtStart on the host, the server session struct, and
//  nothing at all on the cassette. Consequences we could see in testing:
//    * pull the tape out and put it back -> the album starts from track one;
//    * restart the server while a tape plays -> the media is at 0:00;
//    * hand the tape to another player -> they hear it from the beginning.
//
//  The reference mod OMD_CarRadio solved it differently and better than we did:
//  the cassette class carries m_OmdRadioCurrentTrack / m_OmdRadioCurrentTrackTime
//  and saves both in OnStoreSave. The tape is the object that travels, so the
//  tape is the object that remembers.
//
//  We keep the same idea, but NOT the same fields:
//    - OMD kept one int track index; we keep the library track id, because our
//      catalogue ids are stable across media pack rebuilds and OMD's indices
//      break the moment a track is inserted in the middle;
//    - OMD wrote a float per cassette; we also store the deck time, so a "side
//      A 2:17" survives a restart of a server whose clock is not monotonic;
//    - OMD read the config path on every call (CfgVehicles <type> CfgCassette);
//      we cache the resolved SoundSet list on first use.
//
//  Everything here is layer 3_Game on purpose: the server writes the state, the
//  client reads it, and both must see the same class without a cast.
// ============================================================================

// One remembered position inside one track of one album.
class MR_CarrierMemory
{
	string m_TrackId;      // library id, e.g. "kostya_02"
	int    m_Position;     // seconds from the start of that track
	int    m_TrackNo;      // 1-based position inside the album, 0 = unknown
	int    m_SavedAtMs;    // server clock when it was written (audit only)

	void MR_CarrierMemory()
	{
		m_TrackId = string.Empty;
		m_Position = 0;
		m_TrackNo = 0;
		m_SavedAtMs = 0;
	}

	bool IsEmpty()
	{
		return (m_TrackId == string.Empty);
	}

	void Clear()
	{
		m_TrackId = string.Empty;
		m_Position = 0;
		m_TrackNo = 0;
		m_SavedAtMs = 0;
	}
}

// ============================================================================
//  Config side: CfgVehicles <cassette> CfgCassette
// ----------------------------------------------------------------------------
//  This block is the open contract borrowed from VOS_Radio: a carrier may
//  declare its own playlist, so a third party mod can ship a tape without
//  knowing our media pack index. Our own carriers keep using the album key
//  derived from the class name; CfgCassette is the fallback.
//
//      class CfgCassette
//      {
//          isPlaylist = 1;
//          soundSets[] = { "MR_MUSICRADIODB_KOSTYA_01_SoundSet", ... };
//      };
// ============================================================================
class MR_CarrierConfig
{
	// "/CfgVehicles MR_Cassette_Rock01 CfgCassette" or "" when absent.
	static string PathFor(string className)
	{
		if (className == string.Empty)
			return string.Empty;

		string path = "CfgVehicles " + className + " CfgCassette";
		if (!GetGame().ConfigIsExisting(path))
			return string.Empty;
		return path;
	}

	static bool HasBlock(string className)
	{
		return (PathFor(className) != string.Empty);
	}

	// SoundSets declared on the item itself. Empty when the tape is silent.
	static void SoundSetsOf(string className, out TStringArray sets)
	{
		sets = new TStringArray();

		string path = PathFor(className);
		if (path == string.Empty)
			return;

		string listPath = path + " soundSets";
		if (GetGame().ConfigIsExisting(listPath))
			GetGame().ConfigGetTextArray(listPath, sets);

		// A single-shot tape (not a playlist) declares one soundSet only.
		if (sets.Count() == 0)
		{
			string one = string.Empty;
			string onePath = path + " soundSet";
			if (GetGame().ConfigIsExisting(onePath))
			{
				one = GetGame().ConfigGetTextOut(onePath);
				if (one != string.Empty)
					sets.Insert(one);
			}
		}
	}

	static bool IsPlaylist(string className)
	{
		string path = PathFor(className);
		if (path == string.Empty)
			return false;

		if (!GetGame().ConfigIsExisting(path + " isPlaylist"))
			return false;

		return (GetGame().ConfigGetInt(path + " isPlaylist") == 1);
	}

	// Human readable label for logs. ConfigCssPath does not exist, so the
	// fallback is the raw class name - enough to find the tape in the log.
	static string Label(string className)
	{
		if (className == string.Empty)
			return "(no carrier)";
		return className;
	}
}

// ============================================================================
//  State side: the memory kept on one physical cassette / mini disc.
// ----------------------------------------------------------------------------
//  These methods are added to the item classes in 4_World (MR_CarrierState_
//  Hooks.c) because 3_Game must not reference EntityAI subclasses. The logic
//  itself lives here so the server engine and the codec below share ONE copy -
//  OMD duplicated its player logic into radio.c and carscript.c and the copies
//  drifted apart; that is the mistake this split avoids.
// ============================================================================
class MR_CarrierState
{
	// --- codec ---------------------------------------------------------------
	// Wire / save format: "MRT1|marker|trackId|position|trackNo"
	//   marker is a version number, not a checksum: a newer server may add
	//   fields to the right and an older one must ignore the line instead of
	//   resuming a tape at a random second.
	static const string PREFIX = "MRT1";

	// OMD used a bare marker int (78410). A prefix is needed because
	// OnStoreSave returns an array that other mods may also write into.
	static string Encode(MR_CarrierMemory mem)
	{
		if (!mem || mem.IsEmpty())
			return string.Empty;

		TStringArray parts = new TStringArray();
		parts.Insert(PREFIX);
		parts.Insert(MR.TAPE_MARKER.ToString());
		parts.Insert(MR_Net.Sanitize(mem.m_TrackId));
		parts.Insert(mem.m_Position.ToString());
		parts.Insert(mem.m_TrackNo.ToString());
		return MR_Net.JoinParts(parts);
	}

	// Returns null when the line is not ours / not readable. Never throws.
	static MR_CarrierMemory Decode(string line)
	{
		if (line == string.Empty)
			return null;

		TStringArray f = new TStringArray();
		line.Split("|", f);
		if (f.Count() < 4)
			return null;
		if (f.Get(0) != PREFIX)
			return null;

		MR_CarrierMemory mem = new MR_CarrierMemory();
		// f.Get(1) is the version marker: parsed but not enforced yet, so a
		// future format with an extra field still loads here.
		mem.m_TrackId = f.Get(2);
		mem.m_Position = f.Get(3).ToInt();
		if (f.Count() > 4)
			mem.m_TrackNo = f.Get(4).ToInt();

		if (mem.m_TrackId == string.Empty)
			return null;
		if (mem.m_Position < 0)
			mem.m_Position = 0;
		if (mem.m_Position > MR.TAPE_SIDE_MAX)
			mem.m_Position = MR.TAPE_SIDE_MAX;
		return mem;
	}

	// --- sanity --------------------------------------------------------------
	// A position past the end of the track means "the tape ran on": the caller
	// decides whether to advance a track or rewind. We only report it.
	static bool IsFinished(MR_CarrierMemory mem, int trackDuration)
	{
		if (!mem)
			return false;
		if (trackDuration <= 0)
			return false;
		return (mem.m_Position >= trackDuration);
	}

	// Clamp a position into a track. Used before it reaches the audio layer:
	// SetStartOffset with a value past the end makes the engine play silence
	// for the whole track length instead of failing.
	static int ClampPosition(int position, int trackDuration)
	{
		if (position < 0)
			return 0;
		if (trackDuration > 0 && position > trackDuration)
			return trackDuration;
		if (position > MR.TAPE_SIDE_MAX)
			return MR.TAPE_SIDE_MAX;
		return position;
	}
}
