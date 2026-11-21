// ============================================================================
//  MusicRadio - Core
//  MR_AudioTypes.c : settings objects and the "play request" that travels
//                    from the server to the clients.
//  Layer: 3_Game
// ============================================================================

// ---------------------------------------------------------------- settings
class MR_Settings
{
	// General
	int    m_Protocol;
	string m_Version;

	// What players may use
	bool   m_EnableVehicleRadio;
	bool   m_EnableHandheldRadio;
	bool   m_RequireRadioItemInCargo;   // vehicles: radio must be in cargo
	bool   m_AllowLocalTracks;          // may players play library tracks
	bool   m_AllowBroadcast;            // sound reaches other players in radius
	bool   m_AllowRadioTransmission;    // handheld radio -> same frequency
	bool   m_AllowRepeat;               // repeat / autoplay next track in a car
	bool   m_RadioNeedsCarrier;         // handheld radio plays only an inserted tape

	// Defaults
	int    m_DefaultVolume;             // 0..10
	float  m_DefaultRadius;             // meters
	int    m_MaxVolume;                 // 0..10
	float  m_MaxRadius;

	// Data
	string m_DbPath;                    // where the DB mod writes its json
	bool   m_EnableExplicit;            // allow "explicit" flagged tracks

	// Anti abuse
	int    m_CooldownSeconds;           // between two play requests per player
	bool   m_LogRequests;

	void MR_Settings()
	{
		Defaults();
	}

	void Defaults()
	{
		m_Protocol               = MR.PROTOCOL;
		m_Version                = MR.VERSION;
		m_EnableVehicleRadio     = true;
		m_EnableHandheldRadio    = true;
		m_RequireRadioItemInCargo = true;
		m_AllowLocalTracks       = true;
		m_AllowBroadcast         = true;
		m_AllowRadioTransmission = true;
		m_AllowRepeat            = true;
		m_RadioNeedsCarrier      = true;
		m_DefaultVolume          = 6;
		m_DefaultRadius          = MR.RADIUS_DEFAULT;
		m_MaxVolume              = MR.VOLUME_STEPS;
		m_MaxRadius              = MR.RADIUS_MAX;
		m_DbPath                 = MR.PREFIX_DB + "/Music/index.json";
		m_EnableExplicit         = false;
		m_CooldownSeconds        = 3;
		m_LogRequests            = true;
	}

	static MR_Settings FromJson(MR_JsonValue root)
	{
		MR_Settings s = new MR_Settings();
		if (!root || root.m_Type != MR_JsonValue.T_OBJECT)
			return s;

		MR_JsonValue g = root.Get("general");
		if (g)
		{
			s.m_DefaultVolume = g.GetInt("defaultVolume", s.m_DefaultVolume);
			s.m_DefaultRadius = g.GetFloat("defaultRadius", s.m_DefaultRadius);
			s.m_MaxVolume     = g.GetInt("maxVolume", s.m_MaxVolume);
			s.m_MaxRadius     = g.GetFloat("maxRadius", s.m_MaxRadius);
			s.m_EnableExplicit = g.GetBool("allowExplicit", s.m_EnableExplicit);
		}

		MR_JsonValue f = root.Get("features");
		if (f)
		{
			s.m_EnableVehicleRadio   = f.GetBool("vehicleRadio", s.m_EnableVehicleRadio);
			s.m_EnableHandheldRadio  = f.GetBool("handheldRadio", s.m_EnableHandheldRadio);
			s.m_RequireRadioItemInCargo = f.GetBool("requireRadioItemInCargo", s.m_RequireRadioItemInCargo);
			s.m_AllowLocalTracks     = f.GetBool("localTracks", s.m_AllowLocalTracks);
			s.m_AllowRepeat          = f.GetBool("autoplayNext", s.m_AllowRepeat);
			s.m_AllowBroadcast       = f.GetBool("broadcastToOthers", s.m_AllowBroadcast);
			s.m_AllowRadioTransmission = f.GetBool("radioTransmission", s.m_AllowRadioTransmission);
			s.m_RadioNeedsCarrier    = f.GetBool("radioNeedsCarrier", s.m_RadioNeedsCarrier);
		}

		MR_JsonValue l = root.Get("limits");
		if (l)
		{
			s.m_CooldownSeconds    = l.GetInt("cooldownSeconds", s.m_CooldownSeconds);
			s.m_LogRequests        = l.GetBool("logRequests", s.m_LogRequests);
		}

		MR_JsonValue d = root.Get("database");
		if (d)
			s.m_DbPath = d.GetString("index", s.m_DbPath);

		// Sanity
		s.m_DefaultVolume = MR.ClampInt(s.m_DefaultVolume, 0, MR.VOLUME_STEPS);
		s.m_MaxVolume     = MR.ClampInt(s.m_MaxVolume, 1, MR.VOLUME_STEPS);
		s.m_DefaultRadius = MR.ClampFloat(s.m_DefaultRadius, MR.RADIUS_MIN, MR.RADIUS_MAX);
		s.m_MaxRadius     = MR.ClampFloat(s.m_MaxRadius, MR.RADIUS_MIN, MR.RADIUS_MAX);

		return s;
	}

	// Ни одна строка не переносится: каждая - отдельный оператор.
	// Перенос выражения (даже с "+" в КОНЦЕ строки) Enforce Script не
	// принимает: mr_audiotypes.c(118) "Syntax error" + "Missing ';'".
	// Поэтому длинный текст собирается по частям обычными присваиваниями.
	void WriteJson(out string text)
	{
		text = "{\n";
		text = text + "  \"general\": {\n";
		text = text + "    \"defaultVolume\": " + m_DefaultVolume.ToString() + ",\n";
		text = text + "    \"defaultRadius\": " + m_DefaultRadius.ToString() + ",\n";
		text = text + "    \"maxVolume\": " + m_MaxVolume.ToString() + ",\n";
		text = text + "    \"maxRadius\": " + m_MaxRadius.ToString() + ",\n";
		text = text + "    \"allowExplicit\": " + MR_Track.BoolJson(m_EnableExplicit) + "\n";
		text = text + "  },\n";
		text = text + "  \"features\": {\n";
		text = text + "    \"vehicleRadio\": " + MR_Track.BoolJson(m_EnableVehicleRadio) + ",\n";
		text = text + "    \"handheldRadio\": " + MR_Track.BoolJson(m_EnableHandheldRadio) + ",\n";
		text = text + "    \"requireRadioItemInCargo\": " + MR_Track.BoolJson(m_RequireRadioItemInCargo) + ",\n";
		text = text + "    \"localTracks\": " + MR_Track.BoolJson(m_AllowLocalTracks) + ",\n";
		text = text + "    \"autoplayNext\": " + MR_Track.BoolJson(m_AllowRepeat) + ",\n";
		text = text + "    \"broadcastToOthers\": " + MR_Track.BoolJson(m_AllowBroadcast) + ",\n";
		text = text + "    \"radioTransmission\": " + MR_Track.BoolJson(m_AllowRadioTransmission) + ",\n";
		text = text + "    \"radioNeedsCarrier\": " + MR_Track.BoolJson(m_RadioNeedsCarrier) + "\n";
		text = text + "  },\n";
		text = text + "  \"limits\": {\n";
		text = text + "    \"cooldownSeconds\": " + m_CooldownSeconds.ToString() + ",\n";
		text = text + "    \"logRequests\": " + MR_Track.BoolJson(m_LogRequests) + "\n";
		text = text + "  },\n";
		text = text + "  \"database\": {\n";
		text = text + "    \"index\": \"" + m_DbPath + "\"\n";
		text = text + "  }\n";
		text = text + "}\n";
	}
}

// ------------------------------------------------------- play request (wire)
// This is what the server decides and what every client executes. It carries
// no file path in a client resolvable form: files are addressed through the
// locator + the client's own copy of the media pack.
class MR_PlayRequest
{
	MR_HostType   m_HostType;      // item / vehicle
	int           m_HostId;        // network low/high id of the host object
	int           m_HostIdHigh;
	// ВАЖНО: ref. Без него движок пишет
	//   mr_audiotypes.c,167: Variable 'm_Locator' is not strong ref
	ref MR_Locator m_Locator;    // provider + track id + seed
	int           m_StartOffset;   // seconds to skip (sync / late join)
	float         m_Radius;        // meters (broadcast)
	int           m_Volume;        // 0..10
	bool          m_Loop;
	bool          m_Broadcast;     // false = only the requesting client hears it

	void MR_PlayRequest()
	{
		m_HostType = MR_HostType.HOST_NONE;
		m_HostId = 0;
		m_HostIdHigh = 0;
		// Локатор создаёт вызывающий код (DecodePlay / сервер), здесь только
		// проверка на null в тех местах, где он используется.
		m_StartOffset = 0;
		m_Radius = MR.RADIUS_DEFAULT;
		m_Volume = 6;
		m_Loop = false;
		m_Broadcast = true;
	}
}

// ------------------------------------------------------------- session state
// One "radio" that is currently playing somewhere in the world. Lives on the
// server (source of truth) and as a lightweight mirror on each client.
class MR_Session
{
	string      m_Id;              // unique session id (host id + time)
	MR_HostType m_HostType;
	int         m_HostId;
	int         m_HostIdHigh;
	string      m_OwnerId;         // boilerplate of the player who started it
	string      m_OwnerName;
	string      m_TrackId;
	string      m_Provider;
	string      m_Title;
	// Album bound to the session when it was started from a cassette / disc
	// ("" for the normal library). Autoplay never leaves this album.
	string      m_AlbumId;
	int         m_StartTimeMs;     // server time when playback started
	int         m_StartOffset;     // media offset at start
	int         m_Duration;        // seconds
	int         m_Volume;
	float       m_Radius;
	bool        m_Loop;
	bool        m_Broadcast;

	void MR_Session()
	{
		m_Id = string.Empty;
		m_HostType = MR_HostType.HOST_NONE;
		m_HostId = 0;
		m_HostIdHigh = 0;
		m_OwnerId = string.Empty;
		m_OwnerName = string.Empty;
		m_TrackId = string.Empty;
		m_Provider = string.Empty;
		m_Title = string.Empty;
		m_AlbumId = string.Empty;
		m_StartTimeMs = 0;
		m_StartOffset = 0;
		m_Duration = 0;
		m_Volume = 6;
		m_Radius = MR.RADIUS_DEFAULT;
		m_Loop = false;
		m_Broadcast = true;
	}

	// Where the media should be right now (seconds).
	int CurrentOffset()
	{
		int elapsed = (GetGame().GetTime() - m_StartTimeMs) / 1000;
		return m_StartOffset + elapsed;
	}

	bool IsFinished()
	{
		if (m_Loop)
			return false;
		if (m_Duration <= 0)
			return false;
		return (CurrentOffset() >= m_Duration);
	}

	string SerializeConfig()
	{
		string s = m_Id + "\n";
		s = s + ((int)m_HostType).ToString() + "\n";
		s = s + m_HostId.ToString() + "\n";
		s = s + m_HostIdHigh.ToString() + "\n";
		s = s + m_OwnerId + "\n";
		s = s + m_OwnerName + "\n";
		s = s + m_TrackId + "\n";
		s = s + m_Provider + "\n";
		s = s + m_Title + "\n";
		s = s + m_StartTimeMs.ToString() + "\n";
		s = s + m_StartOffset.ToString() + "\n";
		s = s + m_Duration.ToString() + "\n";
		s = s + m_Volume.ToString() + "\n";
		s = s + m_Radius.ToString() + "\n";
		s = s + MR_Track.BoolJson(m_Loop) + "\n";
		s = s + MR_Track.BoolJson(m_Broadcast);
		return s;
	}
}
