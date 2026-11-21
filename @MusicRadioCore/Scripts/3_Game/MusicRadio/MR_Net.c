// ============================================================================
//  MusicRadio - Core
//  MR_Net.c : wire format. One place so the two sides can never disagree.
//  Layer: 3_Game
//
//  Transport: ScriptRPC with a numeric RPC id (see MR.RPC_*), payload is a
//  single string. Strings are used deliberately:
//    * readable in logs while developing
//    * forward compatible (unknown trailing fields are ignored)
//    * no per-field Param<?> allocations
//  Fields are separated by '\n', records by '\t'. Titles never contain either
//  because MR_Net.Sanitize() strips them.
// ============================================================================

// What the player asked for (client -> server).
enum MR_Action
{
	ACT_UNKNOWN     = 0,
	ACT_PLAY_TRACK  = 1,
	// 2 was ACT_PLAY_STREAM: internet radio is not part of the mod any more
	// (_archive/internet_radio/). The number stays reserved.
	ACT_STOP        = 3,
	ACT_VOLUME      = 4,
	ACT_RADIUS      = 5,
	ACT_NEXT        = 6,
	ACT_PREV        = 7,
	ACT_REQUEST_LIB = 8,
	ACT_NOWPLAYING  = 9,
	ACT_TOGGLE_BROADCAST = 10,
	// 11: play the album that sits in a cassette / MiniDisc player
	//     m_TrackId carries the album id, the server resolves it to tracks
	ACT_PLAY_ALBUM  = 11,
	// 12: the client moved the head inside the current track (seek). The tape
	//     deck reports its own position so the server keeps the authoritative
	//     copy that is written to the carrier's OnStoreSave.
	//     m_Volume carries the position in SECONDS (reuses an existing field,
	//     no wire format change, old servers ignore the action).
	ACT_SEEK        = 12
}

class MR_Request
{
	int    m_Action;
	MR_HostType m_HostType;
	int    m_HostId;
	int    m_HostIdHigh;
	string m_TrackId;
	int    m_Volume;
	float  m_Radius;
	bool   m_Broadcast;
	// Offset (секунды), с которого продолжать плёнку. Поле было в
	// MR_PlayRequest, но сервер работает с MR_Request - и падал с
	//   Can't find variable 'm_StartOffset'   (лог 00:52, 319 и 455)
	// Держим его ровно в одном месте: MR_PlayRequest больше не нужен.
	int    m_StartOffset;

	void MR_Request()
	{
		m_Action = MR_Action.ACT_UNKNOWN;
		m_HostType = MR_HostType.HOST_NONE;
		m_HostId = 0;
		m_HostIdHigh = 0;
		m_TrackId = string.Empty;
		m_Volume = -1;
		m_Radius = -1;
		m_Broadcast = true;
		m_StartOffset = 0;
	}

	string Serialize()
	{
		TStringArray parts = new TStringArray();
		parts.Insert(((int)m_Action).ToString());
		parts.Insert(((int)m_HostType).ToString());
		parts.Insert(m_HostId.ToString());
		parts.Insert(m_HostIdHigh.ToString());
		parts.Insert(MR_Net.Sanitize(m_TrackId));
		parts.Insert(m_Volume.ToString());
		parts.Insert(m_Radius.ToString());
		parts.Insert(MR_Track.BoolJson(m_Broadcast));
		parts.Insert(m_StartOffset.ToString());
		return MR_Net.JoinParts(parts);
	}

	static MR_Request Parse(string payload)
	{
		MR_Request r = new MR_Request();
		if (payload == string.Empty)
			return r;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 4)
			return r;

		r.m_Action     = f.Get(0).ToInt();
		r.m_HostType   = MR.HostTypeFromInt(f.Get(1).ToInt());
		r.m_HostId     = f.Get(2).ToInt();
		r.m_HostIdHigh = f.Get(3).ToInt();

		if (f.Count() > 4) r.m_TrackId   = f.Get(4);
		if (f.Count() > 5) r.m_Volume    = f.Get(5).ToInt();
		if (f.Count() > 6) r.m_Radius    = f.Get(6).ToFloat();
		if (f.Count() > 7) r.m_Broadcast = MR.ToBool(f.Get(7));
		if (f.Count() > 8) r.m_StartOffset = f.Get(8).ToInt();

		return r;
	}
}

class MR_Net
{
	// ---------------------------------------------------------------- helpers
	// Собрать строку из частей. Каждая строка исходника - отдельный
	// оператор: длинные выражения с переносами этот компилятор не принимает
	// (mr_audiotypes.c(118): "Syntax error", даже когда "+" стоит в конце
	// строки). Части добавляются вызовом AddPart.
	static string JoinParts(TStringArray parts)
	{
		string outS = "";
		foreach (string part : parts)
			outS = outS + part;
		return outS;
	}

	static string Sanitize(string input)
	{
		string outS = input;
		outS.Replace("\n", " ");
		outS.Replace("\t", " ");
		outS.Replace("\r", " ");
		return outS;
	}

	// ------------------------------------------------------------- play request
	static string EncodePlay(MR_PlayRequest req)
	{
		if (!req)
			return string.Empty;

		TStringArray parts = new TStringArray();
		parts.Insert("PLAY");
		parts.Insert(((int)req.m_HostType).ToString());
		parts.Insert(req.m_HostId.ToString());
		parts.Insert(req.m_HostIdHigh.ToString());
		// Локатор может быть null (запросы без трека): тогда пустая строка.
		string locText = "";
		if (req.m_Locator)
			locText = req.m_Locator.Serialize();
		parts.Insert(locText);
		parts.Insert(req.m_StartOffset.ToString());
		parts.Insert(req.m_Radius.ToString());
		parts.Insert(req.m_Volume.ToString());
		parts.Insert(MR_Track.BoolJson(req.m_Loop));
		parts.Insert(MR_Track.BoolJson(req.m_Broadcast));
		return MR_Net.JoinParts(parts);
	}

	static MR_PlayRequest DecodePlay(string payload)
	{
		if (payload == string.Empty)
			return null;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 9 || f.Get(0) != "PLAY")
			return null;

		MR_PlayRequest req = new MR_PlayRequest();
		req.m_HostType   = MR.HostTypeFromInt(f.Get(1).ToInt());
		req.m_HostId     = f.Get(2).ToInt();
		req.m_HostIdHigh = f.Get(3).ToInt();
		MR_Locator parsedLoc = MR_Locator.FromString(f.Get(4));
		if (parsedLoc)
			req.m_Locator = parsedLoc;
		req.m_StartOffset = f.Get(5).ToInt();
		req.m_Radius     = f.Get(6).ToFloat();
		req.m_Volume     = f.Get(7).ToInt();
		req.m_Loop       = MR.ToBool(f.Get(8));
		// тернарный оператор отдельной строкой: внутри аргумента/списка
		// Enforce Script отвечает "Broken expression (missing ';'?)"
		if (f.Count() > 9)
			req.m_Broadcast = MR.ToBool(f.Get(9));
		else
			req.m_Broadcast = true;
		return req;
	}

	// ------------------------------------------------------------- stop / misc
	// "STOP\n<type>\n<idLow>\n<idHigh>\n<reason>"
	static string EncodeStop(MR_HostType type, int idLow, int idHigh, MR_StopReason reason)
	{
		TStringArray parts = new TStringArray();
		parts.Insert("STOP");
		parts.Insert(((int)type).ToString());
		parts.Insert(idLow.ToString());
		parts.Insert(idHigh.ToString());
		parts.Insert(((int)reason).ToString());
		return MR_Net.JoinParts(parts);
	}

	static bool DecodeStop(string payload, out MR_HostType type, out int idLow, out int idHigh, out int reason)
	{
		type = MR_HostType.HOST_NONE;
		idLow = 0;
		idHigh = 0;
		reason = 0;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 5 || f.Get(0) != "STOP")
			return false;

		type   = MR.HostTypeFromInt(f.Get(1).ToInt());
		idLow  = f.Get(2).ToInt();
		idHigh = f.Get(3).ToInt();
		reason = f.Get(4).ToInt();
		return true;
	}

	// "VOL\n<type>\n<idLow>\n<idHigh>\n<volume0..10>"
	static string EncodeVolume(MR_HostType type, int idLow, int idHigh, int volume)
	{
		return "VOL\n" + ((int)type).ToString() + "\n" + idLow.ToString() + "\n" + idHigh.ToString() + "\n" + MR.ClampInt(volume, 0, MR.VOLUME_STEPS).ToString();
	}

	static bool DecodeVolume(string payload, out MR_HostType type, out int idLow, out int idHigh, out int volume)
	{
		type = MR_HostType.HOST_NONE;
		idLow = 0;
		idHigh = 0;
		volume = 0;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 5 || f.Get(0) != "VOL")
			return false;

		type   = MR.HostTypeFromInt(f.Get(1).ToInt());
		idLow  = f.Get(2).ToInt();
		idHigh = f.Get(3).ToInt();
		volume = f.Get(4).ToInt();
		return true;
	}

	// "ENDED\n<type>\n<idLow>\n<idHigh>" - the client tells the server the media
	// finished (server is authority for "next track" logic).
	static string EncodeEnded(MR_HostType type, int idLow, int idHigh)
	{
		return "ENDED\n" + ((int)type).ToString() + "\n" + idLow.ToString() + "\n" + idHigh.ToString();
	}

	static bool DecodeEnded(string payload, out MR_HostType type, out int idLow, out int idHigh)
	{
		type = MR_HostType.HOST_NONE;
		idLow = 0;
		idHigh = 0;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 4 || f.Get(0) != "ENDED")
			return false;

		type   = MR.HostTypeFromInt(f.Get(1).ToInt());
		idLow  = f.Get(2).ToInt();
		idHigh = f.Get(3).ToInt();
		return true;
	}

	// "NP\n<title>\n<artist>\n<provider>" - now playing feed for chat / UI
	static string EncodeNowPlaying(string title, string artist, string provider)
	{
		return "NP\n" + Sanitize(title) + "\n" + Sanitize(artist) + "\n" + Sanitize(provider);
	}

	static bool DecodeNowPlaying(string payload, out string title, out string artist, out string provider)
	{
		title = string.Empty;
		artist = string.Empty;
		provider = string.Empty;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 4 || f.Get(0) != "NP")
			return false;

		title    = f.Get(1);
		artist   = f.Get(2);
		provider = f.Get(3);
		return true;
	}

	// "REPLY\n<ok>\n<message>" - server answer to a rejected/accepted request
	static string EncodeReply(bool ok, string message)
	{
		return "REPLY\n" + MR_Track.BoolJson(ok) + "\n" + Sanitize(message);
	}

	static bool DecodeReply(string payload, out bool ok, out string message)
	{
		ok = false;
		message = string.Empty;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 3 || f.Get(0) != "REPLY")
			return false;

		ok = MR.ToBool(f.Get(1));
		message = f.Get(2);
		return true;
	}
}
