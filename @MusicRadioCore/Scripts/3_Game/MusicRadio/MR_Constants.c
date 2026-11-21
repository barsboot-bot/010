// ============================================================================
//  MusicRadio - Core
//  MR_Constants.c : all shared constants, enums and small helpers.
//  Layer: 3_Game  (must not reference 4_World / 5_Mission classes)
//  Target: DayZ 1.30+
// ============================================================================

class MR
{
	// --- identity -----------------------------------------------------------
	static const string MOD_NAME        = "MusicRadio";
	static const string VERSION         = "1.0.0";
	static const int    PROTOCOL        = 1;

	// PBO prefixes (must match the folder name inside the @mod folder)
	static const string PREFIX_CORE     = "MusicRadioCore";
	static const string PREFIX_DB       = "MusicRadioDB";

	// --- config / data files ------------------------------------------------
	// Server side  ($mission: -> mpmissions\<map>\)
	static const string DIR_MISSION     = "$mission:MusicRadio/";
	static const string CFG_AUDIO       = "AudioConfig.json";
	static const string CFG_DATABASE    = "DatabaseConfig.json";
	// Client side  ($profile: -> DayZ\profiles\ or client profile dir)
	static const string DIR_PROFILE     = "$profile:MusicRadio/";
	static const string CFG_AUDIO_SAVED = "AudioConfigClient.json";
	static const string LOG_FILE        = "MusicRadio.log";

	// --- playback ------------------------------------------------------------
	static const int    VOLUME_STEPS     = 10;      // 0..10 (10 = 100%)
	static const float  RADIUS_MIN       = 10.0;
	static const float  RADIUS_MAX       = 80.0;
	static const float  RADIUS_DEFAULT   = 40.0;
	static const int    MAX_TRACK_LENGTH = 1800;    // sec, safety net
	static const float  FADE_IN          = 0.8;
	static const float  FADE_OUT         = 0.5;
	static const int    TICK_MS          = 250;     // audio manager tick
	static const int    SERVER_TICK_MS   = 1000;    // server watchdog tick

	// --- tape deck / carrier state (persisted on the cassette itself) --------
	// Pattern taken from OMD_CarRadio: the position lives on the CARRIER, so a
	// tape can be pulled out, carried to another player and resumed there.
	// "Side A 2:17" survives a server restart.
	static const int    TAPE_MARKER      = 78410;   // OnStoreSave version marker
	static const int    TAPE_TICK_MS     = 500;     // position update cadence
	static const int    TAPE_BATTERY_IDLE_MS = 5000; // I/O buffer flush interval

	// Physical tape: 46 minutes per side, so a track can never be longer.
	static const int    TAPE_SIDE_MAX    = 46 * 60;

	// Slot locks: the carrier cannot be pulled out while the deck is spinning.
	// Release is delayed, otherwise the inventory is not ready yet and
	// GetSlotLock returns garbage (same fix as VOS_Radio CarScript.c).
	static const int    SLOT_UNLOCK_DELAY_MS = 1000;

	// Sound set naming contract, implemented by the media pack (DB mod):
	//     MR_<PROVIDER>_<TRACKID>_SoundSet

	// Default (fallback) radio sound set shipped with Core.
	static const string RADIO_FALLBACK_SS = "MR_RadioFallback_SoundSet";

	// --- RPC ids (single base, avoid vanilla ERPC range) ---------------------
	static const int RPC_BASE           = 784100;
	static const int RPC_CONFIG         = 784101;   // server -> client (settings)
	static const int RPC_LIBRARY        = 784102;   // server -> client (tracks)
	static const int RPC_PLAY           = 784103;   // host -> client(s)
	static const int RPC_STOP           = 784104;
	static const int RPC_VOLUME         = 784105;
	static const int RPC_TRACK_ENDED    = 784106;   // host -> client(s)
	static const int RPC_REQUEST        = 784107;   // client -> server (control)
	static const int RPC_REPLY          = 784108;   // server -> single client
	static const int RPC_NOWPLAYING     = 784109;
	static const int RPC_STATE          = 784110;   // late-join sync

	// --- logging -------------------------------------------------------------
	// ------------------------------------------------------------ string kit
	// У string НЕТ StartsWith/EndsWith:
	//   mr_mediaresolver.c,130: Undefined function 'string.StartsWith'
	//   mr_library.c,336:       Undefined function 'string.EndsWith'
	static bool StartsWith(string s, string prefix)
	{
		if (s == string.Empty || prefix == string.Empty)
			return false;
		if (s.Length() < prefix.Length())
			return false;
		return s.Substring(0, prefix.Length()) == prefix;
	}

	static bool EndsWith(string s, string suffix)
	{
		if (s == string.Empty || suffix == string.Empty)
			return false;
		int len = s.Length();
		int slen = suffix.Length();
		if (len < slen)
			return false;
		return s.Substring(len - slen, slen) == suffix;
	}

	// ------------------------------------------------------------ enum kit
	// Приведение "(MR_HostType)x" в присваивании движок считает "не NULL
	// присваиванием" (mr_net.c,84 "Only NULL assignment is allowed").
	// Поэтому конвертация спрятана в функцию: результат уже нужного типа.
	static MR_MediaKind MediaKindFromInt(int value)
	{
		if (value == 1)
			return MR_MediaKind.MEDIA_TRACK;
		return MR_MediaKind.MEDIA_NONE;
	}

	static MR_HostType HostTypeFromInt(int value)
	{
		if (value == 1)
			return MR_HostType.HOST_ITEM;
		if (value == 2)
			return MR_HostType.HOST_VEHICLE;
		if (value == 3)
			return MR_HostType.HOST_SPEAKER;
		if (value == 4)
			return MR_HostType.HOST_PLAYER;
		return MR_HostType.HOST_NONE;
	}

	static void Log(string msg)
	{
		Print("[MusicRadio] " + msg);
	}

	static void LogWarn(string msg)
	{
		Print("[MusicRadio][WARN] " + msg);
	}

	static void LogError(string msg)
	{
		Print("[MusicRadio][ERROR] " + msg);
	}

	// --- math helpers --------------------------------------------------------
	static float Clamp01(float v)
	{
		if (v < 0) return 0;
		if (v > 1) return 1;
		return v;
	}

	// ------------------------------------------------------------------- id
	//  Network id of an object, both halves in one call.
	//
	//  WHY NOT GetNetworkIDLow()/GetNetworkIDHigh(): there is no such method in
	//  Enforce Script. The engine gives exactly one entry point -
	//      proto void Object.GetNetworkID(out int lowBits, out int highBits)
	//  and calling the split names fails at compile time with
	//      Undefined function 'CarScript#3545#5.GetNetworkIDLow'   (log 20:47)
	//  Names are composed at compile time, so ONE wrong call in the base class
	//  breaks every subclass of it: the same error appeared for CarScript,
	//  PersonalRadio and the deck - from a single line. Hence this wrapper:
	//  the API is touched in one place and fixed in one place.
	static void NetId(Object obj, out int idLow, out int idHigh)
	{
		idLow = 0;
		idHigh = 0;
		if (!obj)
			return;
		obj.GetNetworkID(idLow, idHigh);
	}

	static int ClampInt(int v, int lo, int hi)
	{
		if (v < lo) return lo;
		if (v > hi) return hi;
		return v;
	}

	static float ClampFloat(float v, float lo, float hi)
	{
		if (v < lo) return lo;
		if (v > hi) return hi;
		return v;
	}

	// 0..10 user step -> 0..1 gain (perceptual curve, x^1.6)
	static float VolumeStepToGain(int step)
	{
		float t = ClampFloat((float)step / (float)VOLUME_STEPS, 0, 1);
		return Math.Pow(t, 1.6);
	}

	// Distance attenuation: full volume inside 35% of radius, then falls to 0.
	static float DistanceGain(float distance, float radius)
	{
		if (radius <= 0 || distance <= 0)
			return 1.0;

		float inner = radius * 0.35;
		if (distance <= inner)
			return 1.0;
		if (distance >= radius)
			return 0.0;

		float t = (distance - inner) / (radius - inner);
		return 1.0 - t * t;   // soft shoulder
	}

	static string Pad2(int value)
	{
		if (value < 10)
			return "0" + value.ToString();
		return value.ToString();
	}

	static string Safe(string s)
	{
		if (s == string.Empty)
			return "";
		return s;
	}

	static bool ToBool(string s)
	{
		string t = s;
		t.ToLower();
		return (t == "1" || t == "true" || t == "yes");
	}
}

// What kind of host is playing (see MR_DeviceBase / modded CarScript).
enum MR_HostType
{
	HOST_NONE    = 0,
	HOST_ITEM    = 1,   // handheld / portable radio item
	HOST_VEHICLE = 2,   // car, truck, heli ... (needs a radio in cargo)
	HOST_SPEAKER = 3,   // static loudspeaker (optional, future)
	HOST_PLAYER  = 4    // cassette / MD player item (tape = one album)
}

// What kind of media a library entry is.
// Only local .ogg files are supported by design: streaming was evaluated and
// dropped (see _archive/internet_radio/README.md).
enum MR_MediaKind
{
	MEDIA_NONE  = 0,
	MEDIA_TRACK = 1    // .ogg file shipped by a media pack
}

// Why playback stopped - used for UI strings and logging.
enum MR_StopReason
{
	STOP_USER       = 0,
	STOP_ENDED      = 1,
	STOP_TOO_FAR    = 2,
	STOP_HOST_GONE  = 3,
	STOP_REMOVED    = 4,
	STOP_RUINED     = 5,
	STOP_RATE_LIMIT = 6,
	STOP_DENIED     = 7,
	STOP_ERROR      = 8
}
